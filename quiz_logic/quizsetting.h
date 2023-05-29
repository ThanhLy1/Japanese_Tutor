// QuizSettings.h
#pragma once

#include <string>
#include <map>

class QuizSettings {
public:
    QuizSettings();

    void selectTestType(int choice);
    std::string getQuestionFormat() const;

private:
    std::map<int, std::string> testTypeMap;
    std::string testType;
};

QuizSettings::QuizSettings() {
    testTypeMap = {
        {1, "Kanji to Hiragana"},
        {2, "Hiragana to English"},
        {3, "Hiragana to Romaji"},
        {4, "English to Hiragana"}
    };
}

void QuizSettings::selectTestType(int choice) {
    auto it = testTypeMap.find(choice);
    testType = (it != testTypeMap.end()) ? it->second : "Kanji to Hiragana";
}

std::string QuizSettings::getQuestionFormat() const {
    if (testType == "Kanji to Hiragana")
        return "Translate the following kanji to hiragana: ";
    else if (testType == "Hiragana to English")
        return "Translate the following hiragana to English: ";
    else if (testType == "Hiragana to Romaji")
        return "Translate the following hiragana to romaji: ";
    else if (testType == "English to Hiragana")
        return "Translate the following English word to hiragana: ";
    else
        return "Invalid test type.";
}