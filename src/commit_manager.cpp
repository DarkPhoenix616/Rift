#include "../include/commit_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <openssl/sha.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

const std::string COMMITS_DIR = "./data/.vcs/Committed State/commits";
const std::string COMMIT_LOG_PATH = "./data/.vcs/Committed State/commit_log.json";

// CommitNode constructor
CommitNode::CommitNode(std::string hash, std::string msg, CommitNode* p) 
    : commitHash(hash), message(msg), parent(p) {
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    timestamp = ss.str();
}

// Helper function to generate a commit hash
std::string CommitManager::generateCommitHash(
    const std::unordered_map<std::string, std::string>& fileVersions,
    const std::string& message,
    const std::string& timestamp) {
    
    // Combine all file hashes, message, and timestamp
    std::string combined = message + timestamp;
    for (const auto& [filename, hash] : fileVersions) {
        combined += filename + hash;
    }
    
    // Calculate SHA-256 hash
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)combined.c_str(), combined.size(), hash);
    
    // Convert to hexadecimal string
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    
    return ss.str().substr(0, 8); // Return shorter hash (first 8 chars)
}

// Constructor
CommitManager::CommitManager() : headCommit(nullptr), currentBranch("master") {
    if (!fs::exists(COMMITS_DIR)) {
        fs::create_directories(COMMITS_DIR);
    }
    loadFromDisk();
}

// Destructor
CommitManager::~CommitManager() {
    // Clean up commit nodes to avoid memory leaks
    std::vector<CommitNode*> nodesToDelete;
    
    for (auto& [branch, commit] : branches) {
        CommitNode* current = commit;
        while (current && std::find(nodesToDelete.begin(), nodesToDelete.end(), current) == nodesToDelete.end()) {
            nodesToDelete.push_back(current);
            current = current->parent;
        }
    }
    
    for (auto node : nodesToDelete) {
        delete node;
    }
}

// Save a commit to disk
void CommitManager::saveCommitToDisk(CommitNode* commit) {
    if (!commit) return;
    
    json commitJson;
    
    commitJson["hash"] = commit->commitHash;
    commitJson["message"] = commit->message;
    commitJson["timestamp"] = commit->timestamp;
    commitJson["parent"] = commit->parent ? commit->parent->commitHash : "";
    commitJson["files"] = commit->fileVersions;
    
    std::string commitFilePath = COMMITS_DIR + "/" + commit->commitHash + ".json";
    std::ofstream commitFile(commitFilePath);
    commitFile << commitJson.dump(4);
    commitFile.close();
}

// Save commit log to disk
void CommitManager::saveCommitLogToDisk() {
    json logJson;
    
    logJson["head"] = headCommit ? headCommit->commitHash : "";
    logJson["current_branch"] = currentBranch;
    
    json branchesJson;
    for (const auto& [name, commit] : branches) {
        branchesJson[name] = commit ? commit->commitHash : "";
    }
    logJson["branches"] = branchesJson;
    
    std::ofstream logFile(COMMIT_LOG_PATH);
    logFile << logJson.dump(4);
    logFile.close();
}

