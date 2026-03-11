cmake_minimum_required(VERSION 3.15)

project(ASCIIImageConverter VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ascii_converter
    "main.cpp"
 "ArgsParser.cpp" "ArgsParser.h"
 "ImageLoader.cpp" "ImageLoader.h"
 "ASCIIConverter.cpp" "ASCIIConverter.h")

target_include_directories(ascii_converter PRIVATE
    ${PROJECT_SOURCE_DIR}/../../libs
)

set(DATA_DIR "${CMAKE_SOURCE_DIR}/../../data")
add_compile_definitions(DATA_DIR="${DATA_DIR}")

if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
