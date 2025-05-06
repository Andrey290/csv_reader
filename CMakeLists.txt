cmake_minimum_required(VERSION 3.10)
project(csv_parser C)

set(CMAKE_C_STANDARD 11)
include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(csv_parser ${SOURCES})

set_target_properties(csv_parser PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

