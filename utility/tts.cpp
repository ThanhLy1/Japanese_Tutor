#include "tts.h"
#include <iostream>
#include <fstream>
#include <curl/curl.h>

const std::string Voice::BASE_URL = "http://127.0.0.1:50021/"; // Change this in the future.

Voice::Voice() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
}

Voice::~Voice() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

std::string Voice::urlEncode(const std::string& value) {
    char* encoded_value = curl_easy_escape(curl, value.c_str(), value.length());
    std::string result;
    if (encoded_value) {
        result = std::string(encoded_value);
        curl_free(encoded_value);
    }
    return result;
}

json Voice::postRequest(const std::string& url, const std::string& data) {
    std::string response;
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return { { "error", curl_easy_strerror(res) } };
        }

        curl_slist_free_all(headers);
    }

    return json::parse(response);
}

void Voice::postRequestWithJson(const std::string& url, const json& data, const std::string& audioFilePath) {
    std::ofstream outFile(audioFilePath, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file: " << audioFilePath << std::endl;
        return;
    }

    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: audio/wav");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.dump().c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
    }

    outFile.close();
}

std::string Voice::buildUrl(const std::string& endpoint, const std::map<std::string, std::string>& params) {
    std::string url = BASE_URL + endpoint;
    if (!params.empty()) {
        url += "?";
        for (const auto& param : params) {
            url += param.first + "=" + param.second + "&";
        }
        url.pop_back(); // Remove the last '&'
    }
    return url;
}

size_t Voice::write_callback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    s->append((char*)contents, newLength);
    return newLength;
}

size_t Voice::write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* outFile) {
    if (!outFile->is_open()) return 0;  // Ensure the file is open
    outFile->write(reinterpret_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}
