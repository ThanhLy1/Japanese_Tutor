#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class TextToSpeech {
    const std::string BASE_URL = "http://127.0.0.1:50021";
    const std::string ACCEPT_HEADER = "Accept: application/json";

    int speaker;

public:
    TextToSpeech(int speaker) : speaker(speaker) {}

    json audio_query(const std::string& text) {
        std::string url = BASE_URL + "/audio_query";
        std::string params = "text=" + text + "&speaker=" + std::to_string(speaker);

        std::string response = send_post_request(url, params);

        if (!response.empty()) {
            return json::parse(response);
        } else {
            std::cout << "Error: Empty response" << std::endl;
            return nullptr;
        }
    }

    json audio_query_from_preset(const std::string& text, int preset_id) {
        std::string url = BASE_URL + "/audio_query_from_preset";
        std::string params = "text=" + text + "&preset_id=" + std::to_string(preset_id);

        std::string response = send_post_request(url, params);

        if (!response.empty()) {
            return json::parse(response);
        } else {
            std::cout << "Error: Empty response" << std::endl;
            return nullptr;
        }
    }

    void synthesize_and_save_audio(const json& query_data, double speed_scale, double volume_scale, double intonation_scale,
                                   double pre_phoneme_length, double post_phoneme_length, const std::string& audio_file_path) {
        std::string synthesis_url = BASE_URL + "/synthesis";
        std::string params = "speaker=" + std::to_string(speaker) +
                             "&speedScale=" + std::to_string(speed_scale) +
                             "&volumeScale=" + std::to_string(volume_scale) +
                             "&intonationScale=" + std::to_string(intonation_scale) +
                             "&prePhonemeLength=" + std::to_string(pre_phoneme_length) +
                             "&postPhonemeLength=" + std::to_string(post_phoneme_length);

        std::string response = send_post_request(synthesis_url, params, query_data.dump());

        if (!response.empty()) {
            // Wait for the synthesis process to complete
            std::this_thread::sleep_for(std::chrono::seconds(5));

            std::ofstream audio_file(audio_file_path, std::ios::binary);
            audio_file.write(response.c_str(), response.length());
            audio_file.close();

            std::cout << "Audio file saved: " << audio_file_path << std::endl;
        } else {
            std::cout << "Error: Empty response" << std::endl;
        }
    }

private:
    static size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* response) {
        size_t total_size = size * nmemb;
        response->append(ptr, total_size);
        return total_size;
    }

    std::string send_post_request(const std::string& url, const std::string& params, const std::string& data = "") {
        std::string response;

        CURL* curl = curl_easy_init();
        if (curl) {
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, ACCEPT_HEADER.c_str());

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            if (!data.empty()) {
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
            } else {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
            }

            CURLcode result = curl_easy_perform(curl);

            if (result != CURLE_OK) {
                std::cout << "cURL error: " << curl_easy_strerror(result) << std::endl;
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } else {
            std::cout << "Failed to initialize cURL" << std::endl;
        }

        return response;
    }
};

void convert_text_to_speech(const std::string& word, int speaker, int preset_id) {
    TextToSpeech tts(speaker);

    // Split the word using ":" as the delimiter
    std::vector<std::string> parts;
    std::size_t colon_pos = word.find(':');
    std::string text = word.substr(0, colon_pos);
    std::string audio_file_name = (colon_pos != std::string::npos && colon_pos + 1 < word.length()) ? word.substr(colon_pos + 1) : text;
    audio_file_name = audio_file_name.substr(0, 10);

    json result = tts.audio_query(text);
    json result2 = tts.audio_query_from_preset(text, preset_id);

    double speed_scale = 1.0;
    double volume_scale = 1.0;
    double intonation_scale = 2.0;
    double pre_phoneme_length = 0.1;
    double post_phoneme_length = 0.1;

    std::string audio_file_path = audio_file_name + ".wav";
    tts.synthesize_and_save_audio(result2, speed_scale, volume_scale, intonation_scale, pre_phoneme_length, post_phoneme_length, audio_file_path);
}

int main() {
    // Read the words from the file, ignoring empty lines and lines starting with "#" and "//"
    std::ifstream file("words.txt");
    std::vector<std::string> words;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line[0] != '#' && line.substr(0, 2) != "//") {
                words.push_back(line);
            }
        }

        file.close();
    } else {
        std::cout << "Error: Failed to open the words file" << std::endl;
        return 1;
    }

    // Specify the speaker and preset_id
    int speaker = 20;
    int preset_id = 1;

    // Convert each word to speech
    for (const std::string& word : words) {
        convert_text_to_speech(word, speaker, preset_id);
    }

    return 0;
}
