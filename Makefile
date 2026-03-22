# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -Ilibs -Ilibs/sol

# Source settings
SRC = $(wildcard src/*.cpp)
TARGET = Engine

# OS detection
ifeq ($(OS), Windows_NT)
    TARGET_BIN = $(TARGET).exe
    RM         = del /f
    RUN_CMD    = ./$(TARGET_BIN)
    LIBS       = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image \
                 -lSDL2_ttf -lSDL2_mixer -llua
    CXXFLAGS += -IC:/msys64/mingw64/include \
            -IC:/msys64/mingw64/include/SDL2
else
    TARGET_BIN = $(TARGET)
    RM         = rm -f
    RUN_CMD    = ./$(TARGET_BIN)
    LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.4
    CXXFLAGS += -I/opt/homebrew/include -L/opt/homebrew/lib
endif

# Build rule
build: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LIBS) -o $(TARGET_BIN)

# Run rule
run: build
	$(RUN_CMD)

# Clean rule
clean:
	$(RM) $(TARGET_BIN)