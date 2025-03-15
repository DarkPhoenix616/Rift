#include "../include/file_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <openssl/sha.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;
namespace fs = std::filesystem;

const string INITIAL_FILE_HISTORY_PATH = "./data/.vcs/initial_file_history.json";
const string STAGED_FILE_HISTORY_PATH = "./data/.vcs/Staged State/file_history.json";
const string STAGED_HASH_MAP_PATH = "./data/.vcs/Staged State/hash_map.json";
const string COMMITTED_FILE_HISTORY_PATH = "./data/.vcs/Committed State/file_history.json";
const string COMMITTED_HASH_MAP_PATH = "./data/.vcs/Committed State/hash_map.json";

#define RED     "\033[31m"
#define RESET   "\033[0m"
#define GREEN   "\033[32m"

string calculateFileHash(const string& content) {    // Receiving file content in the binary form
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)content.c_str(), content.size(), hash);    // Assigning a hash value to the file content
    
    std::ostringstream hashStream;
    for (unsigned char c : hash) {
        hashStream << std::hex << (int)c;    // Converting the hash value to a hexadecimal string
    } 
    return hashStream.str();
}

string readFileContent(const string& filename) {
    std::ifstream file(filename);
    if (!file) 
        return "";
    
    std::ostringstream ss;
    ss << file.rdbuf();

    return ss.str();         // Returning the content of the file
}


// Base64 encoding function
/* Basically, JSON expects valid UTF-8 strings so if a file has invalid characters, it crashes,
so we use this function to encode the file content in Base64 before storing it in JSON.
This ensures compatibility with binary files and prevents crashes due to invalid UTF-8 characters*/
string base64_encode(const std::string &data) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    int val = 0, valb = -6;
    for (unsigned char c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (encoded.size() % 4) {
        encoded.push_back('=');
    }
    return encoded;
}

string base64_decode(const std::string &encoded) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

    std::string decoded;
    int val = 0, valb = -8;
    for (unsigned char c : encoded) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return decoded;
}

// Function to save data to JSON files
void FileHistoryManager::saveToDisk(unordered_map<string, FileVersion*>& fileHistoryMap, unordered_map<string, string>& hashMap) {
    json fileHistoryJson;
    json hashMapJson;

    for (const auto& [filename, version] : fileHistoryMap) {
        fileHistoryJson[filename] = version->hash;
    }

    // Store file contents with Base64 encoding before storing
    for (const auto& [hash, content] : hashMap) {
        hashMapJson[hash] = base64_encode(content); 
    }


    std::ofstream fileHistoryFile(STAGED_FILE_HISTORY_PATH);
    std::ofstream hashMapFile(STAGED_HASH_MAP_PATH);

    if (fileHistoryFile && hashMapFile) {
        fileHistoryFile << fileHistoryJson.dump(4);
        hashMapFile << hashMapJson.dump(4);
    } else {
        std::cerr << "Error opening file for saving repository data." << std::endl;
    }

    fileHistoryFile.close();
    hashMapFile.close();
}

// Function to load data from JSON files
void FileHistoryManager::loadFromDisk(unordered_map<string, FileVersion*>& fileHistoryMap, unordered_map<string, string>& hashMap) {
    std::ifstream fileHistoryIn(STAGED_FILE_HISTORY_PATH);
    std::ifstream hashMapIn(STAGED_HASH_MAP_PATH);
    std::ifstream fileHistoryInitialIn(INITIAL_FILE_HISTORY_PATH);

    json initialFileHistoryJson;
    fileHistoryInitialIn >> initialFileHistoryJson;
    fileHistoryInitialIn.close();

    // Load initial file history 
    for (auto &entry : initialFileHistoryJson.items()) {
        std::string filename = entry.key();
        std::string fileHash = entry.value();

        
        fileHistoryMapInitial[filename] = new FileVersion(fileHash);
    }

    if (!fileHistoryIn || !hashMapIn) {
        std::cerr << "No previous repository data found." << std::endl;
        return;
    }

    json fileHistoryJson, hashMapJson;
    fileHistoryIn >> fileHistoryJson;
    hashMapIn >> hashMapJson;

    fileHistoryIn.close();
    hashMapIn.close();

    // Load staged file history
    for (auto &entry : fileHistoryJson.items()) {
        std::string filename = entry.key();
        std::string fileHash = entry.value();
        fileHistoryMap[filename] = new FileVersion(fileHash);
    }

    // Load hash map
    for (auto &entry : hashMapJson.items()) {
        std::string fileHash = entry.key();
        std::string encodedContent = entry.value();
        std::string originalContent = base64_decode(encodedContent);
        hashMap[fileHash] = originalContent;
    }

    std::cout << "Loaded repository data from disk." << std::endl;
}



