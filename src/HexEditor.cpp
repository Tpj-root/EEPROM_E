#include "HexEditor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Config.h"
#include <cstring>  // For memset



// Initialize global variables
unsigned char originalData[FILE_SIZE] = { 0 };
unsigned char editedData[FILE_SIZE] = { 0 };
int selectedOffset = -1;
int searchedOffset = -1;
char inputOffset[NUM_INPUT_BOXES][MAX_INPUT_LEN] = { "" };
char inputValue[NUM_INPUT_BOXES][MAX_INPUT_LEN] = { "" };
char searchValue[MAX_INPUT_LEN] = "";
bool searchResult[FILE_SIZE] = { false };

// Function to load binary file
bool LoadBinaryFile(const char* filename, unsigned char* data, size_t size) {
    FILE* file = fopen(filename, "rb");
    if (!file) return false;
    fread(data, 1, size, file);
    fclose(file);
    return true;
}

// Function to save binary file
bool SaveBinaryFile(const char* filename, unsigned char* data, size_t size) {
    FILE* file = fopen(filename, "wb");
    if (!file) return false;
    fwrite(data, 1, size, file);
    fclose(file);
    return true;
}

// Function to draw hex editor view
void DrawHexEditor(const char* title, unsigned char* data, int& selectedOffset, int searchedOffset, bool* searchResult) {
    ImGui::Text("%s", title);
    ImGui::Separator();

    for (int i = 0; i < FILE_SIZE; i += 16) {
        // Display offset
        ImGui::Text("%04X: ", i);

        // Display hex values
        for (int j = 0; j < 16; j++) {
            int offset = i + j;
            if (offset >= FILE_SIZE) break;

            // Highlight selected offset in green
            if (offset == selectedOffset) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255)); // Green text
            }
            // Highlight searched offset in yellow
            else if (offset == searchedOffset) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255)); // Yellow text
            }
            // Highlight search results in orange
            else if (searchResult[offset]) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 165, 0, 255)); // Orange text
            }

            ImGui::SameLine();
            ImGui::Text("%02X", data[offset]);

            if (offset == selectedOffset || offset == searchedOffset || searchResult[offset]) {
                ImGui::PopStyleColor();
            }

            // Add space after 8 bytes
            if (j == 7) ImGui::SameLine();

            // Tooltip for offset value
            if (ImGui::IsItemHovered()) {
                DrawTooltip(offset);
            }
        }

        // Display ASCII values
        ImGui::SameLine();
        ImGui::Text("| ");
        for (int j = 0; j < 16; j++) {
            int offset = i + j;
            if (offset >= FILE_SIZE) break;

            char c = (data[offset] >= 32 && data[offset] <= 126) ? data[offset] : '.';
            ImGui::SameLine();
            ImGui::Text("%c", c);
        }

        ImGui::NewLine();
    }
}

// Function to draw tooltip for offset value
void DrawTooltip(int offset) {
    ImGui::SetItemTooltip("Offset: 0x%04X", offset);
}

// Function to handle replace button
void HandleReplace() {
    for (int i = 0; i < NUM_INPUT_BOXES; i++) {
        int offset = strtol(inputOffset[i], NULL, 16);
        int value = strtol(inputValue[i], NULL, 16);
        if (offset >= 0 && offset < FILE_SIZE && value >= 0 && value <= 0xFF) {
            editedData[offset] = (unsigned char)value;
            selectedOffset = offset;
        }
    }
}

// Function to handle jump to offset
void HandleJumpToOffset() {
    static char offsetInput[MAX_INPUT_LEN] = "";
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("Jump to Offset (Hex)", offsetInput, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    if (ImGui::Button("Jump")) {
        int offset = strtol(offsetInput, NULL, 16);
        if (offset >= 0 && offset < FILE_SIZE) {
            searchedOffset = offset;
        }
    }
}

// Function to handle search for 1-byte, 2-byte, 3-byte, or 4-byte sequences
void HandleSearchBytes() {
    static char searchValue[MAX_INPUT_LEN] = "";
    static char searchValue2[MAX_INPUT_LEN] = "";
    static char searchValue3[MAX_INPUT_LEN] = "";
    static char searchValue4[MAX_INPUT_LEN] = "";

    // Input fields for up to 4 bytes
    ImGui::Text("Search Hex Value (1-4 Bytes)");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##SearchHexValue1", searchValue, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##SearchHexValue2", searchValue2, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##SearchHexValue3", searchValue3, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##SearchHexValue4", searchValue4, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    if (ImGui::Button("Search Bytes")) {
        memset(searchResult, 0, sizeof(searchResult)); // Clear previous search results

        // Convert input hex values to integers, default to 0 if empty
        int value1 = (searchValue[0] != '\0') ? strtol(searchValue, NULL, 16) : 0;
        int value2 = (searchValue2[0] != '\0') ? strtol(searchValue2, NULL, 16) : 0;
        int value3 = (searchValue3[0] != '\0') ? strtol(searchValue3, NULL, 16) : 0;
        int value4 = (searchValue4[0] != '\0') ? strtol(searchValue4, NULL, 16) : 0;

        // Determine the number of bytes to search based on input
        int numBytes = 0;
        if (searchValue[0] != '\0') numBytes++;
        if (searchValue2[0] != '\0') numBytes++;
        if (searchValue3[0] != '\0') numBytes++;
        if (searchValue4[0] != '\0') numBytes++;

        // Perform the search based on the number of bytes
        if (numBytes >= 1 && numBytes <= 4) {
            for (int i = 0; i < FILE_SIZE - (numBytes - 1); i++) {
                bool match = true;
                if (numBytes >= 1 && editedData[i] != (unsigned char)value1) match = false;
                if (numBytes >= 2 && editedData[i + 1] != (unsigned char)value2) match = false;
                if (numBytes >= 3 && editedData[i + 2] != (unsigned char)value3) match = false;
                if (numBytes >= 4 && editedData[i + 3] != (unsigned char)value4) match = false;

                if (match) {
                    for (int j = 0; j < numBytes; j++) {
                        searchResult[i + j] = true;
                    }
                }
            }
        }
    }
}

