#include "gtest/gtest.h"
#include "quiz_logic/vocab.h"
#include <fstream>

class VocabTest : public ::testing::Test {
protected:
    Vocab vocab;

    // You can do set-up work for each test here.
    VocabTest() {
        std::vector<std::string> english = {"hello", "world"};
        vocab.setKanji("kanji");
        vocab.setHiragana("hiragana");
        vocab.setRomaji("romaji");
        vocab.setEnglish(english);
        vocab.setPartOfSpeech("noun");
        vocab.setDialogue("dialogue");
        vocab.setLesson("lesson");
        vocab.setDifficulty(1.0);
    }
};

TEST_F(VocabTest, CheckKanji) {
    EXPECT_EQ(vocab.getKanji(), "kanji");
}

TEST_F(VocabTest, CheckHiragana) {
    EXPECT_EQ(vocab.getHiragana(), "hiragana");
}

TEST_F(VocabTest, CheckRomaji) {
    EXPECT_EQ(vocab.getRomaji(), "romaji");
}

TEST_F(VocabTest, CheckEnglish) {
    std::vector<std::string> english = {"hello", "world"};
    EXPECT_EQ(vocab.getEnglish(), english);
}

TEST_F(VocabTest, CheckPartOfSpeech) {
    EXPECT_EQ(vocab.getPartOfSpeech(), "noun");
}

TEST_F(VocabTest, CheckDialogue) {
    EXPECT_EQ(vocab.getDialogue(), "dialogue");
}

TEST_F(VocabTest, CheckLesson) {
    EXPECT_EQ(vocab.getLesson(), "lesson");
}

TEST_F(VocabTest, CheckDifficulty) {
    EXPECT_EQ(vocab.getDifficulty(), 1.0);
}

TEST_F(VocabTest, CheckCorrectAnswer) {
    EXPECT_EQ(vocab.correctAnswer(), "hello, world");
}

TEST_F(VocabTest, JsonConversion) {
    nlohmann::json expected = {
        {"kanji", "kanji"},
        {"hiragana", "hiragana"},
        {"romaji", "romaji"},
        {"english", std::vector<std::string>{"hello", "world"}},
        {"part_of_speech", "noun"},
        {"dialogue", "dialogue"},
        {"lesson", "lesson"},
        {"difficulty", 1.0}
    };

    EXPECT_EQ(vocab.toJson(), expected);
}

TEST_F(VocabTest, LoadSaveFromJsonFile) {
    // Save the vocab instance to a file
    vocab.saveToPersistenceFile("test.json");

    // Load it into a new instance
    Vocab newVocab;
    newVocab.loadFromJsonFile("test.json");

    // Compare the instances
    EXPECT_EQ(newVocab.getKanji(), vocab.getKanji());
    EXPECT_EQ(newVocab.getHiragana(), vocab.getHiragana());
    EXPECT_EQ(newVocab.getRomaji(), vocab.getRomaji());
    EXPECT_EQ(newVocab.getEnglish(), vocab.getEnglish());
    EXPECT_EQ(newVocab.getPartOfSpeech(), vocab.getPartOfSpeech());
    EXPECT_EQ(newVocab.getDialogue(), vocab.getDialogue());
    EXPECT_EQ(newVocab.getLesson(), vocab.getLesson());
    EXPECT_EQ(newVocab.getDifficulty(), vocab.getDifficulty());

    // Clean up the test file
    std::remove("test.json");
}

// add more tests for the other methods
