cmake_minimum_required(VERSION 3.10)

project(MapGenerationPlugin)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    LineGeneratorPlugin.cpp
    LineGeneratorPlugin.h
    LineGenerator_Wrapper.cpp
    LineGenerator_Wrapper.h
)

add_library(MapGenerationPlugin SHARED ${SOURCE_FILES})

target_compile_definitions(MapGenerationPlugin PRIVATE MAPGENERATIONPLUGIN_EXPORTS)