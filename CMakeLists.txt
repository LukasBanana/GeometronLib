
# === CMake lists for the GeometronLib - (26/07/2015) ===

cmake_minimum_required(VERSION 2.8)
project(GeometronLib)


# === Build path ===

set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_TEST_DIR "${PROJECT_SOURCE_DIR}/test")


# === Preprocessor definitions ===

macro(ADD_TEST_PROJECT TEST_NAME TEST_FILES)
	add_executable(${TEST_NAME} ${TEST_FILES})
	target_link_libraries(${TEST_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} geomlib)
	set_target_properties(${TEST_NAME} PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
	target_compile_features(${TEST_NAME} PRIVATE cxx_range_for)
endmacro()


# === Global files ===

file(GLOB_RECURSE HeadersAll "${PROJECT_SOURCE_DIR}/include/*.*")
file(GLOB_RECURSE SourcesAll "${PROJECT_SOURCE_DIR}/sources/*.*")

set(FilesAll ${HeadersAll} ${SourcesAll})


# === Include directories ===

include_directories("${PROJECT_INCLUDE_DIR}")

find_path(GaussianLib_INCLUDE_DIR NAMES Gauss/Gauss.h)
include_directories(${GaussianLib_INCLUDE_DIR})


# === Binaries ===

add_library(geomlib STATIC ${FilesAll})
set_target_properties(geomlib PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
target_compile_features(geomlib PRIVATE cxx_strong_enums cxx_auto_type)

add_executable(Test1_Primitives "${PROJECT_TEST_DIR}/Test1_Primitives.cpp")
set_target_properties(Test1_Primitives PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
target_compile_features(Test1_Primitives PRIVATE cxx_strong_enums cxx_auto_type)
target_link_libraries(Test1_Primitives geomlib)

find_package(OpenGL)
find_package(GLUT)
if(OpenGL_FOUND AND GLUT_FOUND)
	include_directories(${GLUT_INCLUDE_DIR})
	ADD_TEST_PROJECT(Test2_Mesh "${PROJECT_TEST_DIR}/Test2_Mesh.cpp")
	ADD_TEST_PROJECT(Test3_MeshGen "${PROJECT_TEST_DIR}/Test3_MeshGen.cpp")
else()
	message("OpenGL and GLUT missing -> Optional tests excluded from project")
endif()



