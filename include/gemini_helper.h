#ifndef GEMINI_HELPER_H
#define GEMINI_HELPER_H

// Prevent std::byte to avoid conflicts with Windows headers
#define _ENABLE_EXTENDED_ALIGNED_STORAGE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef byte

#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

class GeminiHelper {
private:
    string apiKey;
    const string apiEndpoint = "https://generativelanguage.googleapis.com/v1/models/gemini-1.5-pro:generateContent";
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* s);
    string formatPrompt(const string& userCommand);
    vector<string> parseResponse(const string& response);
    
public:
    GeminiHelper();
    void setApiKey(const string& key);
    bool hasApiKey() const;
    vector<string> getSuggestions(const string& userCommand);
    
    // Load API key from config file
    bool loadApiKey();
    // Save API key to config file
    bool saveApiKey(const string& key);
};

#endif