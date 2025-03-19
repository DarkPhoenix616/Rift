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
    cout << "  commit log                    - Show commit history\n";
    cout << "  branch <name>          - Create a new branch\n";
    cout << "  checkout <branch>      - Switch to a branch\n";
    cout << "  branches               - List all branches\n";
    cout << "  help                   - Display this help message\n";
}


int main(int argc, char *argv[]){
    /*if(string(argv[0]) != "Rift" ){
        cerr << "Error: Invalid VCS!! " << std::endl;
        return 1;
    }   */

    string command1 = argv[1];
    string command2, command3;

    if(argc >= 3)
        command2 = argv[2];
    
    if(argc >= 4)
        command3 = argv[3];

    VCS vcs;
    
    if (command1 == "init") {
        vcs.init();
    } else if (command1 == "status") {
        vcs.status();
    } else if (command1 == "add") {
        string filename = argv[2];
        vcs.add(filename);
    } else if (command1 == "commit") {
        if(command2 == "-m"){
            string message = command3;
            vcs.commit(message);
        }
        else if(command2 == "log"){
            vcs.displayCommitHistory("main");
        }
        else{
            cerr << "Error: Invalid command!! " << endl;
            return 1;
        }
    }  else if(command1 == "help"){
        displayHelp();
    }
    else {
        cerr << "Error: Invalid command!! " << endl;
        return 1;
    }

}

