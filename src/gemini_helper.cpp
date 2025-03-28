#ifdef _WIN32
#define _ENABLE_EXTENDED_ALIGNED_STORAGE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef byte
#endif

#include "../include/gemini_helper.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

// Static callback function for libcurl
size_t GeminiHelper::WriteCallback(void* contents, size_t size, size_t nmemb, string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    } catch(std::bad_alloc &e) {
        return 0;
    }
}

GeminiHelper::GeminiHelper() {
    // Load API key on initialization
    loadApiKey();
}

void GeminiHelper::setApiKey(const string& key) {
    apiKey = key;
    saveApiKey(key);
}

bool GeminiHelper::hasApiKey() const {
    return !apiKey.empty();
}

string GeminiHelper::formatPrompt(const string& userCommand) {
    std::ostringstream prompt;
    prompt << "I'm using a version control system called Rift (similar to Git). "
           << "I tried to use this command: '" << userCommand << "'. "
           << "Suggest valid Rift commands that might be what I was trying to use. "
           << "Rift supports commands like: init, status, add, commit -m, commit log. "
           << "Please respond with a JSON array of suggested commands only, with no additional text.";
    return prompt.str();
}

vector<string> GeminiHelper::parseResponse(const string& response) {
    vector<string> suggestions;
    try {
        json responseJson = json::parse(response);
        
        if (responseJson.contains("candidates") && responseJson["candidates"].is_array() && 
            !responseJson["candidates"].empty()) {
            
            auto candidate = responseJson["candidates"][0];
            if (candidate.contains("content") && candidate["content"].contains("parts")) {
                auto parts = candidate["content"]["parts"];
                if (parts.is_array() && !parts.empty()) {
                    string textContent = parts[0]["text"];
                    
                    
                    size_t startPos = textContent.find('[');
                    size_t endPos = textContent.rfind(']');
                    
                    if (startPos != string::npos && endPos != string::npos && startPos < endPos) {
                        string jsonArrayStr = textContent.substr(startPos, endPos - startPos + 1);
                        json suggestionArray = json::parse(jsonArrayStr);
                        
                        if (suggestionArray.is_array()) {
                            for (const auto& suggestion : suggestionArray) {
                                if (suggestion.is_string()) {
                                    suggestions.push_back(suggestion.get<string>());
                                }
                            }
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing Gemini response: " << e.what() << std::endl;
    }
    
    return suggestions;
}

vector<string> GeminiHelper::getSuggestions(const string& userCommand) {
    if (apiKey.empty()) {
        std::cerr << "API key not set. Use 'Rift config --set-api-key <key>' to set your Gemini API key." << std::endl;
        return {};
    }
    
    CURL* curl = curl_easy_init();
    vector<string> suggestions;
    
    if (curl) {
        string readBuffer;
        string url = apiEndpoint + "?key=" + apiKey;
        
        // Prepare the JSON payload
        json payload = {
            {"contents", {
                {"parts", {
                    {"text", formatPrompt(userCommand)}
                }}
            }}
        };
        
        string jsonPayload = payload.dump();
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
        
        // Set headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        CURLcode res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            suggestions = parseResponse(readBuffer);
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    
    return suggestions;
}

bool GeminiHelper::loadApiKey() {
    // Create config directory if it doesn't exist
    fs::path configDir = fs::path("./data/.vcs/config");
    fs::path apiKeyFile = configDir / "gemini_api_key.txt";
    
    if (!fs::exists(configDir)) {
        fs::create_directories(configDir);
    }
    
    if (!fs::exists(apiKeyFile)) {
        return false;
    }
    
    std::ifstream file(apiKeyFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::getline(file, apiKey);
    file.close();
    
    return !apiKey.empty();
}

bool GeminiHelper::saveApiKey(const string& key) {
    fs::path configDir = fs::path("./data/.vcs/config");
    fs::path apiKeyFile = configDir / "gemini_api_key.txt";
    
    if (!fs::exists(configDir)) {
        fs::create_directories(configDir);
    }
    
    std::ofstream file(apiKeyFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << key;
    file.close();
    
    return true;
}