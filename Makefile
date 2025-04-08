# Compiler and Flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -g -Iinclude -I/opt/homebrew/opt/nlohmann-json/include
LDFLAGS = -L/opt/homebrew/Cellar/openssl@3/3.4.1/lib -lssl -lcrypto -lcurl
CPPFLAGS = -I/opt/homebrew/Cellar/openssl@3/3.4.1/include -I/opt/homebrew/opt/nlohmann-json/include

# Directories
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Source and Header Files
SRC_FILES = $(filter-out $(SRC_DIR)/generate_api_key_header.cpp, $(wildcard $(SRC_DIR)/*.cpp))
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)

# Output Executable Name
TARGET = Rift

# Default rule
all: $(TARGET)

include/embedded_api_key.h: .env
	@echo "Generating embedded_api_key.h from .env"
	@echo "#ifndef EMBEDDED_API_KEY_H" > include/embedded_api_key.h
	@echo "#define EMBEDDED_API_KEY_H" >> include/embedded_api_key.h
	@grep GEMINI_API_KEY .env | sed 's/^GEMINI_API_KEY=//' | xargs -I {} echo 'constexpr const char* EMBEDDED_GEMINI_API_KEY = "{}";' >> include/embedded_api_key.h
	@echo "#endif" >> include/embedded_api_key.h


# Build Executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_FILES) $(LDFLAGS)

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) include/embedded_api_key.h | $(OBJ_DIR)
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

# Dependencies
deps:
	@echo "To install dependencies, run: make -f dependencies.mk install"

.PHONY: all clean run deps