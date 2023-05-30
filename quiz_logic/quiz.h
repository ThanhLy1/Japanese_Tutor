#ifndef QUIZ_H_
#define QUIZ_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <cctype>
#include <regex>
#include <chrono>
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
    int NUM_QUESTIONS;  // Updated to a non-constant member variable
    size_t totalQuestions_ = 0;
    int correctAnswers_ = 0;
    static const char QUIZ_STATE_FILE[];
    std::map<const Vocab*, std::chrono::steady_clock::time_point> lastQuestionTimes_;


public:
    Quiz()
        : rd_(),
          generator_(rd_()),
          distribution_(),
          ebisuModel_(),
          testType_(),
          NUM_QUESTIONS(10),  // Initialized directly in the constructor
          totalQuestions_(0),
          correctAnswers_(0)
    {}

    explicit Quiz(const std::vector<Vocab>& vocabList)
        : vocabList_(vocabList),
          rd_(),
          generator_(rd_()),
          distribution_(),
          ebisuModel_(),
          testType_(),
          NUM_QUESTIONS(10),  // Initialized directly in the constructor
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
    void processAnswer(const Vocab& vocab, const std::string& userAnswer, const std::chrono::steady_clock::time_point& now);

    Vocab getRandomVocab();
    void printStatistics() const;
    bool validateAnswer(const std::string& answer);
    void saveQuizState();
    void loadQuizState();

    // Needed for unit test otherwise it's protected class
    // std::string testType_;
    bool checkAnswer(const std::string& userAnswer, const std::string& correctAnswer);
    std::string trim(const std::string& str, const char& trimChar = ' ');
    std::string toLowercaseAndTrim(const std::string& str);
    void selectTestType();
    std::string getCorrectAnswer(const Vocab& vocab);
    std::string getUserAnswer();
    bool containsInvalidCharacters(const std::string& str);
    bool containsWhitespace(const std::string& str);
    bool hasLeadingOrTrailingWhitespace(const std::string& str);

    std::chrono::steady_clock::time_point getLastQuestionTime(const Vocab& vocab) const;
    void setLastQuestionTime(Vocab& vocab, const std::chrono::steady_clock::time_point& time);

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
        // std::cout << "Loaded JSON data: " << jsonData.dump(4) << std::endl;
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

void Quiz::saveQuizState() {
    nlohmann::json quizState;
    quizState["total_questions"] = totalQuestions_;
    quizState["correct_answers"] = correctAnswers_;
    quizState["vocab_list"] = nlohmann::json::array();

    for (const auto& vocab : vocabList_) {
        nlohmann::json vocabData = vocab.toJson();
        if (vocab.getLastQuestionTime() == std::chrono::steady_clock::time_point()) {
            vocabData["last_question_time"] = 0; // Indicate that last question time is not available
        } else {
            vocabData["last_question_time"] = vocab.getLastQuestionTime().time_since_epoch().count();
        }
        quizState["vocab_list"].push_back(vocabData);
    }

    quizState["ebisu_model"] = ebisuModel_.toJson();

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
            quizData.contains("vocab_list") &&
            quizData.contains("ebisu_model")) {
            totalQuestions_ = quizData["total_questions"];
            correctAnswers_ = quizData["correct_answers"];

            vocabList_.clear();
            for (const auto& vocabData : quizData["vocab_list"]) {
                Vocab vocab(vocabData);
                // Set the last question time for the vocabulary from the loaded JSON data
                auto lastQuestionTime = std::chrono::steady_clock::time_point();
                if (vocabData["last_question_time"].is_number()) {
                    lastQuestionTime = std::chrono::steady_clock::time_point(
                        std::chrono::nanoseconds(vocabData["last_question_time"].get<int64_t>())
                    );
                }
                vocab.setLastQuestionTime(lastQuestionTime);
                vocabList_.push_back(vocab);
            }

            ebisuModel_.fromJson(quizData["ebisu_model"]);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load quiz state: " << e.what() << std::endl;
    }
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
    std::map<int, std::string> quizTypeMap = {
        {1, "Kanji to Hiragana"},
        {2, "Hiragana to English"},
        {3, "Hiragana to Romaji"},
        {4, "English to Hiragana"}
    };

    std::cout << "Select the quiz type (Enter 'q' or 'quit' to exit):" << std::endl;
    for (const auto& quizType : quizTypeMap) {
        std::cout << quizType.first << ". " << quizType.second << std::endl;
    }

    const int maxAttempts = 3;
    int attempts = 0;
    std::string choice;

    while (attempts < maxAttempts) {
        std::cout << "Enter the number corresponding to the quiz type: ";
        std::getline(std::cin, choice);

        if (choice.empty()) {
            std::cerr << "Invalid input. Try again." << std::endl;
            ++attempts;
            continue;
        }

        if (choice == "q" || choice == "quit") {
            testType_.clear();
            return;
        }

        try {
            int quizType = std::stoi(choice);
            if (quizTypeMap.count(quizType) > 0) {
                testType_ = quizTypeMap[quizType];
                return;
            } else {
                std::cerr << "Invalid quiz type. Try again." << std::endl;
                ++attempts;
            }
        } catch (const std::exception& e) {
            std::cerr << "Invalid input. Try again." << std::endl;
            ++attempts;
        }
    }

    std::cerr << "Too many invalid attempts. Quiz aborted." << std::endl;
}

