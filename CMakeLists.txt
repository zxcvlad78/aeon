cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME Aeon)
project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window System Audio)
find_package(EnTT CONFIG REQUIRED)

find_path(TERMCOLOR_INCLUDE_DIRS "termcolor/termcolor.hpp")


file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include ${TERMCOLOR_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    EnTT::EnTT
)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
    COMMENT "Resources copied successfully to build directory."
)