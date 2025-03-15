#include "../include/vcs.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

void displayHelp() {
    cout << "Rift - A simple version control system\n\n";
    cout << "Commands:\n";
    cout << "  init                   - Initialize a new repository\n";
    cout << "  add <file>             - Add a file to the staging area\n";
    cout << "  status                 - Show the status of files\n";
    cout << "  commit -m \"message\"    - Commit staged changes\n";
    cout << "  log                    - Show commit history\n";
    cout << "  branch <name>          - Create a new branch\n";
    cout << "  checkout <branch>      - Switch to a branch\n";
    cout << "  branches               - List all branches\n";
    cout << "  help                   - Display this help message\n";
}

int main(int argc, char *argv[]){
    // Check for required arguments
    if(argc < 2) {
        cerr << "Error: No command provided" << endl;
        displayHelp();
        return 1;
    }

    string command = argv[1];
    VCS vcs;

    if(command == "init"){
        vcs.init();
    }
    else if(command == "add"){
        if(argc < 3) {
            cerr << "Error: No file specified" << endl;
            return 1;
        }
        string filename = argv[2];
        vcs.add(filename);
    }
    else if(command == "status"){
        vcs.status();
    }
    else if(command == "commit"){
        if(argc < 4 || string(argv[2]) != "-m") {
            cerr << "Error: Commit requires a message (-m \"message\")" << endl;
            return 1;
        }
        string message = argv[3];
        vcs.commit(message);
    }
    else if(command == "log"){
        int limit = 0;
        if(argc > 2) {
            try {
                limit = stoi(argv[2]);
            } catch(...) {
                // If conversion fails, show all commits
                limit = 0;
            }
        }
        vcs.log(limit);
    }
    else if(command == "branch"){
        if(argc < 3) {
            cerr << "Error: No branch name specified" << endl;
            return 1;
        }
        string branchName = argv[2];
        vcs.branch(branchName);
    }
    else if(command == "checkout"){
        if(argc < 3) {
            cerr << "Error: No branch name specified" << endl;
            return 1;
        }
        string branchName = argv[2];
        vcs.checkout(branchName);
    }
    else if(command == "branches"){
        vcs.branches();
    }
    else if(command == "help"){
        displayHelp();
    }
    else{
        cerr << "Error: Invalid command: " << command << endl;
        displayHelp();
        return 1;
    }
    
    return 0;
}