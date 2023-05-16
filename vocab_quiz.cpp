#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <cstdlib> // For system("clear") on Unix-like systems
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class VocabularyQuiz {
public:
    void loadVocabulary(const std::string& filename) {
        std::ifstream file(filename);
        file >> data_;
        file.close();
    }

    void startQuiz() {
        json& vocabulary = data_["vocabulary"];
        std::shuffle(vocabulary.begin(), vocabulary.end(), std::mt19937(std::random_device()()));

        int correctCount = 0;
        int totalCount = 0;
        int score = 0;

        for (json& word : vocabulary) {
            std::string question = word["word"];
            std::string answer = word["meaning"];

            clearConsole();
            std::cout << "Question: " << question << std::endl;
            std::string userAnswer;
            std::cout << "Your answer (type 'q' to quit): ";
            std::cin >> userAnswer;

            if (userAnswer == "q") {
                std::cout << "Quitting the quiz." << std::endl;
                break;  // Exit the for loop
            }

            totalCount++;

            if (userAnswer == answer) {
                std::cout << "Correct!" << std::endl;
                word["recall_level"] = word["recall_level"].get<int>() + 1;
                correctCount++;
                score += 10; // Increase score for correct answer
            } else {
                std::cout << "Incorrect!" << std::endl;
                std::cout << "The correct answer is: " << answer << std::endl;
                word["recall_level"] = word["recall_level"].get<int>() - 1;
                score -= 5; // Decrease score for incorrect answer
            }
        }

        clearConsole();
        std::cout << "Quiz completed!" << std::endl;
        std::cout << "Total Questions: " << totalCount << std::endl;
        std::cout << "Correct Answers: " << correctCount << std::endl;
        std::cout << "Incorrect Answers: " << totalCount - correctCount << std::endl;
        std::cout << "Score: " << score << std::endl;

        saveVocabulary();
    }

private:
    void clearConsole() {
        // Clear console command for Unix-like systems
        // You may need to modify this for other platforms
        system("clear");
    }

    void saveVocabulary() {
        std::ofstream file("japanese_101.json");
        file << data_.dump(2);
        file.close();
    }

    json data_;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./vocab_quiz <filename.json>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    VocabularyQuiz quiz;
    quiz.loadVocabulary(filename);
    quiz.startQuiz();

    return 0;
}
