#include "quiz_logic/quiz.h"
#include "quiz_logic/vocab.h"
#include "gtest/gtest.h"

// Forward declarations for the protected and private member functions
bool checkAnswer(Quiz& quiz, const std::string& userAnswer, const std::string& correctAnswer);
std::string trim(Quiz& quiz, const std::string& str, const char& trimChar = ' ');
std::string toLowercaseAndTrim(Quiz& quiz, const std::string& str);

// Declare friend functions to access protected and private members of Quiz
namespace QuizTestInternal {
    // Friend function to access protected function checkAnswer()
    bool checkAnswer(Quiz& quiz, const std::string& userAnswer, const std::string& correctAnswer) {
        return quiz.checkAnswer(userAnswer, correctAnswer);
    }

    // Friend function to access private function trim()
    std::string trim(Quiz& quiz, const std::string& str, const char& trimChar = ' ') {
        return quiz.trim(str, trimChar);
    }

    // Friend function to access private function toLowercaseAndTrim()
    std::string toLowercaseAndTrim(Quiz& quiz, const std::string& str) {
        return quiz.toLowercaseAndTrim(str);
    }
}

class QuizTest : public ::testing::Test {
protected:
    Quiz quiz;

    // Helper function to create sample vocabularies for testing
    void createSampleVocabularies() {
        try {
            Vocab vocab1({
            {"kanji", "Kanji"},
            {"hiragana", "Hiragana"},
            {"romaji", "Romaji"},
            {"english", std::vector<std::string>{"English"}},  // Wrap the string in a vector
            {"difficulty", 1.0}
        });
        Vocab vocab2({
            {"kanji", "漢字"},
            {"hiragana", "ひらがな"},
            {"romaji", "romaji"},
            {"english", std::vector<std::string>{"hiragana"}},  // Wrap the string in a vector
            {"difficulty", 0.8}
        });
        Vocab vocab3({
            {"kanji", "Test"},
            {"hiragana", "てすと"},
            {"romaji", "tesuto"},
            {"english", std::vector<std::string>{"test"}},  // Wrap the string in a vector
            {"difficulty", 0.5}
        });
        quiz.addVocab(vocab1);
        quiz.addVocab(vocab2);
        quiz.addVocab(vocab3);
        } catch (const std::exception& ex) {
            std::cerr << "Exception occurred: " << ex.what() << std::endl;
        }
    }

};

// Define the forward declared functions outside the QuizTestInternal namespace
bool checkAnswer(Quiz& quiz, const std::string& userAnswer, const std::string& correctAnswer) {
    // Invoke the protected member function indirectly
    return QuizTestInternal::checkAnswer(quiz, userAnswer, correctAnswer);
}

std::string trim(Quiz& quiz, const std::string& str, const char& trimChar) {
    // Invoke the protected member function indirectly
    return QuizTestInternal::trim(quiz, str, trimChar);
}

std::string toLowercaseAndTrim(Quiz& quiz, const std::string& str) {
    // Invoke the private member function indirectly
    return QuizTestInternal::toLowercaseAndTrim(quiz, str);
}

TEST_F(QuizTest, CheckLoadQuiz) {
    std::string quizDataFile = "quiz_data.json";
    bool loadResult = quiz.loadQuiz(quizDataFile);
    EXPECT_TRUE(loadResult) << "Failed to load quiz data from file: " << quizDataFile;
}

TEST_F(QuizTest, CheckCheckAnswer) {
    // Check answer checking function
    std::string userAnswer = "helloworld";
    std::string correctAnswer = "helloworld";
    bool result = checkAnswer(quiz, userAnswer, correctAnswer);
    EXPECT_TRUE(result);

    userAnswer = "helloworld";
    correctAnswer = "hello";
    result = checkAnswer(quiz, userAnswer, correctAnswer);
    EXPECT_FALSE(result);
}

TEST_F(QuizTest, CheckTrim) {
    // Check the trim function
    std::string str = "   hello world   ";
    std::string expected = "hello world";
    std::string result = trim(quiz, str);
    EXPECT_EQ(result, expected);
}

