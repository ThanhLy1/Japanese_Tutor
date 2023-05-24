# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -I./include

# Libraries
LIBS = -lSDL2 -lSDL2_mixer -lstdc++ -lcurl

# Source and object files for vocab_quiz
VOCAB_SRC = vocab_quiz.cpp
VOCAB_OBJ = $(VOCAB_SRC:.cpp=.o)
VOCAB_EXECUTABLE = vocab_quiz

# Source and object files for test_text_to_speech
UTILITY_SRC = utility/test_text_to_speech.cpp utility/tts.cpp
UTILITY_OBJ = $(UTILITY_SRC:.cpp=.o)
UTILITY_EXECUTABLE = test_text_to_speech

.PHONY: all clean-vocab clean-utility

all: $(VOCAB_EXECUTABLE) $(UTILITY_EXECUTABLE)

$(VOCAB_EXECUTABLE): $(VOCAB_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(UTILITY_EXECUTABLE): $(UTILITY_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(VOCAB_EXECUTABLE)
	./$(VOCAB_EXECUTABLE) japanese_101.json

utility-test: $(UTILITY_EXECUTABLE)
	./$(UTILITY_EXECUTABLE) "こにちはAIです"

# Usage: make u-test-args ARGS="my text here"
u-test-args: $(UTILITY_EXECUTABLE)
	./$(UTILITY_EXECUTABLE) "$(ARGS)"

clean-vocab:
	$(RM) $(VOCAB_OBJ) $(VOCAB_EXECUTABLE) audio.wav

clean-utility:
	$(RM) $(UTILITY_OBJ) $(UTILITY_EXECUTABLE) test_audio.wav

clean: clean-vocab clean-utility
