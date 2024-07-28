enable_testing()

# translation-test ============================================================

add_executable(
  translation-test
  translation-test.cc
  ${CMAKE_SOURCE_DIR}/../src/vocabulary/translation.cc

  ${CMAKE_SOURCE_DIR}/../src/tools/string_utils.cc
)

target_link_libraries(
  translation-test
  GTest::gtest_main
)

target_include_directories(
  translation-test PRIVATE
  ${CMAKE_SOURCE_DIR}/../src
  ${CMAKE_SOURCE_DIR}/../spdlog/include
)

# word-test ===================================================================

add_executable(
  word-test
  word-test.cc
  ${CMAKE_SOURCE_DIR}/../src/vocabulary/word.cc

  ${CMAKE_SOURCE_DIR}/../src/tools/string_utils.cc
  ${CMAKE_SOURCE_DIR}/../src/vocabulary/translation.cc
)

target_link_libraries(
  word-test
  GTest::gtest_main
)

target_include_directories(
  word-test PRIVATE
  ${CMAKE_SOURCE_DIR}/../src
  ${CMAKE_SOURCE_DIR}/../spdlog/include
)

# vocabulary-test =============================================================

add_executable(
  vocabulary-test
  vocabulary-test.cc
  ${CMAKE_SOURCE_DIR}/../src/vocabulary/vocabulary.cc

  ${CMAKE_SOURCE_DIR}/../src/tools/string_utils.cc
  ${CMAKE_SOURCE_DIR}/../src/vocabulary/translation.cc
  ${CMAKE_SOURCE_DIR}/../src/vocabulary/word.cc
)

target_link_libraries(
  vocabulary-test
  GTest::gtest_main
  fmt
)

target_include_directories(
  vocabulary-test PRIVATE
  ${CMAKE_SOURCE_DIR}/../src
  ${CMAKE_SOURCE_DIR}/../spdlog/include
)

# =========================================================

include(GoogleTest)

gtest_discover_tests(translation-test)
