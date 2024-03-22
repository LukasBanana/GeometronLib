
# === CMake lists for the GeometronLib - (26/07/2015) ===

cmake_minimum_required(VERSION 3.5)
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
	target_link_libraries(${TEST_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} geomlib)
	set_target_properties(${TEST_NAME} PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
	target_compile_features(${TEST_NAME} PRIVATE cxx_range_for)
endmacro()

macro(ADD_DEFINE IDENT)
	if(MSVC)
		add_definitions("/D${IDENT}")
	else()
		add_definitions("-D${IDENT}")
	endif()
endmacro()


# === Global files ===

file(GLOB_RECURSE HeadersAll "${PROJECT_SOURCE_DIR}/include/*.*")
file(GLOB_RECURSE SourcesAll "${PROJECT_SOURCE_DIR}/sources/*.*")

set(FilesAll ${HeadersAll} ${SourcesAll})


# === Options ===

option(GeomLib_DEFAULT_PLANE_EQUATION_ALT "Enables the alternative plane euqation as default (i.e. 'n*x + d = 0' instead of 'n*x = d')" OFF)
option(GeomLib_ENABLE_MULTI_THREADING "Enables multi-threading for a couple of functions (currently only for VisualC++)" ON)

if(GeomLib_ENABLE_MULTI_THREADING)
	ADD_DEFINE(GM_ENABLE_MULTI_THREADING)
endif()

if(GeomLib_DEFAULT_PLANE_EQUATION_ALT)
	ADD_DEFINE(GM_DEFAULT_PLANE_EQUATION_ALT)
endif()


# === Include directories ===

include_directories("${PROJECT_INCLUDE_DIR}")

include(cmake/FindGaussianLib.cmake)


# === Source groups ===

source_group("Include" FILES ${HeadersAll})
source_group("Sources" FILES ${SourcesAll})


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
	ADD_TEST_PROJECT(Test4_Anim "${PROJECT_TEST_DIR}/Test4_Anim.cpp")
	ADD_TEST_PROJECT(Test5_Skeleton "${PROJECT_TEST_DIR}/Test5_Skeleton.cpp")
	ADD_TEST_PROJECT(Test6_Collision "${PROJECT_TEST_DIR}/Test6_Collision.cpp")
	ADD_TEST_PROJECT(Test7_RayCast "${PROJECT_TEST_DIR}/Test7_RayCast.cpp")
	ADD_TEST_PROJECT(Test8_Vectorization "${PROJECT_TEST_DIR}/Test8_Vectorization.cpp")
else()
	message("OpenGL and GLUT missing -> Optional tests excluded from project")
endif()



