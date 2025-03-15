#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

struct FileVersion{     // Node of the doubly linkedlist storing the file versions
    string hash;
    FileVersion* prev;
    FileVersion* next;
    FileVersion(string h): hash(h), prev(nullptr), next(nullptr) {}
};

class FileHistoryManager {
    private:
        unordered_map<string, FileVersion*> fileHistoryMap;   // Key: File_name , Value: Latest version of the file
        unordered_map<string, string> hashMap;                // Key: Hash of the file, Value: File Content
    public:
        void addFileVersion(const std::string& filename);
        std::string getLatestHash(const std::string& filename);
        bool isFileModified(const std::string& filename);
        void showStatus();
        void loadFromDisk();
        void saveToDisk();
        void initializeRepo();

        std::unordered_map<std::string, std::string> getAllStagedFiles();

};

#endif