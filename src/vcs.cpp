#include "../include/vcs.h"
#include "../include/file_manager.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

VCS::VCS(){
    //constructor 
}

void VCS::init(){
    if(!fs::exists("./data") || !fs::exists("./data/.vcs")){
        fs::create_directory("./data");
        fs::create_directory("./data/.vcs");
        fs::create_directory("./data/.vcs/Committed State");
        std::ofstream commitFile("./data/.vcs/Committed State/commits.log");
        fs::create_directory("./data/.vcs/Staged State");
        fs::create_directory("./data/.vcs/Modified State");
        commitFile.close();

        FileHistoryManager fileHistoryManager;
        fileHistoryManager.initializeRepo(); 

        std::cout << "Initialized Rift Repository" << std::endl;
    } 
    else{
        std::cout << "Error: Repository already exists" << std::endl;
    }
}

void VCS::add(const string& filename){
    FileHistoryManager fileHistoryManager;
    fileHistoryManager.loadFromDisk();    // Loading the previously stored data
    fileHistoryManager.addFileVersion(filename);

    string fileHash = fileHistoryManager.getLatestHash(filename);
    cout << "Added " << filename << " to the staging area \nHash Value: "<<fileHash << endl;
    
}   

void VCS::status(){
    FileHistoryManager fileHistoryManager;
    fileHistoryManager.loadFromDisk(); // First we transfer the data from the JSON files to the maps
    fileHistoryManager.showStatus();
}
void VCS::commit(const std::string& message) {
    // Load staged files
    fileManager.loadFromDisk();
    
    // Get all file names and their latest hash values
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
    std::string commitHash = commitManager.commit(fileVersions, message);
    
    if(!commitHash.empty()) {
        std::cout << "Created commit " << commitHash << ": " << message << std::endl;
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