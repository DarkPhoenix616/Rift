# Compiler and Flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude
LDFLAGS = -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto
CPPFLAGS = -I/opt/homebrew/opt/openssl@3/include

# Directories
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Source and Header Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)

# Output Executable Name
TARGET = Rift

# Default rule
all: $(TARGET)

# Build Executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_FILES) $(LDFLAGS)

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create obj directory if not exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Run executable
run: $(TARGET)
	./$(TARGET)
