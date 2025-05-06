cmake_minimum_required(VERSION 3.10)
project(csvreader C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# 1) Общая библиотека из всей логики парсера
file(GLOB LIB_SOURCES "src/*.c")
add_library(csv_lib STATIC ${LIB_SOURCES})

# 2) Исполняемый файл csvreader
add_executable(csvreader src/main.c)
target_link_libraries(csvreader PRIVATE csv_lib)

# Чтобы бинарник оказался в корне проекта:
#set_target_properties(csvreader PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
#)

# 3) Тесты на Google Test
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Исполняемый файл с тестами
add_executable(csvreader_test tests/csvreader_gtest.cpp)
target_link_libraries(csvreader_test
    PRIVATE
      csv_lib
      GTest::GTest
      GTest::Main
)

# Регистрируем тестовую цель
add_test(NAME CsvReaderTest COMMAND csvreader_test)

# 4) Полная очистка build/
add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Удаление каталога сборки"
)
