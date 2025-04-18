#include "../include/vcs.h"
#include "../include/file_manager.h"
#include "../include/commit_manager.h"
#include "../include/branch_manager.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <unistd.h>

//using namespace std;
namespace fs = std::filesystem;

VCS::VCS(){
    //constructor 
}

void VCS::init(){
    BranchManager branchManager;
    if(!fs::exists("./data") || !fs::exists("./data/.vcs")){
        fs::create_directory("./data");
        fs::create_directory("./data/.vcs");
        fs::create_directory("./data/.vcs/main");
        fs::create_directory("./data/.vcs/main/Committed State");
        //std::ofstream commitFile("./data/.vcs/main/Committed State/commits.log");
        fs::create_directory("./data/.vcs/main/Staged State");


        FileHistoryManager fileHistoryManager;
        fileHistoryManager.initializeRepo(); 

        std::cout << "Initialized Rift Repository" << std::endl;

        branchManager.setCurrentBranch("main");
        std::cout << "Created main branch" << std::endl;
    } 
    else{
        std::cout << "Error: Repository already exists" << std::endl;
    }
}

void VCS::status(){
    BranchManager BranchManager;
    string currentBranch = BranchManager.getCurrentBranch();
    FileHistoryManager fileHistoryManager;
    fileHistoryManager.loadFromDisk(fileHistoryManager.fileHistoryMapStaged, fileHistoryManager.hashMapStaged, currentBranch); // First we transfer the data from the JSON files to the maps
    CommitManager CommitManager(fileHistoryManager);
    fileHistoryManager.showStatus();
}

void VCS::suggestCommands(const std::string& invalidCommand) {
    if (!geminiHelper.hasApiKey()) {
        std::cout << "Command not recognized. To get intelligent suggestions, generate your API key and set it in your .env file" << std::endl;
        return;
    }
    
    std::cout << "Command not recognized. Fetching suggestions..." << std::endl;
    std::vector<std::string> suggestions = geminiHelper.getSuggestions(invalidCommand);
    
    if (suggestions.empty()) {
        std::cout << "No suggestions available. Valid commands include:" << std::endl;
        std::cout << "  - init: Initialize a new repository" << std::endl;
        std::cout << "  - status: Show the current status of the repository" << std::endl;
        std::cout << "  - add <filename>: Add a file to the staging area" << std::endl;
        std::cout << "  - commit -m <message>: Commit changes with a message" << std::endl;
        std::cout << "  - commit log: Show commit history" << std::endl;
        std::cout << "  - branch create <branch_name> - To create a new branch of the name <branch_name>" << std::endl;
        std::cout << "  - branch switch <branch_name>" << std::endl;
    } else {
        std::cout << "Did you mean:" << std::endl;
        for (const auto& suggestion : suggestions) {
            std::cout << "  - " << suggestion << std::endl;
        }
    }
}

bool VCS::hasApiKey() const {
    return geminiHelper.hasApiKey();
}

void VCS::add(const string& filename){
    BranchManager branchManager;
    string currentBranch = branchManager.getCurrentBranch();

    FileHistoryManager fileHistoryManager;
    fileHistoryManager.loadFromDisk(fileHistoryManager.fileHistoryMapStaged, fileHistoryManager.hashMapStaged, currentBranch);    // Loading the previously stored data
    fileHistoryManager.addFileVersion(filename);

    string fileHash = fileHistoryManager.getLatestHashStaged(filename);
    cout << "Added " << filename << " to the staging area \nHash Value: "<<fileHash << endl;
    
}   

void VCS::commit(const string& message){
    BranchManager branchManager;
    string currentBranch = branchManager.getCurrentBranch();

    FileHistoryManager fileHistoryManager;
    fileHistoryManager.loadFromDisk(fileHistoryManager.fileHistoryMapStaged, fileHistoryManager.hashMapStaged, currentBranch);    // Loading the previously stored data
    if(fileHistoryManager.fileHistoryMapStaged.size() == 0){
        std::cout << "Error: No files to commit" << std::endl;
        return;
    }
    CommitManager commitManager(fileHistoryManager);
    commitManager.addCommit(message, fileHistoryManager);
}

void VCS::displayCommitHistory(string branch){
    BranchManager branchManager;
    string currentBranch = branchManager.getCurrentBranch();
    FileHistoryManager fileHistoryManager;
    fileHistoryManager.loadFromDisk(fileHistoryManager.fileHistoryMapCommitted, fileHistoryManager.hashMapCommitted, currentBranch);    // Loading the previously stored data
    CommitManager commitManager(fileHistoryManager);
    commitManager.displayCommitHistory(branch);
}

void VCS::createBranch(const std::string &branchName) {
    BranchManager branchManager;
    branchManager.createBranch(branchName);
}

void VCS::switchBranch(const std::string &branchName) {
    BranchManager branchManager;
    branchManager.switchBranch(branchName);
}