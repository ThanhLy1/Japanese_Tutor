#ifndef QUIZ_H
#define QUIZ_H

#include <string>
#include <vector>
#include <random>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <regex>

#include "ebisu.h"
#include "vocab.h"
#include "utf8/utf8.h"

class Quiz {
private:
    std::vector<Vocab> vocabList;
    std::random_device rd;
    std::mt19937 generator;
    std::uniform_int_distribution<int> distribution;
    std::string testType;
    Ebisu ebisuModel;
    int totalQuestions = 0;
    int correctAnswers = 0;
    const int NUM_QUESTIONS = 10; // Number of questions per quiz
    const std::string QUIZ_STATE_FILE = "quiz_state.json"; // File name for quiz state

public:
    /**
     * Default constructor for Quiz.
     */
    Quiz();

    /**
     * Loads the quiz from the specified file.
     * @param filename The filename of the quiz data.
     * @return True if the quiz was loaded successfully, false otherwise.
     */
    bool loadQuiz(const std::string& filename);

    /**
     * Starts the quiz and interacts with the user.
     */
    void startQuiz();

    /**
     * Starts a multiple-choice quiz.
     */
    void startMultipleChoiceQuiz();

    /**
     * Starts a fill-in-the-blank quiz.
     */
    void startFillInTheBlankQuiz();

    /**
     * Starts a listening comprehension exercise.
     */
    void startListeningComprehensionExercise();

    /**
     * Asks a question based on the given vocab.
     * @param vocab The Vocab object representing the question.
     */
    void askQuestion(const Vocab& vocab);

    /**
     * Processes the user's answer for the given vocab.
     * @param vocab The Vocab object representing the question.
     * @param userAnswer The user's answer.
     */
    void processAnswer(const Vocab& vocab, const std::string& userAnswer);

    /**
     * Retrieves a random Vocab object from the vocabList.
     * @return A random Vocab object.
     */
    Vocab getRandomVocab();

    /**
     * Prints the quiz statistics, including total questions and accuracy.
     */
    void printStatistics() const;

    /**
     * Saves the quiz state to a file.
     */
    void saveQuizState();

    /**
     * Loads the quiz state from a file.
     */
    void loadQuizState();

    /**
     * Checks if the string contains invalid characters.
     * @param str The string to check.
     * @return True if the string contains invalid characters, false otherwise.
     */
    bool containsInvalidCharacters(const std::string& str);

    bool containsWhitespace(const std::string& str);


    /**
     * Validates the user's answer to ensure it is not empty or contains only whitespace.
     * @param answer The user's answer.
     * @return True if the answer is valid, false otherwise.
     */
    bool validateAnswer(const std::string& answer);
    
        /**
     * Trims leading and trailing whitespace from a string.
     * @param str The string to trim.
     * @param trimChar The character to trim (default is ' ').
     * @return The trimmed string.
     */
    std::string trim(const std::string& str, const char& trimChar = ' ');

    /**
     * Converts a string to lowercase and trims leading and trailing whitespace.
     * @param str The string to convert.
     * @return The converted string.
     */
    std::string toLowercaseAndTrim(const std::string& str);


public: // change this to private back
    // Function declarations for quiz modes
    void askMultipleChoiceQuestion(const Vocab& vocab);
    void processMultipleChoiceAnswer(const Vocab& vocab, const std::string& userAnswer);
    void askFillInTheBlankQuestion(const Vocab& vocab);
    void processFillInTheBlankAnswer(const Vocab& vocab, const std::string& userAnswer);
    void askListeningComprehensionQuestion(const Vocab& vocab);
    void processListeningComprehensionAnswer(const Vocab& vocab, const std::string& userAnswer);

    /**
     * Checks if the user's answer is correct.
     * @param userAnswer The user's answer.
     * @param correctAnswer The correct answer.
     * @return True if the user's answer is correct, false otherwise.
     */
    bool checkAnswer(const std::string& userAnswer, const std::string& correctAnswer);

    /**
     * Retrieves the correct answer based on the test type.
     * @param vocab The Vocab object.
     * @param testType The type of test being conducted.
     * @return The correct answer.
     */
    std::string getCorrectAnswer(const Vocab& vocab);
};

void Quiz::askMultipleChoiceQuestion(const Vocab& vocab) {
    // code for asking a multiple choice question
    std::cout << "Translate the following word to English: "
        << vocab.getKanji() << std::endl;
    std::cout << "1. Option 1" << std::endl;
    std::cout << "2. Option 2" << std::endl;
    std::cout << "3. Option 3" << std::endl;
    std::cout << "4. Option 4" << std::endl;
}

