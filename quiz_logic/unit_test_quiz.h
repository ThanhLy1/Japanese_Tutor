#ifndef UNIT_TEST_QUIZ_H
#define UNIT_TEST_QUIZ_H

#include "quiz.h" // Include the quiz.h header file

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

void Quiz::testValidateAnswer()
{
    std::cout << "Running unit test for validateAnswer()" << std::endl;

    std::vector<std::pair<std::string, bool>> tests = {
        {"",             false},
        {"   ",          false},
        {" test answer", true},
        {"test answer ", true},
        {"test@answer",  false},
        {"test.answer",  false},
        {"test-answer",  true},
        {"test answer",  true},
        {"こんにちは",    true},
        {"1234",         true},
        {"abcXYZ",       true}
    };

    for (const auto& test : tests)
    {
        std::cout << "Input: \"" << test.first << "\", Expected: " << test.second
            << ", Actual: " << validateAnswer(test.first) << "\n";
    }

    std::cout << "Unit test completed." << std::endl;
}

void Quiz::runUnitTests()
{
    testTrim();
    testContainsInvalidCharacters();
    testValidateAnswer();
}


#endif // UNIT_TEST_QUIZ_H