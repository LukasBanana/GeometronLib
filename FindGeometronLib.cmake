
# Custom CMake module for finding "GeometronLib" files
# Written by Lukas Hermanns on 24/08/2015

# Macros

macro(_GM_APPEND_LIBRARIES _list _release)
	set(_debug ${_release}_DEBUG)
	if(${_debug})
		set(${_list} ${${_list}} optimized ${${_release}} debug ${${_debug}})
	else()
		set(${_list} ${${_list}} ${${_release}})
	endif()
endmacro()

# Find library

find_path(GeomLib_INCLUDE_DIR Geom/Geom.h)

find_library(GeomLib_LIBRARY NAMES geomlib)
find_library(GeomLib_LIBRARY_DEBUG NAMES geomlibD)

# Setup package handle

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	GeomLib
	DEFAULT_MSG
	GeomLib_INCLUDE_DIR
    GeomLib_LIBRARY
    GeomLib_LIBRARY_DEBUG
)

if(GEOMLIB_FOUND)
	set(GeomLib_FOUND TRUE)
	_GM_APPEND_LIBRARIES(GeomLib_LIBRARIES GeomLib_LIBRARY)
endif(GEOMLIB_FOUND)
