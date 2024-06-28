cmake_minimum_required(VERSION 3.20)
PROJECT(txtz
	VERSION 0.9.2
	LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED HISTO_FILENAME1 AND NOT DEFINED HISTO_FILENAME2)
  # set(HISTO_FILENAME1 "data/de-nachnamen+histo.txt")
  set(HISTO_FILENAME2 "data/de-wiktionary-vornamen+histo.csv")
endif()

if(NOT DEFINED MAPBUILDING_ALGO)
  set(MAPBUILDING_ALGO "shannon-fano")
endif()

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -pedantic")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -pedantic -O2")
endif (UNIX)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /wd4365 /wd4710 /wd4711")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /Od")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /O2")
endif (MSVC)

include_directories(
	PRIVATE
	getopt-cpp/include
	json/include
	glob
	PUBLIC
)

add_custom_command(
	DEPENDS mapbuilder
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/mappings.cpp
    COMMAND ./mapbuilder --quiet --map-file "${CMAKE_CURRENT_SOURCE_DIR}/src/mappings" --input "${CMAKE_CURRENT_SOURCE_DIR}/${HISTO_FILENAME1}" --input "${CMAKE_CURRENT_SOURCE_DIR}/${HISTO_FILENAME2}"
)

add_custom_target(GenerateMap DEPENDS src/mappings.cpp)

if(MAPBUILDING_ALGO STREQUAL "huffman")
  add_definitions(-DALGO_HUFFMAN)
  set(ENCODER_CPP src/huffman.cpp)
elseif(MAPBUILDING_ALGO STREQUAL "shannon-fano")
  add_definitions(-DALGO_SHANNON_FANO)
  set(ENCODER_CPP src/shannon-fano.cpp)
else()
  message(FATAL_ERROR "Unknown map building algorithm: '${MAPBUILDING_ALGO}'")
endif()

add_executable(txtz
  src/txtz-main.cpp
  src/txtz.cpp
  src/mappings.cpp
  src/code.cpp
  src/util.cpp
  ${ENCODER_CPP}
)

add_executable(checker
  src/checker.cpp
  src/txtz.cpp
  src/mappings.cpp
  src/code.cpp
  src/util.cpp
  ${ENCODER_CPP}
)

add_dependencies(txtz GenerateMap)
add_dependencies(checker GenerateMap)

add_executable(mapbuilder
  src/mapbuilder.cpp
  src/code.cpp
  src/util.cpp
  glob/glob.cpp
  ${ENCODER_CPP}
)

if(UNIX AND CMAKE_BUILD_TYPE MATCHES Release)
  add_custom_command(TARGET txtz 
  POST_BUILD
  COMMAND strip $<TARGET_FILE:txtz>)
  add_custom_command(TARGET mapbuilder 
  POST_BUILD
  COMMAND strip $<TARGET_FILE:mapbuilder>)
  add_custom_command(TARGET checker 
  POST_BUILD
  COMMAND strip $<TARGET_FILE:checker>)
endif()

install(TARGETS txtz
  CONFIGURATIONS Release
  RUNTIME DESTINATION "$ENV{HOME}/bin")
