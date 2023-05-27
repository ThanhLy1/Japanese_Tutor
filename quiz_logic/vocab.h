#ifndef VOCAB_H
#define VOCAB_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Vocab {
public:
    Vocab();
    explicit Vocab(const json& jsonData);

    void loadFromJsonFile(const std::string& filename);
    void saveToPersistenceFile(const std::string& filename) const;
    json toJson() const;

    std::string getKanji() const;
    std::string getHiragana() const;
    std::string getRomaji() const;
    std::vector<std::string> getMeaning() const;
    std::string getPartOfSpeech() const;
    std::string getDialogue() const;
    std::string getLesson() const;
    double getDifficulty() const;

    void setKanji(const std::string& newKanji);
    void setHiragana(const std::string& newHiragana);
    void setRomaji(const std::string& newRomaji);
    void setMeaning(const std::vector<std::string>& newMeaning);
    void setPartOfSpeech(const std::string& newPartOfSpeech);
    void setDialogue(const std::string& newDialogue);
    void setLesson(const std::string& newLesson);
    void setDifficulty(double newDifficulty);

    void printDetails() const;
    std::string correctAnswer() const;

private:
    std::string kanji;
    std::string hiragana;
    std::string romaji;
    std::vector<std::string> meaning;
    std::string partOfSpeech;
    std::string dialogue;
    std::string lesson;
    double difficulty;
};

Vocab::Vocab() = default;

Vocab::Vocab(const json& jsonData)
    : kanji(jsonData["kanji"]),
      hiragana(jsonData["hiragana"]),
      romaji(jsonData["romaji"]),
      meaning(jsonData["meaning"].get<std::vector<std::string>>()),
      partOfSpeech(jsonData["part_of_speech"]),
      dialogue(jsonData["dialogue"]),
      lesson(jsonData["lesson"]),
      difficulty(jsonData["difficulty"]) {}

void Vocab::loadFromJsonFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open JSON file." << std::endl;
        return;
    }

    json jsonData;
    try {
        file >> jsonData;
    } catch (const std::exception& e) {
        std::cout << "Failed to parse JSON: " << e.what() << std::endl;
        return;
    }

    if (jsonData.is_array() && !jsonData.empty()) {
        *this = Vocab(jsonData[0]);
    } else {
        std::cout << "Invalid JSON data." << std::endl;
    }
}

void Vocab::saveToPersistenceFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open persistence file for writing."
                  << std::endl;
        return;
    }

    json jsonData = {
        {"kanji", kanji},
        {"hiragana", hiragana},
        {"romaji", romaji},
        {"meaning", meaning},
        {"part_of_speech", partOfSpeech},
        {"dialogue", dialogue},
        {"lesson", lesson},
        {"difficulty", difficulty}};

    try {
        file << std::setw(4) << jsonData << std::endl;
        std::cout << "Data saved to persistence file." << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Failed to write to persistence file: " << e.what()
                  << std::endl;
    }
}

json Vocab::toJson() const {
    json vocabJson;
    vocabJson["kanji"] = kanji;
    vocabJson["hiragana"] = hiragana;
    vocabJson["romaji"] = romaji;
    vocabJson["meaning"] = meaning;
    vocabJson["part_of_speech"] = partOfSpeech;
    vocabJson["dialogue"] = dialogue;
    vocabJson["lesson"] = lesson;
    vocabJson["difficulty"] = difficulty;
    return vocabJson;
}

std::string Vocab::getKanji() const { return kanji; }

std::string Vocab::getHiragana() const { return hiragana; }

std::string Vocab::getRomaji() const { return romaji; }

std::vector<std::string> Vocab::getMeaning() const { return meaning; }

std::string Vocab::getPartOfSpeech() const { return partOfSpeech; }

std::string Vocab::getDialogue() const { return dialogue; }

std::string Vocab::getLesson() const { return lesson; }

double Vocab::getDifficulty() const { return difficulty; }

void Vocab::setKanji(const std::string& newKanji) { kanji = newKanji; }

void Vocab::setHiragana(const std::string& newHiragana) {
    hiragana = newHiragana;
}

void Vocab::setRomaji(const std::string& newRomaji) { romaji = newRomaji; }

void Vocab::setMeaning(const std::vector<std::string>& newMeaning) {
    meaning = newMeaning;
}

void Vocab::setPartOfSpeech(const std::string& newPartOfSpeech) {
    partOfSpeech = newPartOfSpeech;
}

void Vocab::setDialogue(const std::string& newDialogue) {
    dialogue = newDialogue;
}

void Vocab::setLesson(const std::string& newLesson) { lesson = newLesson; }

void Vocab::setDifficulty(double newDifficulty) { difficulty = newDifficulty; }

void Vocab::printDetails() const {
    std::cout << "Kanji: " << kanji << std::endl;
    std::cout << "Hiragana: " << hiragana << std::endl;
    std::cout << "Romaji: " << romaji << std::endl;

    std::cout << "Meaning(s): ";
    for (const auto& m : meaning) {
        std::cout << m << ", ";
    }
    std::cout << std::endl;

    std::cout << "Part of Speech: " << partOfSpeech << std::endl;
    std::cout << "Dialogue: " << dialogue << std::endl;
    std::cout << "Lesson: " << lesson << std::endl;
    std::cout << "Difficulty: " << difficulty << std::endl;
}

std::string Vocab::correctAnswer() const {
    std::string answer = "";
    for (const auto& m : meaning) {
        answer += m + ", ";
    }
    // remove the last comma and space
    if (!answer.empty()) {
        answer.pop_back();
        answer.pop_back();
    }
    return answer;
}

#endif
