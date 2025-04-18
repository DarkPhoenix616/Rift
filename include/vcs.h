#ifndef VCS_H
#define VCS_H

#include "commit_manager.h"
#include "file_manager.h"
#include "gemini_helper.h"
#include "branch_manager.h"

#include <string>
#include <unordered_map>
#include <vector>

class VCS{
    private:
        GeminiHelper geminiHelper;
        
    public:
        VCS();

        void init();
        void status();
        void add(const string& filename);
        void commit(const string& message);
        void displayCommitHistory(string branch);
        void createBranch(const string &branchName);
        void switchBranch(const string &branchName);

        
        void suggestCommands(const std::string& invalidCommand);
        bool hasApiKey() const;
};

#endif