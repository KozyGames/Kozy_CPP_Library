cmake_minimum_required(VERSION 3.21)

project(KozyLib VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)


set(buildFlag_complete_compileTest false)
set(buildFlag_compileTimeString_Test true)

add_custom_target(KozyLib)

if(buildFlag_complete_compileTest)
    #add_custom_target(KozyLib)
    
    add_executable(complete_compileTest 
    test/compileTest.cpp
    )

    target_include_directories(complete_compileTest PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
    )

endif()

if(buildFlag_compileTimeString_Test)
    #add_custom_target(KozyLib)

    add_executable(compileTimeString_Test 
    test/DataStructures/compileTime_String_Test.cpp
    )

    #add_dependencies()

    target_include_directories(compileTimeString_Test PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
    )

endif()