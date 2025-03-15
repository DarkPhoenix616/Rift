#ifndef COMMIT_MANAGER_H
#define COMMIT_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

class CommitNode {
public:
    std::string commitHash;
    std::string message;
    std::string timestamp;
    std::unordered_map<std::string, std::string> fileVersions; // filename -> hash
    CommitNode* parent;
    
    CommitNode(std::string hash, std::string msg, CommitNode* p = nullptr);
};

class CommitManager {
private:
    CommitNode* headCommit;
    std::string currentBranch;
    std::unordered_map<std::string, CommitNode*> branches; // branch name -> commit pointer
    
    std::string generateCommitHash(const std::unordered_map<std::string, std::string>& fileVersions,
                                 const std::string& message,
                                 const std::string& timestamp);
    void saveCommitToDisk(CommitNode* commit);
    void saveCommitLogToDisk();

public:
    CommitManager();
    ~CommitManager();
    
    void loadFromDisk();
    std::string commit(const std::unordered_map<std::string, std::string>& fileVersions, 
                     const std::string& message);
    void showLog(int limit = 0);
    CommitNode* getHead() const;
    std::string getCurrentBranch() const;
    void createBranch(const std::string& branchName);
    void switchBranch(const std::string& branchName);
    std::vector<std::string> getBranches() const;
};

#endif