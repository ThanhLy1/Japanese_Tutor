#ifndef QUIZ_H_
#define QUIZ_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>
#include <limits>
#include <iomanip>
#include <map>

#include "ebisu.h"
#include "vocab.h"
#include "utf8/utf8.h"

class Quiz {
private:
    std::vector<Vocab> vocabList_;
    std::random_device rd_;
    std::mt19937 generator_;
    std::uniform_int_distribution<int> distribution_;
    Ebisu ebisuModel_;
    std::string testType_;
    size_t totalQuestions_ = 0;
    int correctAnswers_ = 0;
    const int NUM_QUESTIONS = 10;
    static const char QUIZ_STATE_FILE[];

public:
    //TODO: let user choose the amount of question
    Quiz()
        : vocabList_(),
          rd_(),
          generator_(rd_()),
          distribution_(),
          ebisuModel_(),
          testType_(),
          totalQuestions_(0),
          correctAnswers_(0),
          NUM_QUESTIONS(10)
    {}

    explicit Quiz(const std::vector<Vocab>& vocabList)
        : vocabList_(vocabList),
          totalQuestions_(0),
          correctAnswers_(0)
    {}

    void addVocab(const Vocab& vocab) { vocabList_.push_back(vocab); }
    bool isFinished() const { return totalQuestions_ == vocabList_.size(); }
    bool checkAnswer(const Vocab& vocab, const std::string& answer);
    int getTotalQuestions() const;
    int getCorrectAnswers() const;
    bool loadQuiz(const std::string& filename);
    void startQuiz();
    void askQuestion(const Vocab& vocab);
    void processAnswer(const Vocab& vocab, const std::string& userAnswer);
    Vocab getRandomVocab();
    void printStatistics() const;
    bool validateAnswer(const std::string& answer);
    void saveQuizState();
    void loadQuizState();

    // Needed for unit test otherwise it's protected class
    //std::string testType_;
    bool checkAnswer(const std::string& userAnswer, const std::string& correctAnswer);
    std::string trim(const std::string& str, const char& trimChar = ' ');
    std::string toLowercaseAndTrim(const std::string& str);
    void selectTestType();
    std::string getCorrectAnswer(const Vocab& vocab);
    std::string getUserAnswer();
    bool containsInvalidCharacters(const std::string& str);
    bool containsWhitespace(const std::string& str);
    bool hasLeadingOrTrailingWhitespace(const std::string& str);

private:

};

const char Quiz::QUIZ_STATE_FILE[] = "quiz_state.json";

bool Quiz::loadQuiz(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    nlohmann::json jsonData;
    try {
        file >> jsonData;
        std::cout << "Loaded JSON data: "<< std::endl;
    #ifndef DEBUG
        //std::cout << "Loaded JSON data: " << jsonData.dump(4) << std::endl;
    #endif
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse quiz data: " << e.what() << std::endl;
        return false;
    }

    if (jsonData.is_array()) {
        for (const auto& vocabData : jsonData) {
            try {
                Vocab vocab(vocabData);
                vocabList_.push_back(vocab);
            } catch (const std::exception& e) {
                std::cerr << "Failed to parse vocab data: " << e.what() << std::endl;
            }
        }
    } else {
        std::cerr << "Invalid quiz data format." << std::endl;
        return false;
    }

    distribution_ = std::uniform_int_distribution<int>(0, static_cast<int>(vocabList_.size()) - 1);
    return true;
}

void Quiz::startQuiz() {
    selectTestType();
    if (testType_.empty()) {
        std::cout << "Invalid test type. Quiz aborted." << std::endl;
        return;
    }

    for (int i = 0; i < NUM_QUESTIONS; ++i) {
        Vocab vocab = getRandomVocab();
        askQuestion(vocab);
    }
}

void Quiz::selectTestType() {
    std::map<int, std::string> testTypeMap = {
        {1, "Kanji to Hiragana"},
        {2, "Hiragana to English"},
        {3, "Hiragana to Romaji"},
        {4, "English to Hiragana"}
    };

    std::cout << "Select the test type:" << std::endl;
    for (const auto& testType : testTypeMap) {
        std::cout << testType.first << ". " << testType.second << std::endl;
    }

    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    auto it = testTypeMap.find(choice);
    if (it == testTypeMap.end()) {
        std::cout << "Invalid test type. Quiz aborted." << std::endl;
        return;
    }

    testType_ = it->second;
}