void Quiz::processMultipleChoiceAnswer(const Vocab& vocab, const std::string& userAnswer) {
    // code for processing a multiple choice answer
    // For simplicity, let's assume that user's answer is the number of correct option
}

void Quiz::askFillInTheBlankQuestion(const Vocab& vocab) {
    // code for asking a fill-in-the-blank question
    std::cout << "Fill in the blank with the correct translation: ";
}

void Quiz::processFillInTheBlankAnswer(const Vocab& vocab, const std::string& userAnswer) {
    // code for processing a fill-in-the-blank answer
}

void Quiz::askListeningComprehensionQuestion(const Vocab& vocab) {
    // code for asking a listening comprehension question
    std::cout << "Listen to the following audio clip and write the English translation: "
        << std::endl;
    // Play audio clip here
}

void Quiz::processListeningComprehensionAnswer(const Vocab& vocab, const std::string& userAnswer) {
    // code for processing a listening comprehension answer

}

Quiz::Quiz() : generator(rd()) {}

bool Quiz::loadQuiz(const std::string& filename) {
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
        for (const auto& vocabData : jsonData) {
            try {
                Vocab vocab(vocabData);
                vocabList.push_back(vocab);
            } catch (const std::exception& e) {
                std::cerr << "Failed to parse vocab data: " << e.what() << std::endl;
            }
        }
    } else {
        std::cerr << "Invalid quiz data format." << std::endl;
        return false;
    }

    distribution = std::uniform_int_distribution<int>(0, static_cast<int>(vocabList.size()) - 1);
    return true;
}

void Quiz::startQuiz() {
    std::cout << "Select the test type:" << std::endl;
    std::cout << "1. Kanji to Hiragana" << std::endl;
    std::cout << "2. Hiragana to English" << std::endl;
    std::cout << "3. Romaji to English" << std::endl;
    std::cout << "4. English to Romaji" << std::endl;

    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    std::map<int, std::string> testTypeMap = {
        {1, "KanjiHiragana"},
        {2, "Hiragana"},
        {3, "Romaji"},
        {4, "English"}
    };

    auto it = testTypeMap.find(choice);
    testType = (it != testTypeMap.end()) ? it->second : "KanjiHiragana";

    for (int i = 0; i < NUM_QUESTIONS; ++i) {
        Vocab vocab = getRandomVocab();
        askQuestion(vocab);
        std::string userAnswer;
        bool isValidAnswer = false;

        while (!isValidAnswer) {
            std::getline(std::cin, userAnswer);

            // Check if the user wants to quit
            std::string answerLower = userAnswer;
            std::transform(answerLower.begin(), answerLower.end(), answerLower.begin(), ::tolower);
            answerLower.erase(std::remove_if(answerLower.begin(), answerLower.end(), ::isspace), answerLower.end());
            if (answerLower == "q" || answerLower == "quit") {
                std::cout << "Quiz terminated by the user." << std::endl;
                return;  // Exit the function
            }

            isValidAnswer = validateAnswer(userAnswer);

            if (!isValidAnswer) {
                std::cout << "Invalid answer. Please try again: ";
            }
        }
        processAnswer(vocab, userAnswer);
    }
}

void Quiz::askQuestion(const Vocab& vocab) {
    std::string question;
    std::string correctAnswer;

    if (testType == "KanjiHiragana") {
        question = "Translate the following kanji to hiragana: ";
        correctAnswer = vocab.getHiragana();
    }
    else if (testType == "Hiragana") {
        question = "Translate the following hiragana to English: ";
        correctAnswer = vocab.getEnglish()[0];
    }
    else if (testType == "Romaji") {
        question = "Translate the following romaji to English: ";
        correctAnswer = vocab.getEnglish()[0];
    }
    else if (testType == "English") {
        question = "Translate the following English word to romaji: ";
        correctAnswer = vocab.getRomaji();
    }
    else {
        std::cout << "Invalid test type." << std::endl;
        return;
    }

    std::cout << question << vocab.getKanji() << std::endl;

    // Rest of the code...
}

