#ifndef VOCAB_H_
#define VOCAB_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <nlohmann/json.hpp>

class Vocab {
private:
    std::string kanji_;
    std::string hiragana_;
    std::string romaji_;
    std::vector<std::string> english_;
    std::string partOfSpeech_;
    std::string dialogue_;
    std::string lesson_;
    double difficulty_ = 0.0; // default initializer
    std::chrono::steady_clock::time_point lastQuestionTime_;

public:
    Vocab()
        : kanji_(""),
          hiragana_(""),
          romaji_(""),
          english_(),
          partOfSpeech_(""),
          dialogue_(""),
          lesson_(""),
          difficulty_(0.0),
          lastQuestionTime_(std::chrono::steady_clock::now())  // Initialized with the current time
    {}

    explicit Vocab(const nlohmann::json& jsonData)
        : kanji_(jsonData["kanji"].get<std::string>()),
          hiragana_(jsonData["hiragana"].get<std::string>()),
          romaji_(jsonData["romaji"].get<std::string>()),
          english_(jsonData["english"].get<std::vector<std::string>>()),
          partOfSpeech_(jsonData["part_of_speech"].get<std::string>()),
          dialogue_(jsonData["dialogue"].get<std::string>()),
          lesson_(jsonData["lesson"].get<std::string>()),
          difficulty_(jsonData["difficulty"].get<double>()),
          lastQuestionTime_(std::chrono::steady_clock::now())  // Initialized with the current time
    {}

    void loadFromJsonFile(const std::string& filename);
    void saveToPersistenceFile(const std::string& filename) const;
    nlohmann::json toJson() const;

    std::string getKanji() const;
    std::string getHiragana() const;
    std::string getRomaji() const;
    std::vector<std::string> getEnglish() const;
    std::string getPartOfSpeech() const;
    std::string getDialogue() const;
    std::string getLesson() const;
    double getDifficulty() const;

    void setKanji(const std::string& newKanji);
    void setHiragana(const std::string& newHiragana);
    void setRomaji(const std::string& newRomaji);
    void setEnglish(const std::vector<std::string>& newEnglish);
    void setPartOfSpeech(const std::string& newPartOfSpeech);
    void setDialogue(const std::string& newDialogue);
    void setLesson(const std::string& newLesson);
    void setDifficulty(double newDifficulty);

    void printDetails() const;
    std::string correctAnswer() const;

    std::chrono::steady_clock::time_point getLastQuestionTime() const;
    void setLastQuestionTime(const std::chrono::steady_clock::time_point& time);
};

nlohmann::json Vocab::toJson() const {
    nlohmann::json vocabJson;
    vocabJson["kanji"] = kanji_;
    vocabJson["hiragana"] = hiragana_;
    vocabJson["romaji"] = romaji_;
    vocabJson["english"] = english_;
    vocabJson["part_of_speech"] = partOfSpeech_;
    vocabJson["dialogue"] = dialogue_;
    vocabJson["lesson"] = lesson_;
    vocabJson["difficulty"] = difficulty_;
    vocabJson["last_question_time"] = lastQuestionTime_.time_since_epoch().count();
    return vocabJson;
}

void Vocab::loadFromJsonFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open JSON file." << std::endl;
        return;
    }

    nlohmann::json jsonData;
    try {
        file >> jsonData;
    } catch (const std::exception& e) {
        std::cout << "Failed to parse JSON: " << e.what() << std::endl;
        return;
    }

    if (jsonData.is_array() && !jsonData.empty()) {
        *this = Vocab(jsonData[0]);
        if (jsonData[0].contains("last_question_time")) {
            auto time = jsonData[0]["last_question_time"].get<long long>();
            lastQuestionTime_ = std::chrono::steady_clock::time_point(std::chrono::nanoseconds(time));
        }
    } else {
        std::cout << "Invalid JSON data." << std::endl;
    }
}

void Vocab::saveToPersistenceFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open persistence file for writing." << std::endl;
        return;
    }

    nlohmann::json jsonData = {
        {"kanji", kanji_},
        {"hiragana", hiragana_},
        {"romaji", romaji_},
        {"english", english_},
        {"part_of_speech", partOfSpeech_},
        {"dialogue", dialogue_},
        {"lesson", lesson_},
        {"difficulty", difficulty_},
        {"last_question_time", std::chrono::time_point_cast<std::chrono::nanoseconds>(lastQuestionTime_).time_since_epoch().count()}
    };

    nlohmann::json arrayData = nlohmann::json::array();
    arrayData.push_back(jsonData);

    try {
        file << std::setw(4) << arrayData << std::endl;
        std::cout << "Data saved to persistence file." << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Failed to write to persistence file: " << e.what() << std::endl;
    }
}

std::string Vocab::getKanji() const { return kanji_; }

std::string Vocab::getHiragana() const { return hiragana_; }

std::string Vocab::getRomaji() const { return romaji_; }

std::vector<std::string> Vocab::getEnglish() const { return english_; }

std::string Vocab::getPartOfSpeech() const { return partOfSpeech_; }

std::string Vocab::getDialogue() const { return dialogue_; }

std::string Vocab::getLesson() const { return lesson_; }

double Vocab::getDifficulty() const { return difficulty_; }

void Vocab::setKanji(const std::string& newKanji) { kanji_ = newKanji; }

void Vocab::setHiragana(const std::string& newHiragana) {
    hiragana_ = newHiragana;
}

void Vocab::setRomaji(const std::string& newRomaji) { romaji_ = newRomaji; }

void Vocab::setEnglish(const std::vector<std::string>& newEnglish) {
    english_ = newEnglish;
}

void Vocab::setPartOfSpeech(const std::string& newPartOfSpeech) {
    partOfSpeech_ = newPartOfSpeech;
}

void Vocab::setDialogue(const std::string& newDialogue) {
    dialogue_ = newDialogue;
}

void Vocab::setLesson(const std::string& newLesson) { lesson_ = newLesson; }

void Vocab::setDifficulty(double newDifficulty) { difficulty_ = newDifficulty; }

void Vocab::printDetails() const {
    std::cout << "Kanji: " << kanji_ << std::endl;
    std::cout << "Hiragana: " << hiragana_ << std::endl;
    std::cout << "Romaji: " << romaji_ << std::endl;

    std::cout << "English(s): ";
    for (const auto& m : english_) {
        std::cout << m << ", ";
    }
    std::cout << std::endl;

    std::cout << "Part of Speech: " << partOfSpeech_ << std::endl;
    std::cout << "Dialogue: " << dialogue_ << std::endl;
    std::cout << "Lesson: " << lesson_ << std::endl;
    std::cout << "Difficulty: " << difficulty_ << std::endl;
}

std::string Vocab::correctAnswer() const {
    std::stringstream ss;
    for (size_t i = 0; i < english_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << english_[i];
    }
    return ss.str();
}

std::chrono::steady_clock::time_point Vocab::getLastQuestionTime() const {
    return lastQuestionTime_;
}

void Vocab::setLastQuestionTime(const std::chrono::steady_clock::time_point& time) {
    lastQuestionTime_ = time;
}


#endif  // VOCAB_H_
