// QuestionHandler.h
#pragma once
#include <iostream>
#include <algorithm>
#include <cctype>

#include "vocab.h"

class QuestionHandler {
public:
    void askQuestion(const Vocab& vocab, const std::string& questionFormat);
    bool processAnswer(const Vocab& vocab, const std::string& userAnswer, const std::string& correctAnswer);
    std::string getCorrectAnswer(const Vocab& vocab, const std::string& questionFormat);
};

void QuestionHandler::askQuestion(const Vocab& vocab, const std::string& questionFormat) {
    std::cout << questionFormat << vocab.getQuestionData() << std::endl;
}

bool QuestionHandler::processAnswer(const Vocab& vocab, const std::string& userAnswer, const std::string& correctAnswer) {
    std::string lowerAndTrimmedAnswer = toLowercaseAndTrim(userAnswer);

    // Check if the user wants to quit
    if (lowerAndTrimmedAnswer == "q" || lowerAndTrimmedAnswer == "quit") {
        std::cout << "Quiz terminated by the user." << std::endl;
        exit(0);  // Exit the program
    }

    bool isCorrect = checkAnswer(lowerAndTrimmedAnswer, correctAnswer);

    if (isCorrect) {
        std::cout << "Correct!" << std::endl;
    } else {
        std::cout << "Incorrect. The correct answer is: " << getCorrectAnswer(vocab, correctAnswer) << std::endl;
    }

    return isCorrect;
}

std::string QuestionHandler::getCorrectAnswer(const Vocab& vocab, const std::string& questionFormat) {
    if (questionFormat == "Translate the following kanji to hiragana: ")
        return vocab.getHiragana();
    else if (questionFormat == "Translate the following hiragana to English: ")
        return vocab.getEnglish()[0];
    else if (questionFormat == "Translate the following hiragana to romaji: ")
        return vocab.getRomaji();
    else if (questionFormat == "Translate the following English word to hiragana: ")
        return vocab.getHiragana();
    else
        return "Invalid test type.";
}