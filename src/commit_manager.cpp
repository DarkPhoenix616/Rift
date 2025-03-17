#include "../include/commit_manager.h"
#include "../include/file_manager.h"


#include <fstream>
#include <nlohmann/json.hpp>
#include <ctime>
#include <sstream>
#include <filesystem>
#include <iomanip>

using json = nlohmann::json;
namespace fs = std::filesystem;

const string COMMITTED_FILE_HISTORY_PATH = "./data/.vcs/main/";


string CommitManager::generateCommitHash(const string& message) {
    time_t now = std::time(nullptr);
    std::stringstream ss;
    ss << message << now;
    return std::to_string(std::hash<string>{}(ss.str()));
}


CommitManager::CommitManager(FileHistoryManager& fileHistoryManager): head(nullptr), tail(nullptr) {
    loadCommitsFromDisk(fileHistoryManager);
}

CommitManager::~CommitManager() {
    // Free all commit nodes.
    Commit* current = head;
    while (current) {
        Commit* next = current->next;
        delete current;
        current = next;
    }
}


void CommitManager::addCommit(const string& message, FileHistoryManager& fileHistoryManager) {

    string commitHash = generateCommitHash(message);

    // Create a new commit node.
    Commit* newCommit = new Commit(message, commitHash);

    // Copy the current staged file versions from FileHistoryManager.
    // (Assuming fileHistoryManager.fileHistoryMapStaged holds filename -> latest FileVersion*,
    // and fileHistoryManager.hashMapStaged holds hash -> file content.)
    for (const auto& entry : fileHistoryManager.fileHistoryMapStaged) {
        string filename = entry.first;
        string latestHash = fileHistoryManager.getLatestHashStaged(filename);
        newCommit->filesCommitted[filename] = latestHash;
    }

    // Append the new commit node to the doubly linked list.
    if (tail) {
        tail->next = newCommit;
        newCommit->prev = tail;
        tail = newCommit;
    } else {
        head = tail = newCommit;
    }

    // Create a directory for this commit.
    string commitDir = "./data/.vcs/main/Committed State/" + commitHash;
    fs::create_directories(commitDir);

    // Prepare JSON objects to store committed file history and content.
    json fileHistoryJson;
    json hashMapJson;
    for (const auto& kv : newCommit->filesCommitted) {
        fileHistoryJson[kv.first] = kv.second;
        fileHistoryManager.fileHistoryMapCommitted[kv.first] = fileHistoryManager.fileHistoryMapStaged.at(kv.first);
        // Retrieve file content from the staged hash map.
        if (fileHistoryManager.hashMapStaged.find(kv.second) != fileHistoryManager.hashMapStaged.end()) {
            hashMapJson[kv.second] = fileHistoryManager.base64_encode(fileHistoryManager.hashMapStaged.at(kv.second));
        }
    }

    // Write the JSON files to the commit directory.
    std::ofstream fhOut(commitDir + "/fileHistoryMapCommitted.json");
    std::ofstream hmOut(commitDir + "/hashMapCommitted.json");
    fhOut << fileHistoryJson.dump(4);
    hmOut << hashMapJson.dump(4);
    fhOut.close();
    hmOut.close();

    // Update the binary commit history file.
    saveCommitsToDisk();

    fileHistoryManager.fileHistoryMapStaged.clear();
    fileHistoryManager.hashMapStaged.clear();
    fileHistoryManager.saveToDisk(fileHistoryManager.fileHistoryMapStaged, fileHistoryManager.hashMapStaged);

    std::cout << "Commit added: \"" << message << "\" with hash: " << commitHash << std::endl;
}


void CommitManager::displayCommitHistory() {
    Commit* current = head;
    while (current) {
        std::cout << "Commit Hash: " << current->commitHash
                  << " | Message: " << current->message << std::endl;
        current = current->next;
    }
}

void CommitManager::saveCommitsToDisk() {
    // Save the commit history in a binary file ("./data/.vcs/commits.dat").
    std::ofstream outFile("./data/.vcs/main/commits.dat", std::ios::binary | std::ios::trunc);
    if (!outFile) {
        std::cerr << "Error: Unable to open commits.dat for writing." << std::endl;
        return;
    }

    // Serialize commit history as a JSON array.
    json commitArray = json::array();
    Commit* current = head;
    while (current) {
        json j;
        j["message"] = current->message;
        j["commitHash"] = current->commitHash;
        j["filesCommitted"] = current->filesCommitted;
        commitArray.push_back(j);
        current = current->next;
    }

    std::string serialized = commitArray.dump();
    size_t size = serialized.size();
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
    outFile.write(serialized.c_str(), size);
    outFile.close();
}

void CommitManager::loadCommitsFromDisk(FileHistoryManager& fileHistoryManager){
    std::ifstream inFile("./data/.vcs/main/commits.dat", std::ios::binary);
    if (!inFile) {
        // No commit history exists yet.
        return;
    }

    size_t size = 0;
    inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::string serialized(size, ' ');
    inFile.read(&serialized[0], size);
    inFile.close();

    json commitArray = json::parse(serialized);
    for (auto& j : commitArray) {
        string message = j["message"];
        string commitHash = j["commitHash"];
        unordered_map<string, string> filesCommitted = j["filesCommitted"].get<unordered_map<string, string>>();

        Commit* newCommit = new Commit(message, commitHash);
        newCommit->filesCommitted = filesCommitted;

        // Append newCommit to the doubly linked list.
        if (tail) {
            tail->next = newCommit;
            newCommit->prev = tail;
            tail = newCommit;
        } else {
            head = tail = newCommit;
        }
    }

    //cout << "Tail hash: " << tail->commitHash << endl;

    std::ifstream fileHistoryCommitted(COMMITTED_FILE_HISTORY_PATH + "/Committed State/"+ tail->commitHash + "/fileHistoryMapCommitted.json");
    if(fileHistoryCommitted){
        json fileHistoryJsonCommitted;
        fileHistoryCommitted >> fileHistoryJsonCommitted;
        fileHistoryCommitted.close();
        for(auto &entry : fileHistoryJsonCommitted.items()){
            std::string filename = entry.key();
            std::string fileHash = entry.value();
            fileHistoryManager.fileHistoryMapCommitted[filename] = new FileVersion(fileHash);
        }

        cout << "Loaded Commit history" << endl;
    }

}
