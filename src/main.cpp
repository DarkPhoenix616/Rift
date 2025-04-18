#include "../include/vcs.h"
#include "../include/branch_manager.h"

#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>

using namespace std;

void printUsage();

int main(int argc, char *argv[]) {
    // Debugging: print all args
    if (argc < 2) {
        cerr << "Error: No command provided.\n";
        printUsage();
        return 1;
    }

    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);

    string command1 = argv[1];
    string command2 = (argc >= 3) ? argv[2] : "";
    string command3 = (argc >= 4) ? argv[3] : "";

    VCS vcs;

    if (command1 == "init") {
        vcs.init();

    } else if (command1 == "status") {
        vcs.status();

    } else if (command1 == "add") {
        if (command2.empty()) {
            cerr << "Error: No filename provided for add command.\n";
        } else {
            vcs.add(command2);
        }

    } else if (command1 == "commit") {
        if (command2 == "-m" && !command3.empty()) {
            vcs.commit(command3);
        } else if (command2 == "log") {
            vcs.displayCommitHistory("main");
        } else {
            cerr << "Error: Invalid commit command. Use: Rift commit -m <message> or Rift commit log\n";
        }

    } else if (command1 == "branch") {
        if (command2 == "create" && !command3.empty()) {
            BranchManager branchManager;
            branchManager.createBranch(command3);
        } else if (command2 == "switch" && !command3.empty()) {
            BranchManager branchManager;
            branchManager.switchBranch(command3);
        } else {
            cerr << "Usage: Rift branch [create|switch] <branch-name>\n";
        }

    } else if (command1 == "help") {
        printUsage();

    } else {
        cerr << "Error: Invalid command!!\n";
        vcs.suggestCommands(command1);
    }

    // Cleanup curl
    curl_global_cleanup();
    return 0;
}

void printUsage() {
    cout << "Usage: Rift <command> [options]\n";
    cout << "Available commands:\n";
    cout << "  init                         Initialize a new repository\n";
    cout << "  status                       Show the status of the repository\n";
    cout << "  add <filename>               Add a file to staging (use '.' for all files)\n";
    cout << "  commit -m <message>          Commit changes with a message\n";
    cout << "  commit log                   Show commit history\n";
    cout << "  branch create <name>         Create a new branch\n";
    cout << "  branch switch <name>         Switch to another branch\n";
    cout << "  help                         Show this help message\n";
}
