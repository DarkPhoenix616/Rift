#ifndef BRANCH_MANAGER_H
#define BRANCH_MANAGER_H

#include<string>

class BranchManager{
public: 
    // this creates a new branch with seperate staging and committed directories 
    void createBranch(const std::string& branchName);
    // switches to active branch 
    void switchBranch(const std::string& branchName);
    // Returns to the current active branch (reads from the HEAD file)
    std::string getCurrentBranch();
    // sets the current active branch (writes to the HEAD file)
    void setCurrentBranch(const std::string& branchName);
    // Checks whether a branch already exists 
    bool branchExists(const std::string& branchName);
};

#endif