cmake_minimum_required(VERSION 3.20)

project(Engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
    src/Game/*.cpp
    src/Logger/*.cpp
    src/ECS/*.cpp
)

add_executable(Engine ${ENGINE_SOURCES})

# Local include paths
target_include_directories(Engine PRIVATE
    libs
    libs/sol
)

# Compiler warnings
if(MSVC)
    target_compile_options(Engine PRIVATE
        /W4
        /WX
    )
else()
    target_compile_options(Engine PRIVATE
        -Wall
        -Wfatal-errors
    )
endif()

# Platform-specific SDL/Lua configuration
if(WIN32)
    target_include_directories(Engine PRIVATE
        C:/msys64/mingw64/include
        C:/msys64/mingw64/include/SDL2
    )

    target_link_directories(Engine PRIVATE
        C:/msys64/mingw64/lib
    )

    target_link_libraries(Engine PRIVATE
        mingw32
        SDL2main
        SDL2
        SDL2_image
        SDL2_ttf
        SDL2_mixer
        lua
    )

else()
    target_include_directories(Engine PRIVATE
        /opt/homebrew/include
    )

    target_link_directories(Engine PRIVATE
        /opt/homebrew/lib
    )

    target_link_libraries(Engine PRIVATE
        SDL2
        SDL2_image
        SDL2_ttf
        SDL2_mixer
        lua
    )
endif()