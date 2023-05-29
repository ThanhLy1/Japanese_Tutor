#include <iostream>
#include <chrono>
#include <thread>
#include "AudioPlayer.h"

int main() {
    // Create an instance of AudioPlayer
    AudioPlayer audioPlayer;

    // Load the audio file
    if (!audioPlayer.loadAudio("../test_audio.wav")) {
        std::cerr << "Failed to load audio." << std::endl;
        return 1;
    }

    // Play the audio
    audioPlayer.play();

    // Delay for a few seconds (adjust the duration as needed)
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Stop the audio
    audioPlayer.stop();

    return 0;
}
