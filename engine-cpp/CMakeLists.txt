cmake_minimum_required(VERSION 3.10)

# Project ka naam
project(algosBehindAlgo_engine)

# C++20 use karne ke liye standard set kar rahe hain
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include folder batao taaki httplib.h aur Tokenizer.h mil jayein
include_directories(include)

# Executable file banao 'engine_server' naam ki, jisme main.cpp aur Tokenizer.cpp use honge
add_executable(engine_server src/main.cpp src/Tokenizer.cpp src/CallGraph.cpp)

# Linux/Mac par multithreading ke liye pthread link karna padta hai (httplib ke liye zaroori hai)
find_package(Threads REQUIRED)
target_link_libraries(engine_server Threads::Threads)