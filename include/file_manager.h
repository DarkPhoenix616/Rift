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
    public:
        unordered_map<string, FileVersion*> fileHistoryMapInitial;   // Gets filled when repo is iniitalized
        unordered_map<string, FileVersion*> fileHistoryMapStaged;   // Key: File_name , Value: Latest version of the file
        unordered_map<string, string> hashMapStaged;                // Key: Hash of the file, Value: File Content
        unordered_map<string, FileVersion*> fileHistoryMapCommitted;  
        unordered_map<string, string> hashMapCommitted;                


        void addFileVersion(const std::string& filename);
        string getLatestHashStaged(const string& filename);
        string getLatestHashCommitted(const string& filename);
        bool isFileModified(const std::string& filename);
        bool isFileStaged(const std::string& filename);
        void showStatus();
        void loadFromDisk(unordered_map<string, FileVersion*>& fileHistoryMap, unordered_map<string, string>& hashMap, string currentBranch);
        void saveToDisk(unordered_map<string, FileVersion*>& fileHistoryMap, unordered_map<string, string>& hashMap, string currentBranch);
        void initializeRepo();
        string base64_encode(const std::string &data);
        string base64_decode(const std::string &encoded);
        //void updateForNewFiles(std::vector<std::string>& untrackedFiles);
};

#endif