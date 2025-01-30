#
# Cross Platform Makefile
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#   apt-get install libglfw-dev
#

# Uncomment the appropriate compiler
CXX = g++  # or clang++

EXE = Project.exe

# Set IMGUI_DIR to the current directory
IMGUI_DIR := $(shell pwd)

SOURCES = main.cpp
SOURCES += $(IMGUI_DIR)/imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp
SOURCES += imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl2.cpp
SOURCES += imgui_hex_editor/imgui_hex.cpp
SOURCES += src/HexEditor.cpp

# Generate object files (.o) from source files
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

UNAME_S := $(shell uname -s)

CXXFLAGS = -std=c++11 -Iimgui/backends -Iimgui_hex_editor -Iimgui/examples/example_glfw_opengl2/lib
CXXFLAGS += -g -Wall -Wformat
LIBS =

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL `pkg-config --static --libs glfw3`

	CXXFLAGS += `pkg-config --cflags glfw3`
	CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include -I/imgui_hex_editor -I../../backends -Isrc/  -Iimgui/
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

# Rule to compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: imgui/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: imgui_hex_editor/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Default target: all
all: $(EXE)
	@echo "IMGUI directory is: $(IMGUI_DIR)"
	@echo Build complete for $(ECHO_MESSAGE)

# Link object files to create the final executable
$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

# Clean up generated files
clean:
	rm -f $(EXE) $(OBJS)

