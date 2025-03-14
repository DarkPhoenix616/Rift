# Compiler and Flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -g -Iinclude -I/opt/homebrew/opt/nlohmann-json/include
LDFLAGS = -L/opt/homebrew/Cellar/openssl@3/3.4.1/lib -lssl -lcrypto
CPPFLAGS = -I/opt/homebrew/Cellar/openssl@3/3.4.1/include -I/opt/homebrew/opt/nlohmann-json/include

# Detect OS
OS := $(shell uname -s)

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

# Install dependencies (cross-platform)
install-dependencies:
ifeq ($(OS), Darwin)  # macOS
	brew install nlohmann-json openssl
endif
ifeq ($(OS), Linux)
	sudo apt-get install -y nlohmann-json3-dev libssl-dev
endif
ifeq ($(OS), Windows_NT)
	@echo "Please install dependencies manually:"
	@echo " - nlohmann-json (https://github.com/nlohmann/json)"
	@echo " - OpenSSL (https://slproweb.com/products/Win32OpenSSL.html)"
endif

# Default rule
all: install-dependencies $(TARGET)

# Build Executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_FILES) $(LDFLAGS)

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Create obj directory if not exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Run executable
run: $(TARGET)
	./$(TARGET)
