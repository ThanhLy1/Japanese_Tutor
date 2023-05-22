# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# SDL libraries
LIBS = -lSDL2 -lSDL2_mixer

# Source files
SRC = $(wildcard *.cpp)
UTILITY_SRC = $(wildcard utility/*.cpp)

# Object files
OBJ = $(SRC:.cpp=.o)
UTILITY_OBJ = $(UTILITY_SRC:.cpp=.o)

# Executable name
EXECUTABLE = vocab_quiz
UTILITY_EXECUTABLE = utility/vocab_quiz

.PHONY: all clean test

all: $(EXECUTABLE) $(UTILITY_EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(UTILITY_EXECUTABLE): $(UTILITY_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(EXECUTABLE) $(UTILITY_EXECUTABLE)
	./$(EXECUTABLE) japanese_101.json
	./$(UTILITY_EXECUTABLE) japanese_101.json

clean:
	$(RM) $(OBJ) $(EXECUTABLE) $(UTILITY_OBJ) $(UTILITY_EXECUTABLE) audio.wav
