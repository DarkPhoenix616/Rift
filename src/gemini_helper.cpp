#ifdef _WIN32     // For making this cross platform, ie work on both windows and macOs
    #include <windows.h>    
#else
    #include <mach-o/dyld.h>
#endif
#include <vector> 
#include "embedded_api_key.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <stdexcept>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
namespace fs = std::filesystem; // For older toolchains, consider: namespace fs = std::experimental::filesystem;
using json = nlohmann::json;

#include "gemini_helper.h"  // Ensure this header declares the GeminiHelper class and its members

// Static callback function for libcurl
size_t GeminiHelper::WriteCallback(void* contents, size_t size, size_t nmemb, string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append(static_cast<char*>(contents), newLength);
        return newLength;
    } catch (std::bad_alloc& e) {
        return 0;
    }
}

GeminiHelper::GeminiHelper() {
    // Load API key on initialization
    loadApiKey();
}


bool GeminiHelper::hasApiKey() const {
    return !apiKey.empty();
}

string GeminiHelper::formatPrompt(const string& userCommand) {
    ostringstream prompt;
    prompt << "I'm using a version control system called Rift (similar to Git). "
           << "I tried to use this command: '" << userCommand << "'. "
           << "Suggest valid Rift commands that might be what I was trying to use. "
           << "Rift supports commands like: init, status, add, commit -m, commit log, branch create, branch switch,"
           << "Please respond with a JSON array of suggested commands only, with no additional text.";
    return prompt.str();
}

vector<string> GeminiHelper::parseResponse(const string& response) {
    vector<string> suggestions;
    try {
        json responseJson = json::parse(response);
        if (responseJson.contains("candidates") &&
            responseJson["candidates"].is_array() &&
            !responseJson["candidates"].empty())
        {
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
        cerr << "Error parsing Gemini response: " << e.what() << endl;
    }
    return suggestions;
}

vector<string> GeminiHelper::getSuggestions(const string& userCommand) {
    if (apiKey.empty()) {
        cerr << "API key not set. Generate your API key and set it in your .env file" << endl;
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
                    {{"text", formatPrompt(userCommand)}}
                }}
            }}
        };
        string jsonPayload = payload.dump();
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
        
        // Set headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        } else {
            suggestions = parseResponse(readBuffer);
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    
    return suggestions;
}


bool GeminiHelper::loadApiKey() {
    apiKey = EMBEDDED_GEMINI_API_KEY;  // Set from compiled-in constant
    std::cout << "Loaded embedded API key." << std::endl;
    return !apiKey.empty();  // Return true if it was loaded
}



