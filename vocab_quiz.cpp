#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;

const std::string PYTHON_INTERPRETER = "/usr/bin/python3";
const std::string PYTHON_SCRIPT = "voiceSynth.py";

class Student {
private:
    std::string name_;
    int score_;

public:
    Student(const std::string& name) : name_(name), score_(0) {}

    const std::string& getName() const { return name_; }
    int getScore() const { return score_; }
    
    void updateScore(int points) { score_ += points; }
};

class AudioPlayer {
public:
    AudioPlayer() {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            std::cerr << "Failed to initialize SDL audio: " << SDL_GetError() << std::endl;
        } else {
            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
                std::cerr << "Failed to open audio: " << Mix_GetError() << std::endl;
            }
        }
    }

    ~AudioPlayer() {
        Mix_CloseAudio();
        SDL_Quit();
    }

    bool loadAudio(const std::string& filePath) {
        music_ = Mix_LoadMUS(filePath.c_str());
        if (!music_) {
            std::cerr << "Failed to load audio: " << Mix_GetError() << std::endl;
            return false;
        }
        return true;
    }

    void play() {
        if (music_) {
            if (Mix_PlayMusic(music_, 1) < 0) {
                std::cerr << "Failed to play audio: " << Mix_GetError() << std::endl;
            }
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

void executePythonScript(const std::string& pythonInterpreter, const std::string& pythonScript, const std::string& text, int presetId, int speaker) {
    std::stringstream commandStream;
    commandStream << pythonInterpreter << " " << pythonScript << " \"" << text << "\" " << presetId << " " << speaker;
    std::string command = commandStream.str();

    int result = system(command.c_str());

    if (result == 0) {
        std::cout << "Python script executed successfully." << std::endl;
    } else {
        std::cerr << "Error executing Python script. Return code: " << result << std::endl;
    }
}

class Tutor {
public:
    void startLesson(Student& student) {
        std::string pythonInterpreter = PYTHON_INTERPRETER;
        std::string pythonScript = PYTHON_SCRIPT;
        std::cout << "Welcome, " << student.getName() << "!" << std::endl;
        std::cout << "Let's begin the Japanese lesson." << std::endl;

        int presetId = 1;
        int speaker = 17;

        std::string text = "Welcome " + student.getName() + ". Let's begin the Japanese lesson.";

        try {
            executePythonScript(pythonInterpreter, pythonScript, text, presetId, speaker);
        } catch (const std::exception& e) {
            std::cerr << "Error executing Python script: " << e.what() << std::endl;
            return;
        }

        AudioPlayer audioPlayer;

        if (audioPlayer.loadAudio("audio.wav")) {
            audioPlayer.play();
        } else {
            std::cerr << "Failed to load audio." << std::endl;
        }

        int lessonScore = 80;
        student.updateScore(lessonScore);

        std::cout << "Lesson completed!" << std::endl;
        std::cout << "Your score: " << student.getScore() << std::endl;
    }
};

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

            std::cout << "Question: " << question << std::endl;
            std::string userAnswer;
            std::cout << "Your answer (type 'q' to quit): ";
            std::cin >> userAnswer;

            if (userAnswer == "q") {
                std::cout << "Quitting the quiz." << std::endl;
                break;
            }

            totalCount++;

            if (userAnswer == answer) {
                std::cout << "Correct!" << std::endl;
                word["recall_level"] = word["recall_level"].get<int>() + 1;
                correctCount++;
                score += 10;
                lastWord = question + " (" + answer + ")";
            } else {
                std::cout << "Incorrect!" << std::endl;
                std::cout << "The correct answer is: " << answer << std::endl;
                word["recall_level"] = word["recall_level"].get<int>() - 1;
                score -= 5;
                lastWord = question + " (" + answer + ")";
            }
        }

        std::cout << "Quiz completed!" << std::endl;
        std::cout << "Total Questions: " << totalCount << std::endl;
        std::cout << "Correct Answers: " << correctCount << std::endl;
        std::cout << "Incorrect Answers: " << totalCount - correctCount << std::endl;
        std::cout << "Score: " << score << std::endl;

        if (!lastWord.empty()) {
            std::cout << "Last Word: " << lastWord << std::endl;
        }

        saveVocabulary();
    }

private:
    void saveVocabulary() {
        std::ofstream file("japanese_101.json");
        file << data_.dump(2);
        file.close();
    }

    json data_;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./vocab_quiz <filename.json>" << std::endl;
        return 1;
    }

    std::string text = "ユー オウド ミー 1万円";
    int presetId = 1;
    int speaker = 17;

    executePythonScript(PYTHON_INTERPRETER, PYTHON_SCRIPT, text, presetId, speaker);

    Student student("John Doe");
    Tutor tutor;
    tutor.startLesson(student);

    std::string filename = argv[1];

    AudioPlayer audioPlayer;

    if (audioPlayer.loadAudio("audio.wav")) {
        audioPlayer.play();
    } else {
        std::cerr << "Failed to load audio." << std::endl;
    }

    VocabularyQuiz quiz;
    quiz.loadVocabulary(filename);
    quiz.startQuiz();

    return 0;
}