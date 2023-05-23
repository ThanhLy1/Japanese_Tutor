#pragma once

#include <string>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Voice {
public:
    Voice();
    ~Voice();

    std::string urlEncode(const std::string& value);
    json postRequest(const std::string& url, const std::string& data);
    void postRequestWithJson(const std::string& url, const json& data, const std::string& audioFilePath);
    std::string buildUrl(const std::string& endpoint, const std::map<std::string, std::string>& params);

private:
    static const std::string BASE_URL;

    void* curl;

    static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* s);
    static size_t write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* outFile);
};
