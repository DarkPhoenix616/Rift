#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

class FileManager {
private:
    std::string repoPath;
    std::string vcsDir;
    std::string stagedFilesPath;
    
    std::unordered_map<std::string, std::string> fileIndex;
    
    std::string hashFile(const std::string& filePath);
    
    bool saveFileObject(const std::string& filePath, const std::string& hash);
    
    bool isIgnored(const std::string& filePath);
    
public:
    FileManager();
    
    void initialize();
    
    bool stageFile(const std::string& filePath);
    
    bool stageAll();
    
    std::vector<std::pair<std::string, std::string>> getStatus();
    
    bool restoreFile(const std::string& filePath, const std::string& commitHash);
    
    std::vector<std::string> getStagedFiles();
    
    bool isModified(const std::string& filePath);
};

#endif