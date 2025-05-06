cmake_minimum_required(VERSION 3.10)
project(csv_reader C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# Общая библиотека
file(GLOB LIB_SOURCES "src/*.c")
add_library(csv_lib STATIC ${LIB_SOURCES})

# Основная программа
add_executable(csv_reader src/main.c)
target_link_libraries(csv_reader PRIVATE csv_lib)

set_target_properties(csv_reader PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Google Test
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Тесты
add_executable(csvreader_test tests/csvreader_gtest.cpp)
target_link_libraries(csvreader_test PRIVATE csv_lib GTest::GTest GTest::Main)

add_test(NAME CsvReaderTest COMMAND csvreader_test)

# Очистка build
add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Удаление каталога сборки"
)
