#include "../include/vcs.h"
#include "../include/file_manager.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

VCS::VCS(){
    // Initialize the managers by loading data when VCS is created
    if (fs::exists("./data/.vcs")) {
        fileManager.loadFromDisk();
    }
}

void VCS::init(){
    if(!fs::exists("./data") || !fs::exists("./data/.vcs")){
        fs::create_directory("./data");
        fs::create_directory("./data/.vcs");
        fs::create_directory("./data/.vcs/Committed State");
        fs::create_directory("./data/.vcs/Committed State/commits");
        std::ofstream commitFile("./data/.vcs/Committed State/commit_log.json");
        commitFile << "{}";  // Initialize with empty JSON
        commitFile.close();
        
        fs::create_directory("./data/.vcs/Staged State");
        fs::create_directory("./data/.vcs/Modified State");
        
        // Initialize the file history with empty JSON files
        std::ofstream fileHistoryFile("./data/.vcs/Staged State/file_history.json");
        fileHistoryFile << "{}";
        fileHistoryFile.close();
        
        std::ofstream hashMapFile("./data/.vcs/Staged State/hash_map.json");
        hashMapFile << "{}";
        hashMapFile.close();

        fileManager.initializeRepo(); 

        std::cout << "Initialized Rift Repository" << std::endl;
    } 
    else{
        std::cout << "Error: Repository already exists" << std::endl;
    }
}

void VCS::add(const string& filename){
    // Use the class member fileManager instead of creating a new one
    fileManager.loadFromDisk();
    fileManager.addFileVersion(filename);
    fileManager.saveToDisk(); // Make sure to save after adding

    string fileHash = fileManager.getLatestHash(filename);
    cout << "Added " << filename << " to the staging area \nHash Value: " << fileHash << endl;
}   

void VCS::status(){
    // Use the class member
    fileManager.loadFromDisk();
    fileManager.showStatus();
}

void VCS::commit(const std::string& message) {
    std::cout << "DEBUG: Starting commit process..." << std::endl;
    
    // Check if repository exists
    if (!fs::exists("./data/.vcs")) {
        std::cout << "ERROR: Repository not initialized. Run 'Rift init' first." << std::endl;
        return;
    }
    
    // Check if Staged State files exist
    if (!fs::exists("./data/.vcs/Staged State/file_history.json") || 
        !fs::exists("./data/.vcs/Staged State/hash_map.json")) {
        std::cout << "DEBUG: Missing Staged State files." << std::endl;
    } else {
        std::cout << "DEBUG: Staged State files exist." << std::endl;
    }
    
    // Load staged files
    std::cout << "DEBUG: Loading file manager data..." << std::endl;
    fileManager.loadFromDisk();
    
    // Check file_history.json content
    std::ifstream fileHistoryFile("./data/.vcs/Staged State/file_history.json");
    std::string fileHistoryContent((std::istreambuf_iterator<char>(fileHistoryFile)), 
                                  std::istreambuf_iterator<char>());
    std::cout << "DEBUG: file_history.json content: " << fileHistoryContent << std::endl;
    fileHistoryFile.close();

    // Get all file names and their latest hash values
    std::cout << "DEBUG: Getting staged files..." << std::endl;
    std::unordered_map<std::string, std::string> fileVersions = fileManager.getAllStagedFiles();
    
    // Debug output
    std::cout << "DEBUG: Number of staged files: " << fileVersions.size() << std::endl;
    for (const auto& [filename, hash] : fileVersions) {
        std::cout << "DEBUG: Staged file: " << filename << " with hash: " << hash << std::endl;
    }
    
    if(fileVersions.empty()) {
        std::cout << "Nothing to commit, working directory clean" << std::endl;
        return;
    }
    
    // Create commit
    std::cout << "DEBUG: Creating commit..." << std::endl;
    std::string commitHash = commitManager.commit(fileVersions, message);
    
    if(!commitHash.empty()) {
        std::cout << "Created commit " << commitHash << ": " << message << std::endl;
    } else {
        std::cout << "DEBUG: commitManager.commit() returned empty hash" << std::endl;
    }
}

void VCS::log(int limit) {
    commitManager.showLog(limit);
}

void VCS::branch(const std::string& branchName) {
    commitManager.createBranch(branchName);
}

void VCS::checkout(const std::string& branchName) {
    commitManager.switchBranch(branchName);
}

void VCS::branches() {
    std::vector<std::string> branchList = commitManager.getBranches();
    std::string currentBranch = commitManager.getCurrentBranch();
    
    std::cout << "\n----- Branches -----\n" << std::endl;
    for(const auto& branch : branchList) {
        if(branch == currentBranch) {
            std::cout << "* " << branch << " (current)" << std::endl;
        } else {
            std::cout << "  " << branch << std::endl;
        }
    }
}