cmake_minimum_required(VERSION 3.30)

project(TestKLib VERSION 1.0.0 LANGUAGES CXX)

# Debug Version may contain runtime checks and experimental features/implementations
# set(MM_IS_DEBUG true)



set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add_executable(TestKLib main2.cpp)



target_include_directories(TestKLib PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
