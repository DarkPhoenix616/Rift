#include "../include/vcs.h"

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
    if(!fs::exists("./data/.vcs")){
        fs::create_directory("./data/.vcs");
        std::ofstream commitFile("./data/.vcs/commits.log");
        commitFile.close();
        std::cout << "Initialized Rift Repository" << std::endl;
    } 
    else{
        std::cout << "Error: Repository already exists" << std::endl;
    }
}