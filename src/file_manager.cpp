#include "../include/file_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <openssl/sha.h>
#include <iomanip>

FileManager::FileManager() {
    repoPath = "./data";
    vcsDir = repoPath + "/.vcs";
    stagedFilesPath = vcsDir + "/staged.txt";
}

void FileManager::initialize() {
    if (!fs::exists(repoPath)) {
        fs::create_directory(repoPath);
    }
    
    if (!fs::exists(vcsDir)) {
        fs::create_directory(vcsDir);
        fs::create_directory(vcsDir + "/objects");
    }
    
    if (!fs::exists(stagedFilesPath)) {
        std::ofstream stagedFile(stagedFilesPath);
        stagedFile.close();
    }
}

std::string FileManager::hashFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file for hashing: " << filePath << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, content.c_str(), content.size());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    
    return ss.str();
}

bool FileManager::saveFileObject(const std::string& filePath, const std::string& hash) {
    std::string objectsDir = vcsDir + "/objects";
    if (!fs::exists(objectsDir)) {
        fs::create_directory(objectsDir);
    }
    
    std::string objectPath = objectsDir + "/" + hash;
    
    if (fs::exists(objectPath)) {
        return true;
    }
    
    std::ifstream src(filePath, std::ios::binary);
    std::ofstream dst(objectPath, std::ios::binary);
    
    if (!src || !dst) {
        std::cerr << "Error: Could not copy file to object storage" << std::endl;
        return false;
    }
    
    dst << src.rdbuf();
    return true;
}

bool FileManager::isIgnored(const std::string& filePath) {
    if (filePath.find(vcsDir) == 0) {
        return true;
    }
    
    std::string filename = fs::path(filePath).filename().string();
    
    std::vector<std::string> ignorePatterns = {
        ".git", ".DS_Store", "*.o", "*.exe", "*.out", "*.log"
    };
    
    for (const auto& pattern : ignorePatterns) {
        if (pattern[0] == '*') {
            std::string ext = pattern.substr(1);
            if (filename.size() >= ext.size() && 
                filename.substr(filename.size() - ext.size()) == ext) {
                return true;
            }
        } else if (pattern == filename) {
            return true;
        }
    }
    
    return false;
}

bool FileManager::stageFile(const std::string& filePath) {
    if (!fs::exists(filePath)) {
        std::cerr << "Error: File does not exist: " << filePath << std::endl;
        return false;
    }
    
    if (isIgnored(filePath)) {
        std::cerr << "Warning: File is ignored: " << filePath << std::endl;
        return false;
    }
    
    std::string hash = hashFile(filePath);
    if (hash.empty()) {
        return false;
    }
    
    if (!saveFileObject(filePath, hash)) {
        return false;
    }
    
    std::ofstream stagedFile(stagedFilesPath, std::ios::app);
    if (!stagedFile) {
        std::cerr << "Error: Could not open staged files list" << std::endl;
        return false;
    }
    
    stagedFile << filePath << " " << hash << std::endl;
    
    std::cout << "Added file to staging area: " << filePath << std::endl;
    return true;
}

bool FileManager::stageAll() {
    bool success = true;
    
    for (const auto& entry : fs::recursive_directory_iterator(repoPath)) {
        if (fs::is_regular_file(entry)) {
            std::string filePath = entry.path().string();
            
            if (!isIgnored(filePath)) {
                success &= stageFile(filePath);
            }
        }
    }
    
    return success;
}

std::vector<std::pair<std::string, std::string>> FileManager::getStatus() {
    std::vector<std::pair<std::string, std::string>> fileStatuses;
    
    std::unordered_map<std::string, std::string> currentIndex;
    for (const auto& entry : fs::recursive_directory_iterator(repoPath)) {
        if (fs::is_regular_file(entry)) {
            std::string filePath = entry.path().string();
            
            if (!isIgnored(filePath)) {
                std::string hash = hashFile(filePath);
                currentIndex[filePath] = hash;
                
                if (fileIndex.find(filePath) == fileIndex.end()) {
                    fileStatuses.push_back({filePath, "new"});
                } else if (fileIndex[filePath] != hash) {
                    fileStatuses.push_back({filePath, "modified"});
                } else {
                    fileStatuses.push_back({filePath, "unchanged"});
                }
            }
        }
    }
    
    for (const auto& file : fileIndex) {
        if (currentIndex.find(file.first) == currentIndex.end()) {
            fileStatuses.push_back({file.first, "deleted"});
        }
    }
    
    return fileStatuses;
}

std::vector<std::string> FileManager::getStagedFiles() {
    std::vector<std::string> stagedFiles;
    
    std::ifstream stagedFilesList(stagedFilesPath);
    if (!stagedFilesList) {
        std::cerr << "Error: Could not open staged files list" << std::endl;
        return stagedFiles;
    }
    
    std::string line;
    while (std::getline(stagedFilesList, line)) {
        size_t spacePos = line.find(' ');
        if (spacePos != std::string::npos) {
            std::string filePath = line.substr(0, spacePos);
            stagedFiles.push_back(filePath);
        }
    }
    
    return stagedFiles;
}

bool FileManager::isModified(const std::string& filePath) {
    if (!fs::exists(filePath)) {
        return false;
    }
    
    std::string currentHash = hashFile(filePath);
    
    return fileIndex.find(filePath) != fileIndex.end() && 
           fileIndex[filePath] != currentHash;
}

bool FileManager::restoreFile(const std::string& filePath, const std::string& commitHash) {
    return false;
}