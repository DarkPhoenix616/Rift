# Compiler and Flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -g -Iinclude -IC:/msys64/mingw64/include/nlohmann-json
LDFLAGS = -LC:/msys64/mingw64/lib -lssl -lcrypto -lcurl
CPPFLAGS = -IC:/msys64/mingw64/include

# Directories
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Source and Header Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)

# Output Executable Name
TARGET = Rift.exe

# Default rule
all: $(TARGET)

# Build Executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_FILES) $(LDFLAGS)

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Create obj directory if not exists
$(OBJ_DIR):
	mkdir $(OBJ_DIR)

# Clean build files
clean:
	del /Q /F $(OBJ_DIR)\*.o $(TARGET) 2>nul || exit 0
	rmdir /S /Q $(OBJ_DIR) 2>nul || exit 0

# Run executable
run: $(TARGET)
	$(TARGET)

# Dependencies
deps:
	@echo "To install dependencies, run: mingw32-make -f dependencies.mk install"

.PHONY: all clean run deps