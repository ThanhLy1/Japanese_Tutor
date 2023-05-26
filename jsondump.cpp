#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "include/nlohmann/json.hpp"

using json = nlohmann::json;

class Vocab {
private:
    std::string kanji;
    std::string hiragana;
    std::string romaji;
    std::vector<std::string> meaning;
    std::string partOfSpeech;
    std::string dialogue;
    std::string lesson;
    double difficulty;

public:
    Vocab() = default;

    explicit Vocab(const json& jsonData)
        : kanji(jsonData["kanji"]),
          hiragana(jsonData["hiragana"]),
          romaji(jsonData["romaji"]),
          meaning(jsonData["meaning"].get<std::vector<std::string>>()),  // Specify the type as std::string
          partOfSpeech(jsonData["part_of_speech"]),
          dialogue(jsonData["dialogue"]),
          lesson(jsonData["lesson"]),
          difficulty(jsonData["difficulty"]) {}

    void loadFromJsonFile(const std::string& filename) {
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

    void saveToPersistenceFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Failed to open persistence file for writing." << std::endl;
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
            {"difficulty", difficulty}
        };

        try {
            file << std::setw(4) << jsonData << std::endl;
            std::cout << "Data saved to persistence file." << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Failed to write to persistence file: " << e.what() << std::endl;
        }
    }

    // Getter methods
    std::string getKanji() const { return kanji; }
    std::string getHiragana() const { return hiragana; }
    std::string getRomaji() const { return romaji; }
    std::vector<std::string> getMeaning() const { return meaning; }
    std::string getPartOfSpeech() const { return partOfSpeech; }
    std::string getDialogue() const { return dialogue; }
    std::string getLesson() const { return lesson; }
    double getDifficulty() const { return difficulty; }

    // Setter methods
    void setKanji(const std::string& newKanji) { kanji = newKanji; }
    void setHiragana(const std::string& newHiragana) { hiragana = newHiragana; }
    void setRomaji(const std::string& newRomaji) { romaji = newRomaji; }
    void setMeaning(const std::vector<std::string>& newMeaning) { meaning = newMeaning; }
    void setPartOfSpeech(const std::string& newPartOfSpeech) { partOfSpeech = newPartOfSpeech; }
    void setDialogue(const std::string& newDialogue) { dialogue = newDialogue; }
    void setLesson(const std::string& newLesson) { lesson = newLesson; }
    void setDifficulty(double newDifficulty) { difficulty = newDifficulty; }

    // Other methods
    void printDetails() const {
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
};

void dumpToJsonFile(const std::vector<Vocab>& vocabList) {
    json jsonData = json::array();

    for (const auto& vocab : vocabList) {
        jsonData.push_back({
            {"kanji", vocab.getKanji()},
            {"hiragana", vocab.getHiragana()},
            {"romaji", vocab.getRomaji()},
            {"meaning", vocab.getMeaning()},
            {"part_of_speech", vocab.getPartOfSpeech()},
            {"dialogue", vocab.getDialogue()},
            {"lesson", vocab.getLesson()},
            {"difficulty", vocab.getDifficulty()}
        });
    }

    std::cout << std::setw(4) << jsonData << std::endl;
}

void printJsonFile(const std::string& filename) {
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

    if (jsonData.is_array()) {
        for (const auto& vocabData : jsonData) {
            Vocab vocab(vocabData);
            vocab.printDetails();
            std::cout << std::endl;
        }
    } else {
        std::cout << "Invalid JSON data." << std::endl;
    }
}

int main() {
    // Load the data from the "quiz_data.json" file
    std::vector<Vocab> vocabList;
    Vocab vocab;
    vocab.loadFromJsonFile("quiz_data.json");

    // Access the loaded data using the getter methods
    vocab.printDetails();

    // Modify data using setter methods
    vocab.setDifficulty(2.5);
    vocab.setMeaning({"cooking", "cuisine"});
    vocab.printDetails();

    // Save the modified data to persistence file using kanji as ID
    vocab.saveToPersistenceFile("persistence_data.json");

    // Add the loaded vocab to the vocabList
    vocabList.push_back(vocab);

    // Dump the vocabList to JSON
    dumpToJsonFile(vocabList);

    // Print the JSON file contents
    printJsonFile("quiz_data.json");

    return 0;
}
