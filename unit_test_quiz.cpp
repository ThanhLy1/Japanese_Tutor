#include "quiz_logic/quiz.h"
#include "gtest/gtest.h"

class QuizTest : public ::testing::Test {
protected:
    Quiz quiz;

    void SetUp() override {
        // Load quiz data
        bool loadResult = quiz.loadQuiz("quiz_data.json");
        ASSERT_TRUE(loadResult);
    }
};

TEST_F(QuizTest, CheckLoadQuiz) {
    // Load quiz data from an invalid file
    bool loadResult = quiz.loadQuiz("invalid_file.json");
    EXPECT_FALSE(loadResult);
}

TEST_F(QuizTest, CheckGetRandomVocab) {
    // Check the function does not return an empty vocab
    Vocab vocab = quiz.getRandomVocab();
    EXPECT_FALSE(vocab.getKanji().empty());
}

TEST_F(QuizTest, CheckCheckAnswer) {
    // Check answer checking function
    EXPECT_TRUE(quiz.checkAnswer("helloworld", "helloworld"));
    EXPECT_FALSE(quiz.checkAnswer("helloworld", "hello"));
}

TEST_F(QuizTest, CheckTrim) {
    // Check the trim function
    EXPECT_EQ(quiz.trim("   hello world   "), "hello world");
}

TEST_F(QuizTest, CheckToLowerCaseAndTrim) {
    // Check the lowercase and trim function
    EXPECT_EQ(quiz.toLowercaseAndTrim("   HeLLo WoRLD   "), "hello world");
}

TEST_F(QuizTest, CheckValidateAnswer) {
    // Check the validate answer function
    EXPECT_FALSE(quiz.validateAnswer("    "));
    EXPECT_TRUE(quiz.validateAnswer("answer"));
}
/*
TEST_F(QuizTest, CheckShuffleVocabList) {
    // Compare shuffled list to original. 
    // In real life, this test can sometimes give false positives because 
    // a shuffled list can theoretically be the same as the original. 
    // To mitigate this, you can check it with larger data.
    std::vector<Vocab> originalVocabList = quiz.vocabList;
    quiz.shuffleVocabList();
    EXPECT_NE(originalVocabList, quiz.vocabList);
}

TEST_F(QuizTest, CheckGetVocabListSize) {
    // Compare the vocab list size with a known value. 
    // In real life, this value could change, so you should set up your test 
    // to have a consistent list size.
    int expectedSize = 100;
    EXPECT_EQ(quiz.getVocabListSize(), expectedSize);
}

TEST_F(QuizTest, CheckStartQuiz) {
    // Check if the quiz starts correctly.
    // In real life, this might check if the first question is displayed properly.
    EXPECT_TRUE(quiz.startQuiz());
}

TEST_F(QuizTest, CheckEndQuiz) {
    // Check if the quiz ends correctly.
    // In real life, this might check if the results are displayed correctly.
    EXPECT_TRUE(quiz.endQuiz());
}

TEST_F(QuizTest, CheckGetCurrentQuestion) {
    // Check if the current question is retrieved correctly.
    // In real life, this might compare the returned question with the known current question.
    EXPECT_EQ(quiz.getCurrentQuestion(), "What is the capital of France?");
}

TEST_F(QuizTest, CheckGetScore) {
    // Check if the score is retrieved correctly.
    // In real life, this might compare the returned score with a known value.
    int expectedScore = 0;
    EXPECT_EQ(quiz.getScore(), expectedScore);
}

TEST_F(QuizTest, CheckResetQuiz) {
    // Check if the quiz resets correctly.
    // In real life, this might check if the score, current question, etc. are reset.
    quiz.resetQuiz();
    EXPECT_EQ(quiz.getScore(), 0);
    EXPECT_EQ(quiz.getCurrentQuestion(), "");
}
*/

// ... Add more tests for the remaining functions