// Load commits from disk
void CommitManager::loadFromDisk() {
    if (!fs::exists(COMMIT_LOG_PATH)) {
        // Initialize with master branch and no commits
        branches["master"] = nullptr;
        headCommit = nullptr;
        return;
    }
    
    // Load commit log
    std::ifstream logFile(COMMIT_LOG_PATH);
    json logJson;
    logFile >> logJson;
    logFile.close();
    
    currentBranch = logJson["current_branch"];
    
    // Load all commits into memory
    std::unordered_map<std::string, CommitNode*> commitMap;
    std::unordered_map<std::string, std::string> parentMap; // hash -> parent hash
    
    for (const auto& entry : fs::directory_iterator(COMMITS_DIR)) {
        if (entry.path().extension() == ".json") {
            std::ifstream commitFile(entry.path());
            json commitJson;
            commitFile >> commitJson;
            commitFile.close();
            
            std::string hash = commitJson["hash"];
            std::string message = commitJson["message"];
            std::string parentHash = commitJson["parent"];
            
            CommitNode* node = new CommitNode(hash, message);
            node->timestamp = commitJson["timestamp"];
            
            for (const auto& [filename, fileHash] : commitJson["files"].items()) {
                node->fileVersions[filename] = fileHash;
            }
            
            commitMap[hash] = node;
            if (!parentHash.empty()) {
                parentMap[hash] = parentHash;
            }
        }
    }
    
    // Connect parent pointers
    for (auto& [hash, node] : commitMap) {
        if (parentMap.find(hash) != parentMap.end()) {
            std::string parentHash = parentMap[hash];
            if (commitMap.find(parentHash) != commitMap.end()) {
                node->parent = commitMap[parentHash];
            }
        }
    }
    
    // Set up branches
    for (const auto& [branch, hash] : logJson["branches"].items()) {
        std::string commitHash = hash;
        if (!commitHash.empty() && commitMap.find(commitHash) != commitMap.end()) {
            branches[branch] = commitMap[commitHash];
        } else {
            branches[branch] = nullptr;
        }
    }
    
    // Set head
    std::string headHash = logJson["head"];
    if (!headHash.empty() && commitMap.find(headHash) != commitMap.end()) {
        headCommit = commitMap[headHash];
    } else {
        headCommit = nullptr;
    }
}

// Create a new commit
std::string CommitManager::commit(
    const std::unordered_map<std::string, std::string>& fileVersions, 
    const std::string& message) {
    
    if (fileVersions.empty()) {
        std::cout << "Nothing to commit, working directory clean" << std::endl;
        return "";
    }
    
    // Generate timestamp for hash uniqueness
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    std::string timestamp = ss.str();
    
    // Generate commit hash
    std::string commitHash = generateCommitHash(fileVersions, message, timestamp);
    
    // Create new commit node
    CommitNode* newCommit = new CommitNode(commitHash, message, headCommit);
    newCommit->fileVersions = fileVersions;
    
    // Update head and current branch
    headCommit = newCommit;
    branches[currentBranch] = newCommit;
    
    // Save to disk
    saveCommitToDisk(newCommit);
    saveCommitLogToDisk();
    
    std::cout << "Created commit " << commitHash << ": " << message << std::endl;
    return commitHash;
}

// Show commit log
void CommitManager::showLog(int limit) {
    CommitNode* current = headCommit;
    int count = 0;
    
    std::cout << "\n----- Commit History -----\n" << std::endl;
    
    while (current && (limit == 0 || count < limit)) {
        std::cout << "Commit: " << current->commitHash << std::endl;
        std::cout << "Date:   " << current->timestamp << std::endl;
        std::cout << "Message: " << current->message << "\n" << std::endl;
        
        current = current->parent;
        count++;
    }
    
    if (current && limit > 0) {
        std::cout << "...(more commits exist)..." << std::endl;
    }
}

// Get the head commit
CommitNode* CommitManager::getHead() const {
    return headCommit;
}

// Get current branch
std::string CommitManager::getCurrentBranch() const {
    return currentBranch;
}

// Create a new branch
void CommitManager::createBranch(const std::string& branchName) {
    if (branches.find(branchName) != branches.end()) {
        std::cout << "Branch '" << branchName << "' already exists" << std::endl;
        return;
    }
    
    branches[branchName] = headCommit;
    std::cout << "Created branch: " << branchName << std::endl;
    
    saveCommitLogToDisk();
}

// Switch to a different branch
void CommitManager::switchBranch(const std::string& branchName) {
    if (branches.find(branchName) == branches.end()) {
        std::cout << "Branch '" << branchName << "' does not exist" << std::endl;
        return;
    }
    
    currentBranch = branchName;
    headCommit = branches[branchName];
    std::cout << "Switched to branch: " << branchName << std::endl;
    
    saveCommitLogToDisk();
}

// get all branches
std::vector<std::string> CommitManager::getBranches() const {
    std::vector<std::string> branchList;
    for (const auto& [branch, _] : branches) {
        branchList.push_back(branch);
    }
    return branchList;
}