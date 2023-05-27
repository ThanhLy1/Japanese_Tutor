#include "quiz_logic/quiz.h"

int main() {
    /*
    // Load the data from the "quiz_data.json" file
    std::vector<Vocab> vocabList;
    Vocab vocab;
    vocab.loadFromJsonFile("quiz_data.json");

    // Access the loaded data using the getter methods
    vocab.printDetails();

    // Modify data using setter methods
    vocab.setDifficulty(2.5);
    vocab.setMeaning({"cooking", "cuisine"});
    vocab.printDetails();

    // Save the modified data to persistence file using kanji as ID
    vocab.saveToPersistenceFile("persistence_data.json");

    // Add the loaded vocab to the vocabList
    vocabList.push_back(vocab);

    // Dump the vocabList to JSON
    dumpToJsonFile(vocabList);

    // Print the JSON file contents
    printJsonFile("quiz_data.json");
    */

    Quiz myQuiz;
    myQuiz.loadQuiz("quiz_data.json");
    myQuiz.loadQuizState();  // Load the quiz state before starting the quiz
    myQuiz.startQuiz();
    myQuiz.printStatistics();
    myQuiz.saveQuizState();  // Save the quiz state after finishing the quiz

    myQuiz.runUnitTests();
    return 0;
}
