#ifdef _WIN32
#define _ENABLE_EXTENDED_ALIGNED_STORAGE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef byte
#endif

#include "../include/vcs.h"
#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>

using namespace std;

void printUsage() {
    cout << "Usage: Rift <command> [options]" << endl;
    cout << "Available commands:" << endl;
    cout << "  init                        Initialize a new repository" << endl;
    cout << "  status                      Show the status of the repository" << endl;
    cout << "  add <filename>              Add a file to the staging area (use '.' for all files)" << endl;
    cout << "  commit -m <message>         Commit changes with a message" << endl;
    cout << "  commit log                  Show commit history" << endl;
    cout << "  config --set-api-key <key>  Set your Gemini API key for command suggestions" << endl;
    cout << "  config --check-api-key      Check if Gemini API key is configured" << endl;
    cout << "  help                        Show this help message" << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    string command1 = argv[1];
    string command2, command3;

    if (argc >= 3)
        command2 = argv[2];
    
    if (argc >= 4)
        command3 = argv[3];

    // Initialize curl for making API requests
    curl_global_init(CURL_GLOBAL_ALL);
    
    VCS vcs;
    
    if (command1 == "init") {
        vcs.init();
    } else if (command1 == "status") {
        vcs.status();
    } else if (command1 == "add") {
        if (argc < 3) {
            cerr << "Error: Missing filename. Usage: Rift add <filename>" << endl;
            return 1;
        }
        string filename = argv[2];
        vcs.add(filename);
    } else if (command1 == "commit") {
        if (command2 == "-m") {
            if (argc < 4) {
                cerr << "Error: Missing commit message. Usage: Rift commit -m <message>" << endl;
                return 1;
            }
            string message = command3;
            vcs.commit(message);
        } else if (command2 == "log") {
            vcs.displayCommitHistory("main");
        } else {
            cerr << "Error: Invalid commit command. Use 'Rift commit -m <message>' or 'Rift commit log'" << endl;
            return 1;
        }
    } else if (command1 == "config") {
        if (argc < 4 && command2 != "--check-api-key") {
            cerr << "Error: Missing configuration value. Usage: Rift config <option> <value>" << endl;
            return 1;
        }
        
        if (command2 == "--set-api-key") {
            vcs.config(command2, command3);
        } else if (command2 == "--check-api-key") {
            vcs.config(command2, "");
        } else {
            cerr << "Error: Unknown configuration option: " << command2 << endl;
            return 1;
        }
    } else if (command1 == "help") {
        printUsage();
    } else {
        vcs.suggestCommands(command1);
        return 1;
    }

    // Clean up curl resources
    curl_global_cleanup();
    
    return 0;
}