#include <iostream>
#include "utility/tts.h"

int main() {
    std::string text = "私の声はAIとChatGPTによって合成されています。";

    Voice voice;

    std::map<std::string, std::string> params;
    params["text"] = voice.urlEncode(text);
    params["speaker"] = "13";
    params["speedScale"] = "1.7";
    params["volumeScale"] = "1.0";
    params["intonationScale"] = "1.5";
    params["prePhonemeLength"] = "1.0";
    params["postPhonemeLength"] = "1.0";

    std::string url1 = voice.buildUrl("audio_query", params);
    json response1 = voice.postRequest(url1, "");

    if (response1.contains("error")) {
        std::cerr << "Error in audio_query request: " << response1["error"] << std::endl;
        return 1;
    }

    std::string url2 = voice.buildUrl("synthesis", params);
    std::string audioFilePath = "audio.wav";
    voice.postRequestWithJson(url2, response1, audioFilePath);

    std::cout << "Audio file saved: " << audioFilePath << std::endl;

    return 0;
}
