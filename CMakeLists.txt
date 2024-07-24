# AsyncInput
# https://github.com/KobaProduction/AsyncInput
# MIT License

cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

project(ASYNC_INPUT
        VERSION 0.1
        DESCRIPTION "Library for asynchronous control of buttons and incremental encoders for microcontrollers."
        LANGUAGES CXX
)

set(ASYNC_INPUT_SRCS
        src/utils.cpp
        src/button.cpp
        src/encoder.cpp
)

idf_component_register(
        SRC_DIRS
            src
        INCLUDE_DIRS
            src
        REQUIRES
            arduino
)

target_compile_options(${COMPONENT_LIB} PUBLIC -DEFINE_OPTION=1)

add_library(ASYNC_INPUT ASYNC_INPUT_SRCS)