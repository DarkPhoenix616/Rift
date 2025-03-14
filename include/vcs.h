#ifndef VCS_H
#define VCS_H

#include "commit_manager.h"
#include "file_manager.h"

#include <string>
#include <unordered_map>

class VCS{
    private:
        
    public:
        VCS();

        void init();
        void add(const string& filename);
        void status();
};

#endif