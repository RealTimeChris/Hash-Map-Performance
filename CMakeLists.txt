# CMakeList.txt : CMake project for Hash-Map-Performance, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

set(PROJECT_NAME "Hash-Map-Performance")
set(LIB_NAME "${PROJECT_NAME}")
set(PRODUCT_VERSION "${VERSION}")
set(DESCRIPTION "A few hashmap implementations and some benchmarks.")

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/Include/*.hpp")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/Source/*.cpp")

set(nanobench_DIR "C:/vcpkg/installed/x64-windows/share/nanobench")

project(
	"${PROJECT_NAME}"
	VERSION "${PRODUCT_VERSION}"
	LANGUAGES CXX
	DESCRIPTION "${DESCRIPTION}"
)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/CMake;")
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

add_executable(
	"${PROJECT_NAME}"
	"./Source/Hash-Map-Performance.cpp"
	"${HEADERS}"
	"${SOURCES}"
)

set_property(TARGET Hash-Map-Performance PROPERTY CXX_STANDARD 20)

target_include_directories(
	"${PROJECT_NAME}" PUBLIC
	"$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/Include>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Include>"
)

target_compile_options(
	"${PROJECT_NAME}" PUBLIC
	"$<$<CXX_COMPILER_ID:GNU>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:/fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:MSVC>:/Zi>"
	"$<$<CXX_COMPILER_ID:MSVC>:/DEBUG>"
	"$<$<CXX_COMPILER_ID:MSVC>:/02>"
	"$<$<CXX_COMPILER_ID:MSVC>:/MP${THREAD_COUNT}>"
)

target_link_options(
	"${PROJECT_NAME}" PUBLIC
	"$<$<CXX_COMPILER_ID:GNU>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:-fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:MSVC>:/INCREMENTAL:NO>"
	"$<$<CXX_COMPILER_ID:MSVC>:/DEBUG>"
)
 
set_target_properties(
	"${PROJECT_NAME}" PROPERTIES 
	"$<$<CXX_COMPILER_ID:MSVC>:$<$<CONFIG:Release>:COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR}>>"
	"$<$<CXX_COMPILER_ID:MSVC>:$<$<CONFIG:Release>:COMPILE_PDB_NAME ${PROJECT_NAME}>>"
)
include(FetchContent)

FetchContent_Declare(
   Jsonifier
   GIT_REPOSITORY https://github.com/RealTimeChris/Jsonifier.git
   GIT_TAG dev
)
FetchContent_MakeAvailable(Jsonifier)

find_package(nanobench CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE Jsonifier::Jsonifier nanobench::nanobench)

set_target_properties(
	"${PROJECT_NAME}" PROPERTIES 
	OUTPUT_NAME "${PROJECT_NAME}-Cpp"
)

if (WIN32)
	install(
		FILES 
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
		DESTINATION "${CMAKE_INSTALL_PREFIX}"
	)
	install(
		FILES 
		"$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_PATH},${RELEASE_PDB_FILE_PATH}>" 
		DESTINATION "${CMAKE_INSTALL_PREFIX}"
	)	
	install(
		FILES 
		"$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
		DESTINATION "${CMAKE_INSTALL_PREFIX}"
	)
endif()

install(
	FILES 
	"$<TARGET_FILE:${PROJECT_NAME}>"
	DESTINATION "./"
)