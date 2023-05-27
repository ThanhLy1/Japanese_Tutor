#ifndef QUIZ_H
#define QUIZ_H

#include <string>
#include <vector>
#include <random>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include "ebisu.h"
#include "vocab.h"

class Quiz {
private:
    std::vector<Vocab> vocabList;
    std::random_device rd;
    std::mt19937 generator;
    std::uniform_int_distribution<int> distribution;
    Ebisu ebisuModel;
    int totalQuestions = 0;
    int correctAnswers = 0;

public:
    Quiz();

    bool loadQuiz(const std::string& filename);
    void startQuiz();
    void askQuestion(const Vocab& vocab);
    void processAnswer(const Vocab& vocab, const std::string& userAnswer);
    Vocab getRandomVocab();
    void printStatistics() const;
    void saveQuizState();
    void loadQuizState();
};

Quiz::Quiz() : generator(rd()) {}

bool Quiz::loadQuiz(const std::string& filename) {
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

void Quiz::startQuiz() {
    int numQuestions = 10;
    for (int i = 0; i < numQuestions; ++i) {
        Vocab vocab = getRandomVocab();
        askQuestion(vocab);
        std::string userAnswer;
        std::getline(std::cin, userAnswer);

        // Check if the user wants to quit
        std::string answerLower = userAnswer;
        std::transform(answerLower.begin(), answerLower.end(),
                       answerLower.begin(), ::tolower);
        answerLower.erase(
            std::remove_if(answerLower.begin(), answerLower.end(),
                           ::isspace),
            answerLower.end());
        if (answerLower == "q" || answerLower == "quit") {
            std::cout << "Quiz terminated by the user." << std::endl;
            break;  // Exit the loop
        }
        processAnswer(vocab, userAnswer);
    }
}

void Quiz::askQuestion(const Vocab& vocab) {
    std::cout << "Translate the following word to English: "
              << vocab.getKanji() << std::endl;
}

void Quiz::processAnswer(const Vocab& vocab, const std::string& userAnswer) {
    std::string answerLower = userAnswer;
    std::transform(answerLower.begin(), answerLower.end(),
                   answerLower.begin(), ::tolower);
    answerLower.erase(
        std::remove_if(answerLower.begin(), answerLower.end(), ::isspace),
        answerLower.end());

    // Replace '-' with space
    std::replace(answerLower.begin(), answerLower.end(), '-', ' ');

    // Check if the user wants to quit
    if (answerLower == "q" || answerLower == "quit") {
        std::cout << "Quiz terminated by the user." << std::endl;
        exit(0);  // Exit the program
    }

    bool isCorrect = false;
    for (const auto& meaning : vocab.getMeaning()) {
        std::string meaningLower = meaning;
        std::transform(meaningLower.begin(), meaningLower.end(),
                       meaningLower.begin(), ::tolower);
        if (meaningLower == answerLower) {
            isCorrect = true;
            break;
        }
    }

    if (isCorrect) {
        std::cout << "Correct!" << std::endl;
        correctAnswers++;
    } else {
        std::cout << "Incorrect. The correct answer is: "
                  << vocab.correctAnswer() << std::endl;
    }
    totalQuestions++;
    double success = isCorrect ? 1.0 : 0.0;
    ebisuModel.updateRecall(success, 1.0, vocab.getDifficulty());
}

Vocab Quiz::getRandomVocab() {
    if (vocabList.empty()) {
        throw std::runtime_error("vocabList is empty!");
    }
    int index = distribution(generator);
    return vocabList[index];
}

void Quiz::printStatistics() const {
    std::cout << "Total questions: " << totalQuestions << std::endl;
    std::cout << "Correct answers: " << correctAnswers << std::endl;
    if (totalQuestions > 0) {
        double percentage = 100.0 * correctAnswers / totalQuestions;
        std::cout << "Accuracy: " << std::fixed << std::setprecision(2)
                  << percentage << "%" << std::endl;
    }
}

void Quiz::saveQuizState() {
    json quizState;
    quizState["total_questions"] = totalQuestions;
    quizState["correct_answers"] = correctAnswers;
    quizState["vocab_list"] = json::array();  // Create an empty JSON array

    // Convert the vocabList vector to a JSON array
    for (const auto& vocab : vocabList) {
        quizState["vocab_list"].push_back(vocab.toJson());
    }

    std::ofstream file("quiz_state.json");
    if (file.is_open()) {
        file << std::setw(4) << quizState << std::endl;
        file.close();
        std::cout << "Quiz state saved." << std::endl;
    } else {
        std::cout << "Unable to open file for saving quiz state."
                  << std::endl;
    }
}

void Quiz::loadQuizState() {
    std::ifstream file("quiz_state.json");
    if (!file.is_open()) {
        std::cout << "No quiz state found. Starting a fresh quiz."
                  << std::endl;
        return;
    }

    json quizData;
    try {
        file >> quizData;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse quiz state: " << e.what()
                  << std::endl;
        return;
    }

    if (quizData.contains("total_questions") &&
        quizData.contains("correct_answers") &&
        quizData.contains("vocab_list")) {
        totalQuestions = quizData["total_questions"];
        correctAnswers = quizData["correct_answers"];

        // Convert the JSON array to a vector of Vocab objects
        for (const auto& vocabData : quizData["vocab_list"]) {
            Vocab vocab(vocabData);
            vocabList.push_back(vocab);
        }

        std::cout << "Quiz state loaded." << std::endl;
    } else {
        std::cerr << "Invalid quiz state data." << std::endl;
    }
}

#endif
