#include "../include/branch_manager.h"
#include <filesystem>
#include <fstream>
#include <iostream>

// Create a shorthand alias for the filesystem namespace.
namespace fs = std::filesystem;

// Define the base VCS directory and the HEAD file path.
const std::string VCS_DIR = "./data/.vcs/";
const std::string HEAD_FILE = VCS_DIR + "HEAD";

// Create a branch by setting up its own staging and committed directories.
void BranchManager::createBranch(const std::string& branchName) {
    std::string branchPath = VCS_DIR + branchName;
    
    if (fs::exists(branchPath)) {
        std::cout << "Branch '" << branchName << "' already exists." << std::endl;
        return;
    }

    // Create the branch folder and its subdirectories.
    fs::create_directory(branchPath);
    fs::create_directory(branchPath + "/Staged State");
    fs::create_directory(branchPath + "/Committed State");

    std::cout << "Branch '" << branchName << "' created." << std::endl;
}

// Switch to the desired branch after checking that it exists.
void BranchManager::switchBranch(const std::string& branchName) {
    if (!branchExists(branchName)) {
        std::cout << "Error: Branch '" << branchName << "' does not exist." << std::endl;
        return;
    }

    setCurrentBranch(branchName);
    std::cout << "Switched to branch '" << branchName << "'." << std::endl;
}

// Reads the HEAD file to find out the currently active branch.
// Defaults to "main" if no branch is recorded.
std::string BranchManager::getCurrentBranch() {
    std::ifstream headFile(HEAD_FILE);
    std::string currentBranch;
    if (headFile.is_open()) {
        std::getline(headFile, currentBranch);
        headFile.close();
    }
    return currentBranch.empty() ? "main" : currentBranch;
}

// Writes the given branch name to the HEAD file.
void BranchManager::setCurrentBranch(const std::string& branchName) {
    std::ofstream headFile(HEAD_FILE);
    if (headFile.is_open()) {
        headFile << branchName;
        headFile.close();
    } else {
        std::cerr << "Error: Unable to open HEAD file for writing." << std::endl;
    }
}

// Checks whether a branch folder exists in the VCS directory.
bool BranchManager::branchExists(const std::string& branchName) {
    return fs::exists(VCS_DIR + branchName);
}
