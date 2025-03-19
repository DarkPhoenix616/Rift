#ifndef VCS_H
#define VCS_H

#include "commit_manager.h"
#include "file_manager.h"

#include <string>
#include <unordered_map>

class VCS{
    private:
        FileHistoryManager fileManager;
        CommitManager commitManager;
        
    public:
        VCS();

        /*void status();
        void commit(const std::string& message);
        void log(int limit = 0);
        void branch(const std::string& branchName);
        void checkout(const std::string& branchName);
        void branches();*/
  
        void init();
        void status();
        void add(const string& filename);
        void commit(const string& message);
        void displayCommitHistory(string branch);

};

#endif