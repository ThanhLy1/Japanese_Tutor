#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <random>
#include <ctime>
#include <iterator>
#include <algorithm>
#include <boost/math/special_functions/beta.hpp>
#include "include/nlohmann/json.hpp"

using json = nlohmann::json;

class Ebisu {
public:
    Ebisu(double alpha = 3.0, double beta = 1.0, double t = 1.0) : alpha(alpha), beta(beta), t(t) {}

    double predictRecall(double elapsed, bool exact = false) {
        double recallProbability = 0.0;
        if (exact) {
            recallProbability = std::exp(std::log(alpha) - std::log(alpha + beta) + (alpha + beta) * std::log((t + elapsed) / t));
        }
        else {
            recallProbability = 1.0 / (1.0 + std::pow((t + elapsed) / t, alpha + beta));
        }
        return recallProbability;
    }

    void updateRecall(double success, double total, double elapsed) {
        double alphaPosterior = alpha + success;
        double betaPosterior = beta + total - success;
        double tPosterior = (alphaPosterior / (alphaPosterior + betaPosterior)) * ((t + elapsed) / alphaPosterior);

        alpha = alphaPosterior;
        beta = betaPosterior;
        t = tPosterior;
    }

    double modelToPercentileDecay() {
        double percentileDecay = std::pow(t, 1.0 / (alpha + beta)) / boost::math::beta(alpha, beta);
        return percentileDecay;
    }

    double percentileDecayToModel(double percentileDecay) {
        double alphaPosterior = (alpha + beta) * (std::pow(percentileDecay * boost::math::beta(alpha, beta), alpha + beta) - 1);
        double betaPosterior = alpha * ((t / percentileDecay) - 1);
        double tPosterior = percentileDecay * (alphaPosterior / (alphaPosterior + betaPosterior));

        alpha = alphaPosterior;
        beta = betaPosterior;
        t = tPosterior;
        
        return tPosterior;  // Return the new t-value
    }

    double getAlpha() const {
        return alpha;
    }

    double getBeta() const {
        return beta;
    }

    double getT() const {
        return t;
    }

    void setAlpha(double newAlpha) {
        alpha = newAlpha;
    }

    void setBeta(double newBeta) {
        beta = newBeta;
    }

    void setT(double newT) {
        t = newT;
    }

private:
    double alpha;
    double beta;
    double t;
};

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
    std::string correctAnswer() const {
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
};

class Quiz {
private:
    std::vector<Vocab> vocabList;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;
    Ebisu ebisuModel;
    int totalQuestions = 0;
    int correctAnswers = 0;

public:
    Quiz() {
        std::srand(std::time(0));
        generator = std::default_random_engine(std::rand());
    }

    bool loadQuiz(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
        json jsonData;
        file >> jsonData;
        if (jsonData.is_array()) {
            for (const auto& vocabData : jsonData) {
                Vocab vocab(vocabData);
                vocabList.push_back(vocab);
            }
        }
        distribution = std::uniform_int_distribution<int>(0, vocabList.size() - 1);
        return true;
    }

    void startQuiz() {
        int numQuestions = 10;
        for (int i = 0; i < numQuestions; ++i) {
            Vocab vocab = getRandomVocab();
            askQuestion(vocab);
            std::string userAnswer;
            std::getline(std::cin, userAnswer);

            // Check if the user wants to quit
        std::string answerLower = userAnswer;
        std::transform(answerLower.begin(), answerLower.end(), answerLower.begin(), ::tolower);
        answerLower.erase(std::remove_if(answerLower.begin(), answerLower.end(), ::isspace), answerLower.end());
        if (answerLower == "q" || answerLower == "quit") {
            std::cout << "Quiz terminated by the user." << std::endl;
            break;  // Exit the loop
        }
            processAnswer(vocab, userAnswer);
        }
    }

    void askQuestion(const Vocab& vocab) {
        std::cout << "Translate the following word to English: " << vocab.getKanji() << std::endl;
    }

    void processAnswer(const Vocab& vocab, const std::string& userAnswer) {
        std::string answerLower = userAnswer;
        std::transform(answerLower.begin(), answerLower.end(), answerLower.begin(), ::tolower);
        answerLower.erase(std::remove_if(answerLower.begin(), answerLower.end(), ::isspace), answerLower.end());

        // Check if the user wants to quit
        if (answerLower == "q" || answerLower == "quit") {
            std::cout << "Quiz terminated by the user." << std::endl;
            exit(0);  // Exit the program
        }

        bool isCorrect = false;
        for (const auto& meaning : vocab.getMeaning()) {
            std::string meaningLower = meaning;
            std::transform(meaningLower.begin(), meaningLower.end(), meaningLower.begin(), ::tolower);
            if (meaningLower == answerLower) {
                isCorrect = true;
                break;
            }
        }

        std::cout << (isCorrect ? "Correct!" : "Incorrect.") << std::endl;
        if (isCorrect) {
            correctAnswers++;
        }
        totalQuestions++;
        double success = isCorrect ? 1.0 : 0.0;
        ebisuModel.updateRecall(success, 1.0, vocab.getDifficulty());
    }

    Vocab getRandomVocab() {
        if(vocabList.empty()){
            throw std::runtime_error("vocabList is empty!");
        }
        int index = distribution(generator);
        return vocabList[index];
    }

    void printStatistics() const {
        std::cout << "Total questions: " << totalQuestions << std::endl;
        std::cout << "Correct answers: " << correctAnswers << std::endl;
        if (totalQuestions > 0) {
            double percentage = 100.0 * correctAnswers / totalQuestions;
            std::cout << "Accuracy: " << percentage << "%" << std::endl;
        }
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

    /*
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
    */

    Quiz myQuiz;
    myQuiz.loadQuiz("quiz_data.json");
    myQuiz.startQuiz();
    myQuiz.printStatistics();
    
    return 0;
}
