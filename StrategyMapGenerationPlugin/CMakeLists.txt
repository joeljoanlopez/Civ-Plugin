cmake_minimum_required(VERSION 3.14)
project(MapGenerationCore)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

add_library(MapGenCore SHARED
        include/CoreAPI.h
        include/HexCoord.h
        src/HexCoord.cpp
        include/HexTile.h
        src/HexTile.cpp
        include/HexGrid.h
        src/HexGrid.cpp
        include/RandomGenerator.h
        src/RandomGenerator.cpp
        include/RandomGenerator.h
        src/TectonicsGenerator.cpp
        include/TectonicsGenerator.h
        include/PerlinNoiseGenerator.h
        src/PerlinNoiseGenerator.cpp
)
target_include_directories(MapGenCore PUBLIC include)
target_compile_definitions(MapGenCore PRIVATE MAPGEN_EXPORTS)

enable_testing()
add_executable(UnitTests tests/HexGridTests.cpp
        tests/HexCoordTests.cpp
        tests/HexTileTests.cpp
        tests/RandomGeneratorTests.cpp
        tests/TectonicsGeneratorTests.cpp
        tests/PerlinNoiseGeneratorTests.cpp)
target_link_libraries(UnitTests PRIVATE MapGenCore gtest_main)