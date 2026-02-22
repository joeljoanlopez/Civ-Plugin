cmake_minimum_required(VERSION 3.14)
project(MapGenerationCore)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(MapGenCore SHARED
        include/core/CoreAPI.h
        include/hex/HexCoord.h
        src/hex/HexCoord.cpp
        include/hex/HexTile.h
        src/hex/HexTile.cpp
        include/hex/HexGrid.h
        src/hex/HexGrid.cpp
        include/generation/RandomGenerator.h
        src/generation/RandomGenerator.cpp
        include/generation/RandomGenerator.h
        src/generation/TectonicsGenerator.cpp
        include/generation/TectonicsGenerator.h
        include/generation/PerlinNoiseGenerator.h
        src/generation/PerlinNoiseGenerator.cpp
)
target_include_directories(MapGenCore PUBLIC include)
target_compile_definitions(MapGenCore PRIVATE MAPGEN_EXPORTS)

enable_testing()
add_executable(UnitTests tests/hex/HexGridTests.cpp
        tests/hex/HexCoordTests.cpp
        tests/hex/HexTileTests.cpp
        tests/generation/RandomGeneratorTests.cpp
        tests/generation/TectonicsGeneratorTests.cpp
        tests/generation/PerlinNoiseGeneratorTests.cpp)
target_link_libraries(UnitTests PRIVATE MapGenCore gtest_main)