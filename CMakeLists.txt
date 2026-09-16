cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Project VERSION 1.0)

# speichert alle cpp-Dateien in der Variable SRC_FILES um die dann in add_executable() einfach und dynamisch anzugeben
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# Legt Namen der Binary auf "hello" fest und berücksichtigt die Source-Dateien
add_executable(hello src/main.cpp ${SRC_FILES})

# Teilt CMake mit, wo die header-Files sind (hier in include/)
target_include_directories(hello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
