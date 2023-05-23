# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# Libraries
LIBS = -lSDL2 -lSDL2_mixer -lstdc++ -lcurl

# Source and object files
SRC = $(wildcard *.cpp utility/*.cpp)
OBJ = $(SRC:.cpp=.o)

# Executable name
EXECUTABLE = vocab_quiz
UTILITY_EXECUTABLE = utility/vocab_quiz

.PHONY: all clean test

all: $(EXECUTABLE) $(UTILITY_EXECUTABLE)

$(EXECUTABLE): $(filter-out utility/%, $(OBJ))
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(UTILITY_EXECUTABLE): $(filter utility/%, $(OBJ))
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: all
	./$(EXECUTABLE) japanese_101.json
	./$(UTILITY_EXECUTABLE) japanese_101.json

clean:
	$(RM) $(OBJ) $(EXECUTABLE) $(UTILITY_EXECUTABLE) audio.wav
