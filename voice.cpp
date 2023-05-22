#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <map>

using json = nlohmann::json;

class CurlRequest {
public:
    CurlRequest() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
    }

    ~CurlRequest() {
        if(curl) {
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
    }

    std::string urlEncode(const std::string& value) {
        char* encoded_value = curl_easy_escape(curl, value.c_str(), value.length());
        std::string result;
        if(encoded_value) {
            result = std::string(encoded_value);
            curl_free(encoded_value);
        }
        return result;
    }

    json postRequest(const std::string& url, const std::string& data) {
        std::string response;
        if(curl) {
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }

            curl_slist_free_all(headers); 
        }

        return json::parse(response);
    }

    void postRequestWithJson(const std::string& url, const json& data, const std::string& audioFilePath) {
        std::ofstream outFile(audioFilePath, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Failed to open output file: " << audioFilePath << std::endl;
            return;
        }

        if(curl) {
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: audio/wav");
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.dump().c_str());

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);

            CURLcode res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }

            curl_slist_free_all(headers);
        }

        outFile.close();
    }

    std::string buildUrl(const std::string& endpoint, const std::map<std::string, std::string>& params) {
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

private:
    static const std::string BASE_URL;

    CURL *curl;

    static size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *s) {
        size_t newLength = size * nmemb;
        s->append((char*)contents, newLength);
        return newLength;
    }

    static size_t write_data(void *ptr, size_t size, size_t nmemb, std::ofstream *outFile) {
        if (!outFile->is_open()) return 0;  // Ensure the file is open
        outFile->write(reinterpret_cast<char*>(ptr), size * nmemb);
        return size * nmemb;
    }
};

const std::string CurlRequest::BASE_URL = "http://127.0.0.1:50021/";

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <text>" << std::endl;
        return 1;
    }

    std::string text = argv[1];

    CurlRequest curlRequest;
    
    std::map<std::string, std::string> params;
    params["text"] = curlRequest.urlEncode(text);
    params["speaker"] = "13";
    params["speedScale"] = "1.7";
    params["volumeScale"] = "4.0";
    params["intonationScale"] = "1.5";
    params["prePhonemeLength"] = "1.0";
    params["postPhonemeLength"] = "1.0";

    std::string url1 = curlRequest.buildUrl("audio_query", params);
    json response1 = curlRequest.postRequest(url1, "");

    std::string url2 = curlRequest.buildUrl("synthesis", params);
    std::string audioFilePath = "audio.wav";
    curlRequest.postRequestWithJson(url2, response1, audioFilePath);

    std::cout << "Audio file saved: " << audioFilePath << std::endl;

    return 0;
}
