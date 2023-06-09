cmake_minimum_required(VERSION 3.20)
PROJECT(txtz
	VERSION 0.9.0
	LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

set(BOOST_ROOT $ENV{BOOST_ROOT})
find_package(Boost 1.71.0 REQUIRED COMPONENTS locale)
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost lib dirs: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost libs: ${Boost_LIBRARIES}")

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
	smaz
	PUBLIC
	${Boost_INCLUDE_DIRS}
)

add_executable(txtz
  src/txtz.cpp
  src/de_DE-utf8.cpp
  src/shannon-fano.cpp
  src/util.cpp
  smaz/smaz.c
)

add_executable(mapbuilder
  src/mapbuilder.cpp
  src/shannon-fano.cpp
  src/util.cpp
  glob/glob.cpp
)

target_link_libraries(txtz
	${Boost_LIBRARIES}
)

target_link_libraries(mapbuilder
	${Boost_LIBRARIES}
)

if (UNIX)
	add_custom_command(TARGET txtz 
	POST_BUILD
	COMMAND strip txtz)
	add_custom_command(TARGET mapbuilder 
	POST_BUILD
	COMMAND strip mapbuilder)
endif (UNIX)

install(TARGETS txtz
  CONFIGURATIONS Release
  RUNTIME DESTINATION "$ENV{HOME}/bin")
