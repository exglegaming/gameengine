# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -Ilibs -Ilibs/sol

# Source and target settings
SRC = $(wildcard src/*.cpp)
TARGET = Engine

# Build rule
build: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) \
		-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3 -o $(TARGET)

# Run rule
run: build
	./$(TARGET)

# Clean rule
clean:
	rm -f $(TARGET)