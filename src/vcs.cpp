#include "../include/vcs.h"
#include "../include/file_manager.h"
#include "../include/commit_manager.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>

//using namespace std;
namespace fs = std::filesystem;

VCS::VCS(){
    //constructor 
}

void VCS::init(){
    if(!fs::exists("./data") || !fs::exists("./data/.vcs")){
        fs::create_directory("./data");
        fs::create_directory("./data/.vcs");
        fs::create_directory("./data/.vcs/main");
        fs::create_directory("./data/.vcs/main/Committed State");
        //std::ofstream commitFile("./data/.vcs/main/Committed State/commits.log");
        fs::create_directory("./data/.vcs/main/Staged State");
        //fs::create_directory("./data/.vcs/Modified State");
        //commitFile.close();

        FileHistoryManager fileHistoryManager;
        fileHistoryManager.initializeRepo(); 

        std::cout << "Initialized Rift Repository" << std::endl;
    } 
    else{
        std::cout << "Error: Repository already exists" << std::endl;
    }
}

void VCS::status(){
    FileHistoryManager fileHistoryManager;
    fileHistoryManager.loadFromDisk(fileHistoryManager.fileHistoryMapStaged, fileHistoryManager.hashMapStaged); // First we transfer the data from the JSON files to the maps
    CommitManager CommitManager(fileHistoryManager);
    fileHistoryManager.showStatus();
}
void VCS::config(const std::string& option, const std::string& value) {
    if (option == "--set-api-key") {
        geminiHelper.setApiKey(value);
        std::cout << "Gemini API key has been set successfully." << std::endl;
    } else if (option == "--check-api-key") {
        if (geminiHelper.hasApiKey()) {
            std::cout << "Gemini API key is configured." << std::endl;
        } else {
            std::cout << "Gemini API key is not configured. Use 'Rift config --set-api-key <key>' to set it." << std::endl;
        }
    } else {
        std::cout << "Unknown configuration option: " << option << std::endl;
        std::cout << "Available options: --set-api-key, --check-api-key" << std::endl;
    }
}

void VCS::suggestCommands(const std::string& invalidCommand) {
    if (!geminiHelper.hasApiKey()) {
        std::cout << "Command not recognized. To get intelligent suggestions, set your Gemini API key:" << std::endl;
        std::cout << "  Rift config --set-api-key <your-api-key>" << std::endl;
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
    FileHistoryManager fileHistoryManager;
    fileHistoryManager.loadFromDisk(fileHistoryManager.fileHistoryMapStaged, fileHistoryManager.hashMapStaged);    // Loading the previously stored data
    fileHistoryManager.addFileVersion(filename);

    string fileHash = fileHistoryManager.getLatestHashStaged(filename);
    cout << "Added " << filename << " to the staging area \nHash Value: "<<fileHash << endl;
    
}   

void VCS::commit(const string& message){
    FileHistoryManager fileHistoryManager;
    fileHistoryManager.loadFromDisk(fileHistoryManager.fileHistoryMapStaged, fileHistoryManager.hashMapStaged);    // Loading the previously stored data
    if(fileHistoryManager.fileHistoryMapStaged.size() == 0){
        std::cout << "Error: No files to commit" << std::endl;
        return;
    }
    CommitManager commitManager(fileHistoryManager);
    commitManager.addCommit(message, fileHistoryManager);
}

void VCS::displayCommitHistory(string branch){
    FileHistoryManager fileHistoryManager;
    fileHistoryManager.loadFromDisk(fileHistoryManager.fileHistoryMapCommitted, fileHistoryManager.hashMapCommitted);    // Loading the previously stored data
    CommitManager commitManager(fileHistoryManager);
    commitManager.displayCommitHistory(branch);
}