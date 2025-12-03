cmake_minimum_required(VERSION 3.10)

project(SumatoryCore)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    SumatoryCorePlugin.cpp
    SumatoryCorePlugin.h
    SumatoryWrapper.cpp
    SumatoryWrapper.h
)

add_library(SumatoryCore SHARED ${SOURCE_FILES})

target_compile_definitions(SumatoryCore PRIVATE SUMATORY_CORE_EXPORTS)