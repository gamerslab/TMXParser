cmake_minimum_required (VERSION 2.6)
project(TMXParser)

file(GLOB TMXPARSER_SOURCES src/*.cpp)
set(TMXPARSER_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "DIRS")

include_directories(
    include
)

add_library(TMXParser ${TMXPARSER_SOURCES})
