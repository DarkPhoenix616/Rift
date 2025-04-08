# Detect OS
OS := $(shell uname -s)

# Install dependencies (cross-platform)
install:
ifeq ($(OS), Darwin)  # macOS
	brew install nlohmann-json openssl curl
endif
ifeq ($(OS), Linux)
	sudo apt-get install -y nlohmann-json3-dev libssl-dev libcurl4-openssl-dev
endif
ifeq ($(OS), Windows_NT)
	@echo "Please install dependencies manually:"
	@echo " - nlohmann-json (https://github.com/nlohmann/json)"
	@echo " - OpenSSL (https://slproweb.com/products/Win32OpenSSL.html)"
	@echo " - libcurl (https://curl.se/windows/)"
endif

.PHONY: install