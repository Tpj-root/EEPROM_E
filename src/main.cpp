#include "HexEditor.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <GLFW/glfw3.h>
#include <cstdio>  // For fprintf and stderr
#include "Config.h"

int main(int argc, char** argv) {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Create a window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "EEPROM Editor", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    // Load the original binary file
    if (!LoadBinaryFile("KeyProgramming_Study/BIN_FILES/Toyota_/Corolla/AT93C56_EEPROM_BEST.bin", originalData, FILE_SIZE)) {
        fprintf(stderr, "Failed to load EEPROM.bin\n");
        return -1;
    }
    memcpy(editedData, originalData, FILE_SIZE); // Copy original data to edited data

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main window
        ImGui::Begin("EEPROM Editor");

        // Display original and edited data side by side
        ImGui::Columns(2, "HexView", true);
        DrawHexEditor("Original Data", originalData, selectedOffset, searchedOffset, searchResult);
        ImGui::NextColumn();
        
        // Add Reset Button above the edited data
        if (ImGui::Button("Reset Edited Data")) {
            // Copy original data back to edited data
            memcpy(editedData, originalData, FILE_SIZE);
        
            // Clear search results and selected offset
            memset(searchResult, 0, sizeof(searchResult));
            selectedOffset = -1;
            searchedOffset = -1;
        }
        
        // Display edited data
        DrawHexEditor("Edited Data", editedData, selectedOffset, searchedOffset, searchResult);
        ImGui::Columns(1);
        // Input for offset and value (8 boxes)
        ImGui::Separator();
        for (int i = 0; i < NUM_INPUT_BOXES; i++) {
            ImGui::PushID(i);
            ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
            ImGui::InputText("Offset (Hex)", inputOffset[i], MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(TEXT_WIDTH);  // Use global width
            ImGui::InputText("Value (Hex)", inputValue[i], MAX_INPUT_LEN, ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::PopID();
        }

        // Replace button
        if (ImGui::Button("Replace")) {
            HandleReplace();
        }

        // Jump to offset
        HandleJumpToOffset();

        // Search functionality
        // In the main loop
        ImGui::Separator();
        HandleSearchBytes(); // Function to handle search for 1-byte, 2-byte, 3-byte, or 4-byte sequences
        ImGui::Separator();
        HandleSearchAndReplaceBytes(); // Function to handle search for 1-byte, 2-byte, 3-byte, or 4-byte sequences


        // Save button
        if (ImGui::Button("Save As New File")) {
            if (SaveBinaryFile("EEPROM_edited.bin", editedData, FILE_SIZE)) {
                ImGui::OpenPopup("Save Success");
            } else {
                ImGui::OpenPopup("Save Failed");
            }
        }

        // Popup for save status
        if (ImGui::BeginPopupModal("Save Success", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("File saved successfully as EEPROM_edited.bin");
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupModal("Save Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Failed to save file.");
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