void Quiz::askQuestion(const Vocab& vocab) {
    std::cout << "-----------------------------" << std::endl;
    std::cout << "Question " << totalQuestions_ + 1 << ":" << std::endl;

    std::string question;
    std::string correctAnswer;

    // Get the last question time for the current vocab
    std::chrono::steady_clock::time_point lastQuestionTime = getLastQuestionTime(vocab);

    // Calculate the elapsed time since the last question in minutes
    auto now = std::chrono::steady_clock::now();
    auto elapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(now - lastQuestionTime).count();

    // Use the elapsed time to predict recall
    double predictedRecall = ebisuModel_.predictRecall(elapsedMinutes);

    if (testType_ == "Kanji to Hiragana") {
        question = "What is the hiragana reading of the following kanji? " + vocab.getKanji();
        correctAnswer = vocab.getHiragana();
    } else if (testType_ == "Hiragana to English") {
        question = "What is the English meaning of the following hiragana? " + vocab.getHiragana();
        correctAnswer = vocab.correctAnswer();
    } else if (testType_ == "Hiragana to Romaji") {
        question = "What is the romaji reading of the following hiragana? " + vocab.getHiragana();
        correctAnswer = vocab.getRomaji();
    } else if (testType_ == "English to Hiragana") {
        question = "What is the hiragana reading of the following English word? " + vocab.correctAnswer();
        correctAnswer = vocab.getHiragana();
    }

    std::cout << question << std::endl;
    std::string userAnswer = getUserAnswer();
    processAnswer(vocab, userAnswer, now); // Pass the current time as an argument
    std::cout << "Correct answer: " << correctAnswer << std::endl;
    std::cout << "Predicted Recall: " << std::fixed << std::setprecision(2) << (predictedRecall * 100) << "%" << std::endl;

    // Display the last question time for the current vocab
    if (lastQuestionTime == std::chrono::steady_clock::time_point()) {
        std::cout << "Last question time: Not available" << std::endl;
    } else {
        std::chrono::system_clock::time_point sysTimePoint = std::chrono::time_point_cast<std::chrono::system_clock::duration>(lastQuestionTime - std::chrono::steady_clock::now() + std::chrono::system_clock::now());
        std::time_t lastQuestionTimeT = std::chrono::system_clock::to_time_t(sysTimePoint);

        std::cout << "Last question time: " << std::ctime(&lastQuestionTimeT);
    }

    std::cout << "-----------------------------" << std::endl;

    // Update the last question time for the current vocab
    setLastQuestionTime(const_cast<Vocab&>(vocab), now);
    saveQuizState();
}

