#include "../include/vcs.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char *argv[]){
    /*if(string(argv[0]) != "Rift" ){
    // Extract just the filename without path
    string programName = fs::path(argv[0]).filename().string();
    
    // Check if it starts with "Rift" (allows for both "Rift" and "Rift.exe")
    if(programName.find("Rift") != 0){
        cerr << "Error: Invalid VCS!! " << std::endl;
        return 1;
    }   */

    // Check for required arguments
    if(argc < 2) {
        cerr << "Error: No command provided" << endl;
        return 1;
    }

    string command1 = argv[1];

    if(argc == 3){
        string command2 = argv[2];
    }

    VCS vcs;

    if(command1 == "init"){
        vcs.init();
    }
    else if(command1 == "add"){
        string filename = argv[2];
        vcs.add(filename);
    }
    else if(command1 == "status"){
        vcs.status();
    }
    else{
        cerr << "Error: Invalid command!! " << endl;
        return 1;
    }
    
    return 0;
}