void Quiz::processAnswer(const Vocab& vocab, const std::string& userAnswer) {
    std::string lowerAndTrimmedAnswer = toLowercaseAndTrim(userAnswer);

    // Check if the user wants to quit
    if (lowerAndTrimmedAnswer == "q" || lowerAndTrimmedAnswer == "quit") {
        std::cout << "Quiz terminated by the user." << std::endl;
        exit(0);  // Exit the program
    }

    bool isCorrect = false;

    if (testType == "KanjiHiragana") {
        isCorrect = checkAnswer(lowerAndTrimmedAnswer, vocab.getHiragana());
    }
    else if (testType == "Hiragana") {
        isCorrect = checkAnswer(lowerAndTrimmedAnswer, vocab.getEnglish()[0]);
    }
    else if (testType == "Romaji") {
        isCorrect = checkAnswer(lowerAndTrimmedAnswer, vocab.getEnglish()[0]);
    }
    else if (testType == "English") {
        isCorrect = checkAnswer(lowerAndTrimmedAnswer, vocab.getRomaji());
    }
    else {
        std::cout << "Invalid test type." << std::endl;
        return;
    }

    if (isCorrect) {
        std::cout << "Correct!" << std::endl;
        correctAnswers++;
    }
    else {
        std::cout << "Incorrect. The correct answer is: " << getCorrectAnswer(vocab) << std::endl;
    }

    totalQuestions++;
    double success = isCorrect ? 1.0 : 0.0;
    ebisuModel.updateRecall(success, 1.0, vocab.getDifficulty());
}

bool Quiz::checkAnswer(const std::string& userAnswer, const std::string& correctAnswer) {
    return userAnswer == correctAnswer;
}

std::string Quiz::getCorrectAnswer(const Vocab& vocab) {
    if (testType == "KanjiHiragana") {
        return vocab.getHiragana();
    }
    else if (testType == "Hiragana") {
        return vocab.getEnglish()[0];
    }
    else if (testType == "Romaji") {
        return vocab.getEnglish()[0];
    }
    else if (testType == "English") {
        return vocab.getRomaji();
    }
    else {
        return "Invalid test type.";
    }
}

Vocab Quiz::getRandomVocab() {
    if (vocabList.empty()) {
        std::cerr << "vocabList is empty!" << std::endl;
        // Return an empty Vocab object as a fallback
        return Vocab();
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

    std::ofstream file(QUIZ_STATE_FILE);
    if (file.is_open()) {
        file << std::setw(4) << quizState << std::endl;
        file.close();
        std::cout << "Quiz state saved." << std::endl;
    }
    else {
        std::cout << "Unable to open file for saving quiz state." << std::endl;
    }
}

void Quiz::loadQuizState() {
    std::ifstream file(QUIZ_STATE_FILE);
    if (!file.is_open()) {
        std::cout << "No quiz state found. Starting a fresh quiz." << std::endl;
        return;
    }

    json quizData;
    try {
        file >> quizData;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to parse quiz state: " << e.what() << std::endl;
        return;
    }

    if (quizData.contains("total_questions") &&
        quizData.contains("correct_answers") &&
        quizData.contains("vocab_list")) {
        totalQuestions = quizData["total_questions"];
        correctAnswers = quizData["correct_answers"];

        // Clear the current vocabList before loading new items
        vocabList.clear();

        // Convert the JSON array to a vector of Vocab objects
        for (const auto& vocabData : quizData["vocab_list"]) {
            Vocab vocab(vocabData);
            vocabList.push_back(vocab);
        }
    }
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

bool Quiz::containsWhitespace(const std::string& str) {
    return str.find(' ') != std::string::npos;
}

bool hasLeadingOrTrailingWhitespace(const std::string& str) {
    return !str.empty() && (str.front() == ' ' || str.back() == ' ');
}

bool Quiz::containsInvalidCharacters(const std::string& str) {
    const std::string invalidChars = "!@#$%^&*()_+[],./={}':;";
    std::regex reg("[" + std::regex_replace(invalidChars, std::regex("([\\[\\]-])"), "\\$1") + "]");
    return std::regex_search(str, reg);
}

// @see: utf8 https://github.com/nemtrif/utfcpp
bool Quiz::validateAnswer(const std::string& answer) {
    std::string trimmedAnswer = trim(answer);
    std::string lowerAndTrimmedAnswer = toLowercaseAndTrim(trimmedAnswer);
    return !lowerAndTrimmedAnswer.empty()
        && utf8::is_valid(lowerAndTrimmedAnswer.begin(), lowerAndTrimmedAnswer.end())
        && !hasLeadingOrTrailingWhitespace(trimmedAnswer)
        && !containsInvalidCharacters(lowerAndTrimmedAnswer)
        && lowerAndTrimmedAnswer.find("--") == std::string::npos;
}

#endif