void Quiz::processAnswer(const Vocab& vocab, const std::string& userAnswer, const std::chrono::steady_clock::time_point& now) {
    std::string trimmedAnswer = trim(userAnswer);

    if (trimmedAnswer == "q" || trimmedAnswer == "quit") {
        std::cout << "Quiz aborted. Goodbye!" << std::endl;
        // Perform any necessary cleanup or termination steps
        saveQuizState();
        printStatistics();
        exit(0);
    }

    if (trimmedAnswer.empty()) {
        std::cout << "Please enter an answer." << std::endl;
        askQuestion(vocab);  // Ask the question again
        return;
    }

    if (validateAnswer(trimmedAnswer)) {
        std::string correctAnswer = getCorrectAnswer(vocab);

        if (checkAnswer(trimmedAnswer, correctAnswer)) {
            std::cout << "Correct!" << std::endl;
            ++correctAnswers_;
        } else {
            std::cout << "Incorrect." << std::endl;
        }

        // Calculate the elapsed time since the last question in minutes
        auto elapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(now - getLastQuestionTime(vocab)).count();
        ebisuModel_.updateRecall(correctAnswers_, totalQuestions_, elapsedMinutes);

        setLastQuestionTime(const_cast<Vocab&>(vocab), now);
        saveQuizState();

        // Increment the total questions counter
        ++totalQuestions_;
    } else {
        askQuestion(vocab);  // Ask the question again
    }
}

Vocab Quiz::getRandomVocab() {
    if (vocabList_.empty()) {
        throw std::runtime_error("No vocabularies loaded.");
    }

    int randomIndex = distribution_(generator_);
    return vocabList_[randomIndex];
}

void Quiz::printStatistics() const {
    std::cout << "Quiz Statistics:" << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << "Total Questions: " << totalQuestions_ << std::endl;
    std::cout << "Correct Answers: " << correctAnswers_ << std::endl;

    if (totalQuestions_ > 0) {
        double successRate = static_cast<double>(correctAnswers_) / totalQuestions_;
        
        // Calculate the elapsed time since the last question in minutes
        auto now = std::chrono::steady_clock::now();
        auto elapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(now - getLastQuestionTime(vocabList_.back())).count();
        
        // Predict recall based on the elapsed time
        double predictedRecall = ebisuModel_.predictRecall(elapsedMinutes);

        std::cout << "Success Rate: " << std::fixed << std::setprecision(2) << (successRate * 100) << "%" << std::endl;
        std::cout << "Predicted Recall: " << std::fixed << std::setprecision(2) << (predictedRecall * 100) << "%" << std::endl;
    }

    std::cout << "-----------------------------" << std::endl;
}


bool Quiz::validateAnswer(const std::string& answer) {
    std::string trimmedAnswer = trim(answer);
    std::string lowerAndTrimmedAnswer = toLowercaseAndTrim(trimmedAnswer);
    if(lowerAndTrimmedAnswer.empty()){
        std::cerr << "Please enter an answer." << std::endl;
        return false;
    }
    if(!utf8::is_valid(lowerAndTrimmedAnswer.begin(), lowerAndTrimmedAnswer.end())){
        std::cerr << "Answer contains invalid UTF-8 characters. Please try again." << std::endl;
        return false;
    }
    if(hasLeadingOrTrailingWhitespace(trimmedAnswer)){
        std::cerr << "Answer should not have leading or trailing whitespace. Please try again." << std::endl;
        return false;
    }
    if(containsInvalidCharacters(lowerAndTrimmedAnswer)){
        std::cerr << "Answer contains invalid characters. Please try again." << std::endl;
        return false;
    }
    if(lowerAndTrimmedAnswer.find("--") != std::string::npos){
        std::cerr << "Answer should not contain consecutive dashes (--). Please try again." << std::endl;
        return false;
    }
    return true;
}

