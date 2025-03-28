# Detect OS
OS := $(shell uname -s)

# Install dependencies (Windows using MSYS2)
install:
ifeq ($(OS), MINGW64_NT)
	@echo "Installing dependencies using MSYS2 (MinGW)..."
	pacman -S --noconfirm mingw-w64-x86_64-nlohmann-json mingw-w64-x86_64-openssl mingw-w64-x86_64-curl
endif

.PHONY: install