// QuizFileHandler.h
#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include "vocab.h"

using json = nlohmann::json;

class QuizFileHandler {
public:
    bool loadQuiz(const std::string& filename, std::vector<Vocab>& vocabList);
    bool saveQuizState(const std::string& filename, int totalQuestions, int correctAnswers, const std::vector<Vocab>& vocabList);
    bool loadQuizState(const std::string& filename, int& totalQuestions, int& correctAnswers, std::vector<Vocab>& vocabList);
    bool resetQuizStateFile(const std::string& filename);

private:
    json convertVocabListToJson(const std::vector<Vocab>& vocabList);
    std::vector<Vocab> convertJsonToVocabList(const json& jsonData);
};

bool QuizFileHandler::loadQuiz(const std::string& filename, std::vector<Vocab>& vocabList) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    json jsonData;
    try {
        file >> jsonData;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse quiz data: " << e.what() << std::endl;
        return false;
    }

    if (jsonData.is_array()) {
        vocabList = convertJsonToVocabList(jsonData);
    } else {
        std::cerr << "Invalid quiz data format." << std::endl;
        return false;
    }

    return true;
}

bool QuizFileHandler::saveQuizState(const std::string& filename, int totalQuestions, int correctAnswers, const std::vector<Vocab>& vocabList) {
    json quizState;
    quizState["total_questions"] = totalQuestions;
    quizState["correct_answers"] = correctAnswers;
    quizState["vocab_list"] = convertVocabListToJson(vocabList);

    std::ofstream file(filename);
    if (file.is_open()) {
        file << std::setw(4) << quizState << std::endl;
        file.close();
        std::cout << "Quiz state saved." << std::endl;
        return true;
    } else {
        std::cout << "Unable to open file for saving quiz state." << std::endl;
        return false;
    }
}

bool QuizFileHandler::loadQuizState(const std::string& filename, int& totalQuestions, int& correctAnswers, std::vector<Vocab>& vocabList) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "No quiz state found. Starting a fresh quiz." << std::endl;
        return false;
    }

    json quizData;
    try {
        file >> quizData;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse quiz state: " << e.what() << std::endl;
        return false;
    }

    if (quizData.contains("total_questions") && quizData.contains("correct_answers") && quizData.contains("vocab_list")) {
        totalQuestions = quizData["total_questions"];
        correctAnswers = quizData["correct_answers"];
        vocabList = convertJsonToVocabList(quizData["vocab_list"]);
        return true;
    } else {
        std::cerr << "Invalid quiz state format." << std::endl;
        return false;
    }
}

json QuizFileHandler::convertVocabListToJson(const std::vector<Vocab>& vocabList) {
    json jsonData = json::array();
    for (const auto& vocab : vocabList) {
        jsonData.push_back(vocab.toJson());
    }
    return jsonData;
}

std::vector<Vocab> QuizFileHandler::convertJsonToVocabList(const json& jsonData) {
    std::vector<Vocab> vocabList;
    for (const auto& vocabData : jsonData) {
        Vocab vocab(vocabData);
        vocabList.push_back(vocab);
    }
    return vocabList;
}

bool QuizFileHandler::resetQuizStateFile(const std::string& filename) {
    if (std::remove(filename.c_str()) == 0) {
        std::cout << "Quiz state file reset." << std::endl;
        return true;
    } else {
        std::cerr << "Failed to reset quiz state file." << std::endl;
        return false;
    }
}