bool Quiz::checkAnswer(const std::string& userAnswer, const std::string& correctAnswer) {
    //convert to lowercase and trim whitespace
    std::string userAnswerLowercase = toLowercaseAndTrim(userAnswer);
    std::string correctAnswerLowercase = toLowercaseAndTrim(correctAnswer);

    // Remove hyphens from user answer and correct answer
    userAnswerLowercase.erase(std::remove(userAnswerLowercase.begin(), userAnswerLowercase.end(), '-'), userAnswerLowercase.end());
    correctAnswerLowercase.erase(std::remove(correctAnswerLowercase.begin(), correctAnswerLowercase.end(), '-'), correctAnswerLowercase.end());

    // Check if the first or last character is a hyphen and return false if it is
    if (userAnswer.front() == '-' || userAnswer.back() == '-' || correctAnswer.front() == '-' || correctAnswer.back() == '-') {
        std::cerr << "Hyphens at the beginning or end of the answer are not allowed. Please try again." << std::endl;
        return false;
    }
    
    return userAnswerLowercase == correctAnswerLowercase;
}

std::string Quiz::trim(const std::string& str, const char& trimChar) {
    std::string trimmedString = str;
    trimmedString.erase(trimmedString.begin(), std::find_if(trimmedString.begin(), trimmedString.end(), [&](unsigned char ch) {
        return ch != trimChar;
    }));
    trimmedString.erase(std::find_if(trimmedString.rbegin(), trimmedString.rend(), [&](unsigned char ch) {
        return ch != trimChar;
    }).base(), trimmedString.end());
    return trimmedString;
}

std::string Quiz::toLowercaseAndTrim(const std::string& str) {
    std::string trimmedString = trim(str);
    std::transform(trimmedString.begin(), trimmedString.end(), trimmedString.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return trimmedString;
}

std::string Quiz::getCorrectAnswer(const Vocab& vocab) {
    if (testType_ == "Kanji to Hiragana") {
        return vocab.getHiragana();
    } else if (testType_ == "Hiragana to English") {
        return vocab.getEnglish()[0];
    } else if (testType_ == "Hiragana to Romaji") {
        return vocab.getRomaji();
    } else if (testType_ == "English to Hiragana") {
        return vocab.getHiragana();
    }
    return "Invalid test type.";
}

std::string Quiz::getUserAnswer() {
    std::string userAnswer;
    bool validAnswer = false;

    while (!validAnswer) {
        std::cout << "Enter your answer: ";
        std::getline(std::cin, userAnswer);

        validAnswer = validateAnswer(userAnswer);
    }

    return userAnswer;
}

bool Quiz::containsInvalidCharacters(const std::string& str) {
    const std::string invalidChars = "!@#$%^&*()_+[],./={}':;";
    std::regex reg("[" + std::regex_replace(invalidChars, std::regex("([\\[\\]])"), "\\$1") + "]");
    return std::regex_search(str, reg);
}

bool Quiz::containsWhitespace(const std::string& str) {
    return std::any_of(str.begin(), str.end(), [](char c) {
        return std::isspace(static_cast<unsigned char>(c));
    });
}

bool Quiz::hasLeadingOrTrailingWhitespace(const std::string& str) {
    return !str.empty() && (std::isspace(static_cast<unsigned char>(str.front())) || std::isspace(static_cast<unsigned char>(str.back())));
}

int Quiz::getTotalQuestions() const {
    return totalQuestions_;
}

int Quiz::getCorrectAnswers() const {
    return correctAnswers_;
}

std::chrono::steady_clock::time_point Quiz::getLastQuestionTime(const Vocab& vocab) const {
    auto it = lastQuestionTimes_.find(&vocab);
    if (it != lastQuestionTimes_.end()) {
        return it->second;
    }
    // Return the default time if the vocabulary is not found
    return std::chrono::steady_clock::time_point{};
}

void Quiz::setLastQuestionTime(Vocab& vocab, const std::chrono::steady_clock::time_point& time) {
    lastQuestionTimes_[&vocab] = time;
}

#endif  // QUIZ_H_
