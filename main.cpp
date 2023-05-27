#include "quiz_logic/quiz.h"

int main() {
    Quiz myQuiz;
    myQuiz.loadQuiz("quiz_data.json");
    myQuiz.loadQuizState();  // Load the quiz state before starting the quiz
    myQuiz.startQuiz();
    myQuiz.printStatistics();
    myQuiz.saveQuizState();  // Save the quiz state after finishing the quiz

    return 0;
}
