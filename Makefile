# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wfatal-errors 
LANG_STD = -std=c++17 
INCLUDE_PATH = -Ilibs -Ilibs/sol
SRC = $(wildcard src/*.cpp src/Game/*.cpp src/Logger/*.cpp)
TARGET = Engine

# OS detection
ifeq ($(OS), Windows_NT)
    TARGET_BIN = $(TARGET).exe
    RM         = rm -f
    RUN_CMD    = ./$(TARGET_BIN)
    LIBS       = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image \
                 -lSDL2_ttf -lSDL2_mixer -llua
    CXXFLAGS += -IC:/msys64/mingw64/include \
            -IC:/msys64/mingw64/include/SDL2
else
    TARGET_BIN = $(TARGET)
    RM         = rm -f
    RUN_CMD    = ./$(TARGET_BIN)
    LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3
    CXXFLAGS += -I/opt/homebrew/include -L/opt/homebrew/lib
endif

# Build rule
build: $(SRC)
	$(CXX) $(CXXFLAGS) $(LANG_STD) $(INCLUDE_PATH) $(SRC) $(LIBS) -o $(TARGET_BIN)

# Run rule
run: build
	$(RUN_CMD)

# Clean rule
clean:
	$(RM) $(TARGET_BIN)