#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <unordered_map>
#include <iomanip>
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

class QuizItem {
public:
    QuizItem(double difficulty, std::string question, std::vector<std::string> validAnswers, std::string partOfSpeech)
        : difficulty(difficulty), question(question), validAnswers(validAnswers), partOfSpeech(partOfSpeech) {}

    double getDifficulty() const {
        return difficulty;
    }

    const std::string& getQuestion() const {
        return question;
    }

    const std::vector<std::string>& getValidAnswers() const {
        return validAnswers;
    }

    const std::string& getPartOfSpeech() const {
        return partOfSpeech;
    }

private:
    double difficulty;
    std::string question;
    std::vector<std::string> validAnswers;
    std::string partOfSpeech;
};

class Quiz {
public:
    Quiz(const std::string& quizDataFile, const std::string& persistenceFile)
        : quizDataFile(quizDataFile), persistenceFile(persistenceFile) {}

    void start() {
        loadQuizData();
        loadPersistenceData();

        std::cout << "Welcome to the quiz!" << std::endl;
        std::cout << "-------------------------------------" << std::endl;

        for (int i = 0; i < quizItems.size(); ++i) {
            QuizItem& item = quizItems[i];
            double difficulty = item.getDifficulty();
            double nextDifficulty = ebisu.predictRecall(getCurrentTimestamp(), true);

            std::cout << "Quiz Item " << (i + 1) << std::endl;
            std::cout << "Question: " << item.getQuestion() << std::endl;
            std::cout << "Part of Speech: " << item.getPartOfSpeech() << std::endl;
            std::cout << "Difficulty: " << difficulty << std::endl;
            std::cout << "Predicted Next Difficulty: " << nextDifficulty << std::endl;

            std::string learnerAnswer;
            std::cout << "Enter your answer: ";
            std::getline(std::cin, learnerAnswer);

            int response = 0;
            for (const auto& validAnswer : item.getValidAnswers()) {
                if (learnerAnswer == validAnswer) {
                    response = 1;
                    break;
                }
            }

            ebisu.updateRecall(response, 1, getCurrentTimestamp());
            itemDifficultyMap[i] = difficulty;

            std::cout << "-------------------------------------" << std::endl;
        }

        std::cout << "Quiz completed!" << std::endl;

        savePersistenceData();
        generateStatisticalReport();
    }

private:
    Ebisu ebisu;
    std::vector<QuizItem> quizItems;
    std::unordered_map<int, double> itemDifficultyMap;
    std::string quizDataFile;
    std::string persistenceFile;

    double getCurrentTimestamp() const {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void loadQuizData() {
        std::ifstream file(quizDataFile);
        if (!file.is_open()) {
            std::cout << "Failed to open quiz data file: " << quizDataFile << std::endl;
            return;
        }

        try {
            json jsonData = json::parse(file);

            for (const auto& item : jsonData) {
                double difficulty = item["difficulty"];
                std::string question = item["kanji"];
                std::vector<std::string> validAnswers = item["meaning"];
                std::string partOfSpeech = item["part_of_speech"];

                QuizItem quizItem(difficulty, question, validAnswers, partOfSpeech);
                quizItems.push_back(quizItem);
            }

            file.close();
        } catch (const std::exception& e) {
            std::cout << "Failed to parse quiz data file: " << e.what() << std::endl;
        }
    }

    void savePersistenceData() {
        json jsonData;

        for (const auto& entry : itemDifficultyMap) {
            int itemId = entry.first;
            double difficulty = entry.second;
            jsonData.push_back({{"id", itemId}, {"difficulty", difficulty}});
        }

        std::ofstream file(persistenceFile);
        if (file.is_open()) {
            file << std::setw(4) << jsonData << std::endl;
            file.close();
        }
    }

    void loadPersistenceData() {
        std::ifstream file(persistenceFile);
        if (!file.is_open()) {
            std::cout << "Persistence data file not found. Starting with default difficulty values." << std::endl;
            return;
        }

        json jsonData;
        file >> jsonData;
        file.close();

        for (const auto& item : jsonData) {
            int itemId = item["id"];
            double difficulty = item["difficulty"];
            itemDifficultyMap[itemId] = difficulty;
        }
    }

    void generateStatisticalReport() {
        std::cout << "Statistical Report" << std::endl;
        std::cout << "-------------------------------------" << std::endl;

        std::vector<double> difficulties;
        for (const auto& entry : itemDifficultyMap) {
            double difficulty = entry.second;
            difficulties.push_back(difficulty);
        }

        double averageDifficulty = 0.0;
        if (!difficulties.empty()) {
            double sum = 0.0;
            for (double difficulty : difficulties) {
                sum += difficulty;
            }
            averageDifficulty = sum / difficulties.size();
        }

        std::cout << "Number of quiz items: " << itemDifficultyMap.size() << std::endl;
        std::cout << "Average difficulty: " << averageDifficulty << std::endl;

        std::cout << "-------------------------------------" << std::endl;
    }
};

int main() {
    std::string quizDataFile = "./quiz.json"; // Update the file path if necessary
    std::string persistenceFile = "persistence.json";

    Quiz quiz(quizDataFile, persistenceFile);
    quiz.start();

    return 0;
}