void FileHistoryManager::initializeRepo() {    // Iterates the entire repository and allots initial hash values to the files
    json fileHistoryJson, hashMapJson;

    for (const auto &entry : fs::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();

            // Ignore files inside the .vcs directory
            if (filePath.find("./data/.vcs") != std::string::npos) continue;

            std::string content = readFileContent(filePath);
            if (content.empty()) continue; // Ignore empty files

            std::string fileHash = calculateFileHash(content);
            std::string relativePath = filePath.substr(2); // Remove "./" from path

            fileHistoryMapInitial[relativePath] = new FileVersion(fileHash);

            // Store in JSON with Base64 encoding
            fileHistoryJson[relativePath] = fileHash;
            hashMapJson[fileHash] = base64_encode(content);
        }
    }

    // Save to JSON files
    std::ofstream fileHistoryOut(INITIAL_FILE_HISTORY_PATH);
    fileHistoryOut << fileHistoryJson.dump(4);
    fileHistoryOut.close();

    /*std::ofstream fileHistoryOut(STAGED_FILE_HISTORY_PATH);
    fileHistoryOut << fileHistoryJson.dump(4);
    fileHistoryOut.close();

    std::ofstream hashMapOut(STAGED_HASH_MAP_PATH);
    hashMapOut << hashMapJson.dump(4);
    hashMapOut.close();

    std::ofstream fileHistoryOut(COMMITTED_FILE_HISTORY_PATH);
    fileHistoryOut << fileHistoryJson.dump(4);
    fileHistoryOut.close();

    std::ofstream hashMapOut(COMMITTED_HASH_MAP_PATH);
    hashMapOut << hashMapJson.dump(4);
    hashMapOut.close();*/

    std::cout << "Scanned and stored initial file versions." << std::endl;
}



void FileHistoryManager::addFileVersion(const string& filename) {  //Adds the new version of the file to the doubly linked list
    
    if(filename == "."){
        initializeRepo();
        vector<string> modified;

        for (const auto& entry : fileHistoryMapInitial) {
            std::string filename = entry.first;
            if(isFileModified(filename)) {
                modified.push_back(filename);
            }
        }

        for(const string file: modified) {
            addFileVersion(file);
        }
        std::cout << "Staged all modified files.\n";
        return;
    }
    string content = readFileContent(filename);
    if(content.empty()){
        std::cerr << "No content in the file!! : " <<filename << std::endl;
        return;
    }
    string newHash = calculateFileHash(content);

    if (fileHistoryMapStaged.find(filename) != fileHistoryMapStaged.end()) {   // CHECKING IF THE FILE ALREADY EXISTS
        FileVersion* lastVersion = fileHistoryMapStaged[filename];
        if (lastVersion->hash == newHash) {
            std::cout << filename << " has no changes.\n";
            return;
        }
        
        FileVersion* newVersion = new FileVersion(newHash);  // Linking the new has version of the file
        newVersion->prev = lastVersion;
        lastVersion->next = newVersion;
        fileHistoryMapStaged[filename] = newVersion;
    } else {
        fileHistoryMapStaged[filename] = new FileVersion(newHash);
    }

    hashMapStaged[newHash] = content;
    saveToDisk(fileHistoryMapStaged, hashMapStaged);
    std::cout << "Added " << filename << " with hash " << newHash << "\n";
}

string FileHistoryManager::getLatestHashStaged(const string& filename) {
    return fileHistoryMapStaged.find(filename) != fileHistoryMapStaged.end() ? fileHistoryMapStaged[filename]->hash : "";
}
string FileHistoryManager::getLatestHashCommitted(const string& filename) {
    return fileHistoryMapCommitted.find(filename) != fileHistoryMapCommitted.end() ? fileHistoryMapCommitted[filename]->hash : "";
}

/* fileHistoryMap.find(filename) != fileHistoryMap.end() 
        For maps in cpp, the find() checks if the key is present in the map or not. 
        If the key is present, it returns an iterator to the key-value pair,
        else it returns an iterator to the end of the map.
*/



bool FileHistoryManager::isFileModified(const string& filename) {
    std::string content = readFileContent(filename);
    std::string newHash = calculateFileHash(content);
    return getLatestHashCommitted(filename) != newHash;  // Checking if the current hash of the file is the same as the new hash
}

bool FileHistoryManager::isFileStaged(const string& filename) {
    if (fileHistoryMapStaged.find(filename) == fileHistoryMapStaged.end()){
        return false;
    }
        
    std::string content = readFileContent(filename);
    std::string newHash = calculateFileHash(content);
    return getLatestHashStaged(filename) == newHash;
}


void FileHistoryManager::showStatus() {

    initializeRepo();

    vector<string> modified;
    vector<string> staged;
    //vector<string> untrackedFiles;

    
    std::cout << "fileHistoryMapInitial size: " << fileHistoryMapInitial.size() << std::endl;


    for (const auto& entry : fileHistoryMapInitial) {
        std::string filename = entry.first;
        if(isFileStaged(filename)) {
            staged.push_back(filename);
        }
        else if(isFileModified(filename)) {
            modified.push_back(filename);
        }

    }

    std::cout << GREEN << "Staged files: "; 
    for(const string file : staged) {
        std::cout << file << " ";
    }
    std::cout << "\n";
    std::cout << RED << "Modified files: ";
    for(const string file: modified) {
        std::cout << file << " ";
    }
    std::cout << "\n" << RESET;
}
