#ifndef GEMINI_HELPER_H
#define GEMINI_HELPER_H

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
    bool hasApiKey() const;
    vector<string> getSuggestions(const string& userCommand);
    
    bool loadApiKey();
};

#endif
