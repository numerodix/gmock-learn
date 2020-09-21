cmake_minimum_required(VERSION 3.0)

project(gmock-learn)
 
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
add_executable(assert_test assert_test.cc)
target_link_libraries(assert_test ${GTEST_LIBRARIES} pthread)

add_executable(mock_unconfigured_test mock_unconfigured_test.cc)
target_link_libraries(mock_unconfigured_test ${GTEST_LIBRARIES} gmock pthread)

add_executable(mock_dyi_test mock_dyi_test.cc)
target_link_libraries(mock_dyi_test ${GTEST_LIBRARIES} gmock pthread)

add_executable(parameterize_test parameterize_test.cc)
target_link_libraries(parameterize_test ${GTEST_LIBRARIES} pthread)
