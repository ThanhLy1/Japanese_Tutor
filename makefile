# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# Source files
SOURCES = vocab_quiz.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE = vocab_quiz

.PHONY: all clean test

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(EXECUTABLE)
	./$(EXECUTABLE) japanese_101.json

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)