std::string Quiz::getUserAnswer() {
    std::string userAnswer;

    while (true) {
        std::getline(std::cin, userAnswer);

        if (userAnswer.empty()) {
            std::cout << "Invalid answer. Please try again: ";
            continue;
        }

        std::string answerLower = toLowercaseAndTrim(userAnswer);
        if (answerLower == "q" || answerLower == "quit") {
            std::cout << "Quiz terminated by the user." << std::endl;
            saveQuizState();
            // You may choose not to exit here, but handle the termination appropriately
            exit(0);
        }

        if (validateAnswer(userAnswer)) {
            break;
        }

        std::cout << "Invalid answer. Please try again: ";
    }
    return userAnswer;
}

void Quiz::processAnswer(const Vocab& vocab, const std::string& userAnswer) {
    std::string lowerAndTrimmedAnswer = toLowercaseAndTrim(userAnswer);

    if (lowerAndTrimmedAnswer == "") {
        std::cout << "Invalid answer. Please try again." << std::endl;
        askQuestion(vocab);  // Ask the question again
        return;
    }

    bool isCorrect = false;

    if (testType_ == "Kanji to Hiragana") {
        isCorrect = checkAnswer(lowerAndTrimmedAnswer, vocab.getHiragana());
    }
    else if (testType_ == "Hiragana to English") {
        isCorrect = checkAnswer(lowerAndTrimmedAnswer, vocab.getEnglish()[0]);
    }
    else if (testType_ == "Hiragana to Romaji") {
        isCorrect = checkAnswer(lowerAndTrimmedAnswer, vocab.getRomaji());
    }
    else if (testType_ == "English to Hiragana") {
        isCorrect = checkAnswer(lowerAndTrimmedAnswer, vocab.getHiragana());
    }
    else {
        std::cout << "Invalid test type." << std::endl;
        return;
    }

    if (isCorrect) {
        std::cout << "Correct!" << std::endl;
        correctAnswers_++;
    }
    else {
        std::cout << "Incorrect. The correct answer is: " << getCorrectAnswer(vocab) << std::endl;
    }
    totalQuestions_++;
    double success = isCorrect ? 1.0 : 0.0;
    ebisuModel_.updateRecall(success, 1.0, vocab.getDifficulty());
}

void Quiz::askQuestion(const Vocab& vocab) {
    std::string question;
    std::string correctAnswer;

    if (testType_ == "Kanji to Hiragana") {
        question = "Translate the following kanji to hiragana: ";
        correctAnswer = vocab.getHiragana();
        std::cout << question << vocab.getKanji() << std::endl;
    }
    else if (testType_ == "Hiragana to English") {
        question = "Translate the following hiragana to English: ";
        correctAnswer = vocab.getEnglish()[0];
        std::cout << question << vocab.getHiragana() << std::endl;
    }
    else if (testType_ == "Hiragana to Romaji") {
        question = "Translate the following hiragana to romaji: ";
        correctAnswer = vocab.getRomaji()[0];
        std::cout << question << vocab.getHiragana() << std::endl;
    }
    else if (testType_ == "English to Hiragana") {
        question = "Translate the following English word to hiragana: ";
        correctAnswer = vocab.getHiragana();
        std::cout << question << vocab.getEnglish()[0] << std::endl;
    }
    else {
        std::cout << "Invalid test type." << std::endl;
        return;
    }

    while (true) {
        std::string userAnswer = getUserAnswer();
        if (userAnswer.empty()) {
            continue;  // Ask the question again if the user input is empty
        }
        if (userAnswer == "q" || userAnswer == "quit") {
            std::cout << "Quiz terminated by the user." << std::endl;
            return;  // Terminate the quiz if the user enters "q" or "quit"
        }
        processAnswer(vocab, userAnswer);
        break;
    }
}

Vocab Quiz::getRandomVocab() {
    if (vocabList_.empty()) {
        std::cerr << "vocabList is empty!" << std::endl;
        // Return an empty Vocab object as a fallback
        return Vocab();
    }
    int index = distribution_(generator_);
    return vocabList_[index];
}

void Quiz::printStatistics() const {
    std::cout << "Total questions: " << totalQuestions_ << std::endl;
    std::cout << "Correct answers: " << correctAnswers_ << std::endl;
    if (totalQuestions_ > 0) {
        double percentage = 100.0 * correctAnswers_ / totalQuestions_;
        std::cout << "Accuracy: " << std::fixed << std::setprecision(2)
            << percentage << "%" << std::endl;
    }
}

