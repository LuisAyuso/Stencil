project(Stencil)
cmake_minimum_required(VERSION 2.8)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (cimg)
include (gtest)
include (FindX11)

if(APPLE)
   include_directories(AFTER "/opt/X11/include")
endif()

# -------------------------------------------------------------------

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )


#	Configure build 
#	===============
#	include path, compiler flags
include_directories ( "include/" )
include_directories( ${CIMG_INCLUDE_DIR} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


# 2d exec

add_executable("Stencil2D" "src/main2D.cxx" ${sources} )
add_dependencies("Stencil2D" cimg)
target_link_libraries("Stencil2D" ${X11_X11_LIB})

# 3d exec
#add_executable("Stencil3D" "src/main3D.cxx" ${sources} )
#add_dependencies("Stencil3D" cimg)


file(GLOB_RECURSE test_sources "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc" )

foreach( test_source ${test_sources} )

	get_filename_component (unit_test ${test_source} NAME_WE )

	include_directories( ${GTEST_INCLUDE_DIR} )
	add_definitions(-DGTEST_COLOR ) 
	add_executable (${unit_test} ${test_source})
	target_link_libraries(${unit_test} ${GTEST_LIBRARY})
	target_link_libraries(${unit_test} ${GTEST_LIBRARY_WITH_MAIN})
	target_link_libraries(${unit_test} ${X11_X11_LIB})
	add_test (${unit_test} ${unit_test} )

endforeach(test_source)


enable_testing()

