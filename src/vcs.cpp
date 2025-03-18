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