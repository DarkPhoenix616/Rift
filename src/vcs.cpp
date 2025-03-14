#include "../include/vcs.h"
#include "../include/file_manager.h"

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