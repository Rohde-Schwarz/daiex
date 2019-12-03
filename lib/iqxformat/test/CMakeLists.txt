file(GLOB_RECURSE IQXFORMAT_TEST_SOURCES *.h *.cpp *.hpp)
add_executable(${PROJECT_NAME}_test ${IQXFORMAT_TEST_SOURCES})
target_include_directories(${PROJECT_NAME}_test PRIVATE ${GTEST_ROOT}/include)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME} gtest)

add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)
