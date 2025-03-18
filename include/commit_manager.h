#ifndef COMMIT_MANAGER_H
#define COMMIT_MANAGER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include "file_manager.h"
#include <ctime>
#include <ctime>

using namespace std;

struct Commit {
    string message;
    string commitHash;
    unordered_map<string , string> filesCommitted;
    Commit* prev;
    Commit* next;
    string commitTime;

    Commit(string m, string c, string commitTime): message(m), commitHash(c), commitTime(commitTime), prev(nullptr), next(nullptr)  {}
};

class CommitManager{
    private:
        Commit* head;   // Points to the first commit
        Commit* tail;   // Points to the latest commit

        void saveCommitsToDisk();

        string generateCommitHash(const string& message);  // Generating commit hash using message and time

    public:
        // unordered_map<string, FileVersion*> fileHistoryMapCommitted;  
        // unordered_map<string, string> hashMapCommitted;    

        CommitManager(FileHistoryManager& fileHistoryManager);   // Constructor to initialize the commit manager
        ~CommitManager();   // Destructor to free memory

        void loadCommitsFromDisk(FileHistoryManager& fileHistoryManager);
        
        
        void addCommit(const string& message, FileHistoryManager& fileHistoryManager);  

        void displayCommitHistory(string branch);

};

#endif