include(FetchContent)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG 9b12f74)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_library(lib.gtest
            ${googletest_SOURCE_DIR}/googletest/src/gtest-all.cc)
    target_include_directories(lib.gtest
            PUBLIC ${googletest_SOURCE_DIR}/googletest/include/
            PRIVATE ${googletest_SOURCE_DIR}/googletest)
endif()

add_executable(unittests unitTest.cpp
    AlphabetTest.cpp
    OperatorTest.cpp
    RegexTest.cpp
    PrinterTest.cpp
    NfaTest.cpp
    StateTest.cpp)
target_link_libraries(unittests lib.gtest)
target_include_directories(unittests PRIVATE ../../source)