# Japanese_Tutor

Builds:

make:                                   Build everything
make main:                              Build main.cpp
make test:                              Build and test vocab_quiz.cpp test_text_to_speech
make utility-test:                      Build and test utility text_to_speech.cpp
make u-test-args ARGS="こにちはAIです"   Build and test test_text_to_speech.cpp

 Terminal Compile:   
 unit_test_ebisu, unit_test_quiz, unit_test_vocab

 g++ -o unit_test_vocap unit_test_vocap.cpp -lgtest -lgtest_main -pthread -Iinclude

TODO: 
    Vocabulary Database: Maintain a database or file containing the Japanese vocabulary words, their meanings, pronunciation, and other relevant information. You can load this data into your program to provide word quizzes or learning exercises.

    Quiz Modes: 
        -Implement different quiz modes to cater to various learning needs. 
        -Multiple-choice quizzes, 
        -fill-in-the-blank quizzes,
        -listening comprehension exercises.

    Scoring and Progress Tracking: 
        Keep track of users' scores and progress. You can store scores for each quiz or exercise attempted by the user and calculate an overall score or performance summary. This allows users to track their progress and identify areas for improvement.

    Study Materials: 
        Provide study materials such as grammar rules, verb conjugation tables, kanji charts, or example sentences. These materials can be presented alongside quizzes to help users learn and reinforce their understanding.

    Speech Recognition: 
        Implement speech recognition functionality to enable users to practice speaking Japanese. The program can listen to the user's speech and provide feedback or evaluate pronunciation accuracy.

    Lesson Plan or Curriculum: 
        Design a structured lesson plan or curriculum that guides users through various learning stages. This can include organizing vocabulary topics, grammar lessons, and progressively challenging exercises to ensure a well-rounded learning experience.

    User Profiles: 
        Allow users to create individual profiles with personal information, learning goals, and preferences. This enables personalized learning experiences and tailored content suggestions.

    Progress Reports: 
        Generate progress reports or summaries periodically to provide users with an overview of their achievements, areas of improvement, and recommendations for further study.


    /*
    // Function declarations for quiz modes
    void askMultipleChoiceQuestion(const Vocab& vocab);
    void processMultipleChoiceAnswer(const Vocab& vocab, const std::string& userAnswer);
    void askFillInTheBlankQuestion(const Vocab& vocab);
    void processFillInTheBlankAnswer(const Vocab& vocab, const std::string& userAnswer);
    void askListeningComprehensionQuestion(const Vocab& vocab);
    void processListeningComprehensionAnswer(const Vocab& vocab, const std::string& userAnswer);
    */

/*
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
*/