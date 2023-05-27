#ifndef QUIZ_H
#define QUIZ_H

#include <string>
#include <vector>
#include <random>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
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

    /**
     * Validates the user's answer to ensure it is not empty or contains only whitespace.
     * @param answer The user's answer.
     * @return True if the answer is valid, false otherwise.
     */
    bool validateAnswer(const std::string& answer);

    void testTrim();
    void testContainsInvalidCharacters();

    /**
     * Runs unit tests for the Quiz class.
     */
    void runUnitTests();

private:
    /**
     * Unit test for the validateAnswer function.
     */
    void testValidateAnswer();
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
    distribution = std::uniform_int_distribution<int>(0, static_cast<int>(vocabList.size()) - 1);
    return true;
}

void Quiz::startQuiz() {
    for (int i = 0; i < NUM_QUESTIONS; ++i) {
        Vocab vocab = getRandomVocab();
        askQuestion(vocab);
        std::string userAnswer;
        bool isValidAnswer = false;

        while (!isValidAnswer) {
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
    }
    else {
        std::cout << "Incorrect. The correct answer is: "
            << vocab.correctAnswer() << std::endl;
    }
    totalQuestions++;
    double success = isCorrect ? 1.0 : 0.0;
    ebisuModel.updateRecall(success, 1.0, vocab.getDifficulty());
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
        std::cout << "Unable to open file for saving quiz state."
            << std::endl;
    }
}

void Quiz::loadQuizState() {
    std::ifstream file(QUIZ_STATE_FILE);
    if (!file.is_open()) {
        std::cout << "No quiz state found. Starting a fresh quiz."
            << std::endl;
        return;
    }

    json quizData;
    try {
        file >> quizData;
    }
    catch (const std::exception& e) {
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
    }
    else {
        std::cerr << "Invalid quiz state data." << std::endl;
    }
}

std::string trim(const std::string& str, const char& trimChar = ' ')
{
    size_t first = str.find_first_not_of(trimChar);
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(trimChar);
    return str.substr(first, (last - first + 1));
}

bool containsWhitespace(const std::string& str)
{
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

bool Quiz::validateAnswer(const std::string& answer) {
    std::string trimmedAnswer = trim(answer);
    return !trimmedAnswer.empty()
        && utf8::is_valid(trimmedAnswer.begin(), trimmedAnswer.end())
        && !hasLeadingOrTrailingWhitespace(trimmedAnswer)
        && !containsInvalidCharacters(trimmedAnswer)
        && trimmedAnswer.find("--") == std::string::npos;
}

void Quiz::testTrim()
{
    std::cout << "Running unit test for trim()" << std::endl;

    std::string str = "   test   ";
    std::cout << "Original string: \"" << str << "\"" << std::endl;
    std::cout << "Trimmed string with ' ': \"" << trim(str, ' ') << "\"" << std::endl;
    std::cout << "Trimmed string with '\\t': \"" << trim(str, '\t') << "\"" << std::endl;
    std::cout << "Trimmed string with '\\n': \"" << trim(str, '\n') << "\"" << std::endl;

    std::cout << "Unit test completed." << std::endl;
}

void Quiz::testContainsInvalidCharacters()
{
    std::cout << "Running unit test for containsInvalidCharacters()" << std::endl;

    std::string str = "test@answer";
    std::cout << "Checking string: \"" << str << "\"" << std::endl;
    std::cout << "Contains invalid characters: " << containsInvalidCharacters(str) << std::endl;

    str = "test.answer";
    std::cout << "Checking string: \"" << str << "\"" << std::endl;
    std::cout << "Contains invalid characters: " << containsInvalidCharacters(str) << std::endl;

    std::cout << "Unit test completed." << std::endl;
}

void Quiz::runUnitTests()
{
    testTrim();
    testContainsInvalidCharacters();
    testValidateAnswer();
}

void Quiz::testValidateAnswer()
{
    std::cout << "Running unit test for validateAnswer()" << std::endl;

    std::vector<std::pair<std::string, bool>> tests = {
        {"", false},
        {"   ", false},
        {" test answer", true},
        {"test answer ", true},
        {"test@answer", false},
        {"test.answer", false},
        {"test-answer", true},
        {"test answer", true},
        {"こんにちは", true},
        {"1234", true},
        {"abcXYZ", true}
    };

    for (const auto& test : tests)
    {
        std::cout << "Input: \"" << test.first << "\", Expected: " << test.second
            << ", Actual: " << validateAnswer(test.first) << "\n";
    }

    std::cout << "Unit test completed." << std::endl;
}

#endif // QUIZ_H