// Function to handle search and replace bytes for 1-4 byte sequences
void HandleSearchAndReplaceBytes() {
    static char searchValue1[MAX_INPUT_LEN] = "";
    static char searchValue2[MAX_INPUT_LEN] = "";
    static char searchValue3[MAX_INPUT_LEN] = "";
    static char searchValue4[MAX_INPUT_LEN] = "";

    static char replaceValue1[MAX_INPUT_LEN] = "";
    static char replaceValue2[MAX_INPUT_LEN] = "";
    static char replaceValue3[MAX_INPUT_LEN] = "";
    static char replaceValue4[MAX_INPUT_LEN] = "";

    // Input fields for up to 4 bytes to search and replace
    ImGui::Text("Search Hex Value (1-4 Bytes) and Replace");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##SearchHexValue1", searchValue1, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##SearchHexValue2", searchValue2, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##SearchHexValue3", searchValue3, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##SearchHexValue4", searchValue4, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    
    ImGui::Text("Replace Hex Value (1-4 Bytes)");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##ReplaceHexValue1", replaceValue1, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##ReplaceHexValue2", replaceValue2, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##ReplaceHexValue3", replaceValue3, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
    ImGui::InputText("##ReplaceHexValue4", replaceValue4, MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);

    ImGui::SameLine();
    if (ImGui::Button("Search and Replace Bytes")) {
        memset(searchResult, 0, sizeof(searchResult)); // Clear previous search results

        // Convert input hex values to integers for search and replace, default to 0 if empty
        int searchVal1 = (searchValue1[0] != '\0') ? strtol(searchValue1, NULL, 16) : 0;
        int searchVal2 = (searchValue2[0] != '\0') ? strtol(searchValue2, NULL, 16) : 0;
        int searchVal3 = (searchValue3[0] != '\0') ? strtol(searchValue3, NULL, 16) : 0;
        int searchVal4 = (searchValue4[0] != '\0') ? strtol(searchValue4, NULL, 16) : 0;

        int replaceVal1 = (replaceValue1[0] != '\0') ? strtol(replaceValue1, NULL, 16) : 0;
        int replaceVal2 = (replaceValue2[0] != '\0') ? strtol(replaceValue2, NULL, 16) : 0;
        int replaceVal3 = (replaceValue3[0] != '\0') ? strtol(replaceValue3, NULL, 16) : 0;
        int replaceVal4 = (replaceValue4[0] != '\0') ? strtol(replaceValue4, NULL, 16) : 0;

        // Determine the number of bytes to search and replace based on input
        int numBytes = 0;
        if (searchValue1[0] != '\0') numBytes++;
        if (searchValue2[0] != '\0') numBytes++;
        if (searchValue3[0] != '\0') numBytes++;
        if (searchValue4[0] != '\0') numBytes++;

        // Perform the search and replace
        if (numBytes >= 1 && numBytes <= 4) {
            for (int i = 0; i < FILE_SIZE - (numBytes - 1); i++) {
                bool match = true;
                if (numBytes >= 1 && editedData[i] != (unsigned char)searchVal1) match = false;
                if (numBytes >= 2 && editedData[i + 1] != (unsigned char)searchVal2) match = false;
                if (numBytes >= 3 && editedData[i + 2] != (unsigned char)searchVal3) match = false;
                if (numBytes >= 4 && editedData[i + 3] != (unsigned char)searchVal4) match = false;

                // If a match is found, replace the bytes
                if (match) {
                    for (int j = 0; j < numBytes; j++) {
                        if (j == 0) editedData[i + j] = (unsigned char)replaceVal1;
                        if (j == 1) editedData[i + j] = (unsigned char)replaceVal2;
                        if (j == 2) editedData[i + j] = (unsigned char)replaceVal3;
                        if (j == 3) editedData[i + j] = (unsigned char)replaceVal4;
                    }
                }
            }
        }
    }
}
