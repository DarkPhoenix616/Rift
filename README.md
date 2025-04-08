# Rift – Navigate the Flow. Control the Change.

Rift is a fast and flexible version control system that makes branching, tracking, and collaboration effortless. Stay in control of your code, experiment freely, and never lose progress.


Rift is a lightweight version control system inspired by Git. This repository contains the implementation of Rift, allowing users to track changes in their files.

## 📥 Cloning the Repository

To get started, clone this repository using the following command:

```sh
git clone https://github.com/DarkPhoenix616/Rift
```

After cloning, navigate into the project directory:

```sh
cd rift
```

## ⚙️ Building the Project

This project uses a `Makefile` to compile the source code and a 'dependencies.mk' to install the required dependencies. For Ios, to build the executable, run:

```sh
make -f dependencies.mk install
```

```sh
make
```

For windows, replace the Makefiles with these: 
```
# Detect OS
OS := $(shell uname -s)

# Install dependencies (Windows using MSYS2)
install:
ifeq ($(OS), MINGW64_NT)
	@echo "Installing dependencies using MSYS2 (MinGW)..."
	pacman -S --noconfirm mingw-w64-x86_64-nlohmann-json mingw-w64-x86_64-openssl
endif

.PHONY: install
```

```
# Compiler and Flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -g -Iinclude -IC:/nlohmann-json/include
LDFLAGS = -LC:/OpenSSL/lib -lssl -lcrypto -lcurl
CPPFLAGS = -IC:/OpenSSL/include -IC:/nlohmann-json/include

# Directories
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Source and Header Files
SRC_FILES = $(filter-out $(SRC_DIR)/generate_api_key_header.cpp, $(wildcard $(SRC_DIR)/*.cpp))
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)

# Output Executable Name
TARGET = Rift.exe

# Default rule
all: $(TARGET)

include/embedded_api_key.h: .env
	@echo Generating embedded_api_key.h from .env
	@echo "#ifndef EMBEDDED_API_KEY_H" > include/embedded_api_key.h
	@echo "#define EMBEDDED_API_KEY_H" >> include/embedded_api_key.h
	@grep GEMINI_API_KEY .env | sed "s/^GEMINI_API_KEY=//" | xargs -I {} echo "constexpr const char* EMBEDDED_GEMINI_API_KEY = \"{}\";" >> include/embedded_api_key.h
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

```

To run, 
```
mingw32-make -f dependencies.mk install  # Install dependencies
mingw32-make                             # Compile the code
mingw32-make run                         # Run the executable
```


This will generate the `Rift` executable in the project directory.

## 🚀 Running Rift

Once compiled, you can use Rift without the `./` prefix by moving it to `/usr/local/bin`:

```sh
sudo mv Rift /usr/local/bin/
```

Now you can run Rift from anywhere in the terminal!

## 📌 Initializing a Rift Repository

To start using Rift, initialize a repository in your project directory:

```sh
Rift init
```

## 📄 Available Commands

- `Rift init` - Initializes a new Rift repository.
- `Rift status` - Detects changes in the files and shows the status of your directory
- `Rift add <filename>` - To add a specific filename to the staging area ('.' to add all the modified files)
- `Rift commit -m <message>` - To create a commit (adding the staged files into the Committed State)
- `Rift commit log` - To log the commit history from the most recent to the past ones


## 🛠 Cleaning Up

To remove the compiled executable:

```sh
make clean
```

## 🤝 Contributing

Feel free to contribute by submitting issues or pull requests.

---


