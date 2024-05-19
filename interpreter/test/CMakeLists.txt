enable_testing()

add_executable(InterpreterTests
  InterpreterTests.cpp
)

target_include_directories(InterpreterTests PUBLIC ../src)

target_link_libraries(InterpreterTests
  GTest::gtest_main
  spdlog::spdlog
  spdlog
)

include(GoogleTest)
gtest_discover_tests(InterpreterTests)
