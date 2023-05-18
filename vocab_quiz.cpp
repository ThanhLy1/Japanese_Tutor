#include <iostream>
#include <sstream> // For commandStream;
#include <fstream>
#include <random>
#include <algorithm>
#include <cstdlib> // For system("clear") on Unix-like systems, system()
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;

class VocabularyQuiz {
public:
    void loadVocabulary(const std::string& filename) {
        std::ifstream file(filename);
        file >> data_;
        file.close();
    }

    void startQuiz() {
    json& vocabulary = data_["vocabulary"]; 
    std::shuffle(vocabulary.begin(), vocabulary.end(), std::mt19937(std::random_device()()));

    int correctCount = 0;
    int totalCount = 0;
    int score = 0;
    std::string lastWord = "";

    for (json& word : vocabulary) {
        std::string question = word["word"];
        std::string answer = word["meaning"];

        //clearConsole();
        std::cout << "Question: " << question << std::endl;
        std::string userAnswer;
        std::cout << "Your answer (type 'q' to quit): ";
        std::cin >> userAnswer;

        if (userAnswer == "q") {
            std::cout << "Quitting the quiz." << std::endl;
            break;  // Exit the for loop
        }

        totalCount++;

        if (userAnswer == answer) {
            std::cout << "Correct!" << std::endl;
            word["recall_level"] = word["recall_level"].get<int>() + 1;
            correctCount++;
            score += 10; // Increase score for correct answer
            lastWord = question + " (" + answer + ")"; // Save the last correct word
        } else {
            std::cout << "Incorrect!" << std::endl;
            std::cout << "The correct answer is: " << answer << std::endl;
            word["recall_level"] = word["recall_level"].get<int>() - 1;
            score -= 5; // Decrease score for incorrect answer
            lastWord = question + " (" + answer + ")"; // Save the last incorrect word
        }
    }

    //clearConsole();
    std::cout << "Quiz completed!" << std::endl;
    std::cout << "Total Questions: " << totalCount << std::endl;
    std::cout << "Correct Answers: " << correctCount << std::endl;
    std::cout << "Incorrect Answers: " << totalCount - correctCount << std::endl;
    std::cout << "Score: " << score << std::endl;

    //show the last word
    if (!lastWord.empty()) {
        std::cout << "Last Word: " << lastWord << std::endl;
    }

    saveVocabulary();
}


private:
    void clearConsole() {
    // Clear console command for Unix-like systems and Windows
    std::cout << "\033[2J\033[1;1H";  // For Unix-like systems
    // system("cls");  // For Windows
}

    void saveVocabulary() {
        std::ofstream file("japanese_101.json");
        file << data_.dump(2);
        file.close();
    }

    json data_;
};

class AudioPlayer {
public:
    AudioPlayer() {
        SDL_Init(SDL_INIT_AUDIO);
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    }

    ~AudioPlayer() {
        Mix_CloseAudio();
        SDL_Quit();
    }

    bool loadAudio(const std::string& filePath) {
        music_ = Mix_LoadMUS(filePath.c_str());
        return (music_ != nullptr);
    }

    void play() {
        if (music_) {
            Mix_PlayMusic(music_, 1);  // 1 for loop playback
        }
    }

    void stop() {
        Mix_HaltMusic();
    }

    void pause() {
        Mix_PauseMusic();
    }

    void resume() {
        Mix_ResumeMusic();
    }

private:
    Mix_Music* music_ = nullptr;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./vocab_quiz <filename.json>" << std::endl;
        return 1;
    }

    // Provide the absolute path to the Python interpreter and the Python script
    std::string pythonInterpreter = "/usr/bin/python3";
    std::string pythonScript = "voiceSynth.py";

    // Parameters to pass to the Python script
    std::string text = "ユー オウド ミー 1万円";
    int presetId = 1;
    int speaker = 18;

    // Create a command to execute the Python script with the parameters
    std::stringstream commandStream;
    commandStream << pythonInterpreter << " " << pythonScript << " \"" << text << "\" " << presetId << " " << speaker;
    std::string command = commandStream.str();

     // Execute the Python script
    int result = system(command.c_str());

    // Check the return value for errors
    if (result == 0) {
        std::cout << "Python script executed successfully." << std::endl;
    } else {
        std::cerr << "Error executing Python script. Return code: " << result << std::endl;
    }

    std::string filename = argv[1];

    AudioPlayer audioPlayer;

    if (audioPlayer.loadAudio("audio.wav")) {
        audioPlayer.play();
    } else {
        // Handle audio loading error
    }

    VocabularyQuiz quiz;
    quiz.loadVocabulary(filename);
    quiz.startQuiz();

    return 0;
}
