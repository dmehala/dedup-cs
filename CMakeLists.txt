cmake_minimum_required(VERSION 3.14)
project(dedup)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES src/csv.cpp src/helper.cpp)
set(TEST_SOURCES tests/helper_test.cpp tests/csv_test.cpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(include)

add_executable(dedup ${SOURCES} src/main.cpp)

target_include_directories(dedup PRIVATE include)

add_executable(dedup_tests ${SOURCES} ${TEST_SOURCES})

target_link_libraries(
  dedup_tests
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(dedup_tests)