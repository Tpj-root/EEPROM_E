#ifndef HEX_EDITOR_H
#define HEX_EDITOR_H

#include <GLFW/glfw3.h>
#include "imgui.h"
#include <cstddef>

// Constants
#define FILE_SIZE 256
#define MAX_INPUT_LEN 5 // For 3-digit hex values + null terminator
#define NUM_INPUT_BOXES 1

// Global variables
extern unsigned char originalData[FILE_SIZE];
extern unsigned char editedData[FILE_SIZE];
extern int selectedOffset;
extern int searchedOffset;
extern char inputOffset[NUM_INPUT_BOXES][MAX_INPUT_LEN];
extern char inputValue[NUM_INPUT_BOXES][MAX_INPUT_LEN];
extern char searchValue[MAX_INPUT_LEN];
extern bool searchResult[FILE_SIZE];

// Function declarations
bool LoadBinaryFile(const char* filename, unsigned char* data, size_t size);
bool SaveBinaryFile(const char* filename, unsigned char* data, size_t size);
void DrawHexEditor(const char* title, unsigned char* data, int& selectedOffset, int searchedOffset, bool* searchResult);
void DrawTooltip(int offset);
void HandleReplace();
void HandleJumpToOffset();
void HandleSearchBytes();
void HandleSearchAndReplaceBytes();

#endif // HEX_EDITOR_H
