#ifndef VCS_H
#define VCS_H

#include "commit_manager.h"
#include "file_manager.h"
#include "gemini_helper.h"

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
        
        // Configuration methods
        void config(const std::string& option, const std::string& value);
        void suggestCommands(const std::string& invalidCommand);
        bool hasApiKey() const;
};

#endif