void Quiz::saveQuizState() {
    nlohmann::json quizState;
    quizState["total_questions"] = totalQuestions_;
    quizState["correct_answers"] = correctAnswers_;
    quizState["vocab_list"] = nlohmann::json::array();

    for (const auto& vocab : vocabList_) {
        quizState["vocab_list"].push_back(vocab.toJson());
    }

    try {
        std::ofstream file(QUIZ_STATE_FILE);
        file << std::setw(4) << quizState << std::endl;
        std::cout << "Quiz state saved." << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Unable to open file for saving quiz state: " << e.what() << std::endl;
    }
}

void Quiz::loadQuizState() {
    try {
        std::ifstream file(QUIZ_STATE_FILE);

        if (!file) {
            throw std::runtime_error("No quiz state found. Starting a fresh quiz.");
        }

        nlohmann::json quizData;
        file >> quizData;

        if (quizData.contains("total_questions") &&
            quizData.contains("correct_answers") &&
            quizData.contains("vocab_list")) {
            totalQuestions_ = quizData["total_questions"];
            correctAnswers_ = quizData["correct_answers"];

            vocabList_.clear();
            for (const auto& vocabData : quizData["vocab_list"]) {
                Vocab vocab(vocabData);
                vocabList_.push_back(vocab);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load quiz state: " << e.what() << std::endl;
    }
}

bool Quiz::checkAnswer(const std::string& userAnswer, const std::string& correctAnswer) {
    // Convert both answers to lowercase and remove dashes
    std::string lowerUserAnswer = userAnswer;
    std::string lowerCorrectAnswer = correctAnswer;
    std::transform(lowerUserAnswer.begin(), lowerUserAnswer.end(), lowerUserAnswer.begin(), ::tolower);
    std::transform(lowerCorrectAnswer.begin(), lowerCorrectAnswer.end(), lowerCorrectAnswer.begin(), ::tolower);
    lowerUserAnswer.erase(std::remove(lowerUserAnswer.begin(), lowerUserAnswer.end(), '-'), lowerUserAnswer.end());
    lowerCorrectAnswer.erase(std::remove(lowerCorrectAnswer.begin(), lowerCorrectAnswer.end(), '-'), lowerCorrectAnswer.end());

    return lowerUserAnswer == lowerCorrectAnswer;
}

std::string Quiz::getCorrectAnswer(const Vocab& vocab) {
    if (testType_ == "Kanji to Hiragana") {
        return vocab.getHiragana();
    }
    else if (testType_ == "Hiragana to English") {
        return vocab.getEnglish()[0];
    }
    else if (testType_ == "Hiragana to Romaji") {
        return vocab.getRomaji();
    }
    else if (testType_ == "English to Hiragana") {
        return vocab.getHiragana();
    }
    else {
        return "Invalid test type.";
    }
}

bool Quiz::validateAnswer(const std::string& answer) {
    std::string trimmedAnswer = trim(answer);
    std::string lowerAndTrimmedAnswer = toLowercaseAndTrim(trimmedAnswer);
    return !lowerAndTrimmedAnswer.empty()
        && utf8::is_valid(lowerAndTrimmedAnswer.begin(), lowerAndTrimmedAnswer.end())
        && !hasLeadingOrTrailingWhitespace(trimmedAnswer)
        && !containsInvalidCharacters(lowerAndTrimmedAnswer)
        && lowerAndTrimmedAnswer.find("--") == std::string::npos;
}

std::string Quiz::trim(const std::string& str, const char& trimChar) {
    size_t first = str.find_first_not_of(trimChar);
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(trimChar);
    return str.substr(first, (last - first + 1));
}

std::string Quiz::toLowercaseAndTrim(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return trim(lower);
}

bool Quiz::containsInvalidCharacters(const std::string& str) {
    const std::string invalidChars = "!@#$%^&*()_+[],./={}':;";
    std::regex reg("[" + std::regex_replace(invalidChars, std::regex("([\\[\\]-])"), "\\$1") + "]");
    return std::regex_search(str, reg);
}

bool Quiz::containsWhitespace(const std::string& str) {
    return std::any_of(str.begin(), str.end(), ::isspace);
}

bool Quiz::hasLeadingOrTrailingWhitespace(const std::string& str) {
    return !str.empty() && (std::isspace(str.front()) || std::isspace(str.back()));
}

int Quiz::getTotalQuestions() const {
    return totalQuestions_;
}

int Quiz::getCorrectAnswers() const {
    return correctAnswers_;
}

#endif  // QUIZ_H_