TEST_F(QuizTest, CheckToLowerCaseAndTrim) {
    // Check the lowercase and trim function
    std::string str = "   HeLLo WoRLD   ";
    std::string expected = "hello world";
    std::string result = toLowercaseAndTrim(quiz, str);
    EXPECT_EQ(result, expected);
}

TEST_F(QuizTest, CheckValidateAnswer) {
    // Check the validate answer function
    std::string answer = "    ";
    bool result = quiz.validateAnswer(answer);
    EXPECT_FALSE(result);

    answer = "answer";
    result = quiz.validateAnswer(answer);
    EXPECT_TRUE(result);
}

TEST_F(QuizTest, ValidateAnswer) {
    // Valid answers
    EXPECT_TRUE(quiz.validateAnswer("test"));
    EXPECT_TRUE(quiz.validateAnswer("  test  "));
    EXPECT_TRUE(quiz.validateAnswer("漢字"));
    EXPECT_TRUE(quiz.validateAnswer("てすと"));

    // Answers with leading/trailing whitespace
    EXPECT_FALSE(quiz.validateAnswer("  test"));  // Expected: false
    EXPECT_FALSE(quiz.validateAnswer("test  "));  // Expected: false
    EXPECT_FALSE(quiz.validateAnswer(" 漢字"));   // Expected: false
    EXPECT_FALSE(quiz.validateAnswer("てすと "));  // Expected: false

    // Answers with invalid characters
    EXPECT_FALSE(quiz.validateAnswer("test!"));  // Expected: false
    EXPECT_FALSE(quiz.validateAnswer("漢字@"));  // Expected: false
    EXPECT_FALSE(quiz.validateAnswer("てすと#"));  // Expected: false

    // Answers containing multiple hyphens
    EXPECT_FALSE(quiz.validateAnswer("te--st"));  // Expected: false
    EXPECT_FALSE(quiz.validateAnswer("漢-字"));   // Expected: false
    EXPECT_FALSE(quiz.validateAnswer("て-すと"));  // Expected: false
}

TEST_F(QuizTest, ContainsInvalidCharacters) {
    // Valid input without invalid characters
    EXPECT_FALSE(quiz.containsInvalidCharacters("test"));
    EXPECT_FALSE(quiz.containsInvalidCharacters("漢字"));
    EXPECT_FALSE(quiz.containsInvalidCharacters("てすと"));

    // Invalid characters present
    EXPECT_TRUE(quiz.containsInvalidCharacters("test!"));
    EXPECT_TRUE(quiz.containsInvalidCharacters("漢字@"));
    EXPECT_TRUE(quiz.containsInvalidCharacters("てすと#"));
}

TEST_F(QuizTest, ContainsWhitespace) {
    // Valid input without whitespace
    EXPECT_FALSE(quiz.containsWhitespace("test"));
    EXPECT_FALSE(quiz.containsWhitespace("漢字"));
    EXPECT_FALSE(quiz.containsWhitespace("てすと"));

    // Whitespace present
    EXPECT_TRUE(quiz.containsWhitespace("test "));
    EXPECT_TRUE(quiz.containsWhitespace("漢字 "));
    EXPECT_TRUE(quiz.containsWhitespace("てすと "));
}

TEST_F(QuizTest, HasLeadingOrTrailingWhitespace) {
    // Valid input without leading/trailing whitespace
    EXPECT_FALSE(quiz.hasLeadingOrTrailingWhitespace("test"));
    EXPECT_FALSE(quiz.hasLeadingOrTrailingWhitespace("漢字"));
    EXPECT_FALSE(quiz.hasLeadingOrTrailingWhitespace("てすと"));

    // Leading/trailing whitespace present
    EXPECT_TRUE(quiz.hasLeadingOrTrailingWhitespace(" test"));
    EXPECT_TRUE(quiz.hasLeadingOrTrailingWhitespace("漢字 "));
    EXPECT_TRUE(quiz.hasLeadingOrTrailingWhitespace(" てすと "));
}

TEST_F(QuizTest, LoadQuiz) {
    EXPECT_TRUE(quiz.loadQuiz("quiz_data.json"));  // Provide a valid quiz data file
    EXPECT_FALSE(quiz.loadQuiz("invalid_file.json"));  // Provide an invalid/non-existent quiz data file
    // Additional assertions to verify the loaded quiz data
}

// ... Add more tests for the remaining functions
