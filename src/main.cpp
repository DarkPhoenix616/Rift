#include "../include/vcs.h"
#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>

using namespace std;

void printUsage();

int main(int argc, char *argv[]){
    /*if(string(argv[0]) != "Rift" ){
        cerr << "Error: Invalid VCS!! " << std::endl;
        return 1;
    }   */
    VCS vcs;

    // Initialize curl for making API requests
    curl_global_init(CURL_GLOBAL_ALL);

    string command1 = argv[1];
    string command2, command3;

    if(argc >= 3)
        command2 = argv[2];
    
    if(argc >= 4)
        command3 = argv[3];

    
    
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
    } else if (command1 == "help") {
        printUsage();
    }
    else {
        cerr << "Error: Invalid command!! " << endl;
        vcs.suggestCommands(command1);
        return 1;
    }

    // Clean up curl resources
    curl_global_cleanup();

    return 0;

}

void printUsage() {
    cout << "Usage: Rift <command> [options]" << endl;
    cout << "Available commands:" << endl;
    cout << "  init                        Initialize a new repository" << endl;
    cout << "  status                      Show the status of the repository" << endl;
    cout << "  add <filename>              Add a file to the staging area (use '.' for all files)" << endl;
    cout << "  commit -m <message>         Commit changes with a message" << endl;
    cout << "  commit log                  Show commit history" << endl;
    cout << "  help                        Show this help message" << endl;
}

