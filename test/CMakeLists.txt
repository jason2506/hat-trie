FIND_PACKAGE (GTest REQUIRED)
FIND_PACKAGE (Threads REQUIRED)
INCLUDE_DIRECTORIES (${GTEST_INCLUDE_DIRS})

FILE (GLOB test_SRCS "*_test.cpp")

FOREACH(file ${test_SRCS})
    GET_FILENAME_COMPONENT (file_name ${file} NAME)
    STRING (REPLACE ".cpp" "" test_BIN ${file_name} )
    ADD_EXECUTABLE (${test_BIN} ${file})
    SET_PROPERTY (TARGET ${test_BIN} PROPERTY CXX_STANDARD 11)
    SET_PROPERTY (TARGET ${test_BIN} PROPERTY CXX_STANDARD_REQUIRED ON)
    TARGET_LINK_LIBRARIES (${test_BIN} ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    ADD_TEST (${test_BIN} ${test_BIN} --gtest_color=yes)
ENDFOREACH(file ${test_SRCS})
