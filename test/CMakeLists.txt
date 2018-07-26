find_package(GTest REQUIRED)

include_directories(
  ${GTEST_INCLUDE_DIRS}
  ${DANA_INCLUDE_DIRS}
)

file(GLOB_RECURSE CPP_FILES *.cpp)

add_executable(tests ${CPP_FILES})

target_link_libraries(tests
  ${GTEST_LIBRARIES}
  pthread
  dana
)