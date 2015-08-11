project(Stencil)
cmake_minimum_required(VERSION 2.8)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# look for source files:
# ======================
#    NOTE: this thing has only one main function in the main.cpp file
#    		in the case that you build another program, you migh need to 
#    		pass that file to the add_executable command, as another parameter
#			I know 2 solutions: 
#				- change name of main files to extension cxx and include them manually in add_executable
#				- remove main files form the sources variable and add them manually in add_executable


file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )


#	Configure build 
#	===============
#	include path, compiler flags
include_directories ( "include/" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
add_executable(${PROJECT_NAME} ${sources} )

include (FindX11)
target_link_libraries(${PROJECT_NAME} ${X11_X11_LIB})

include (gtest)

file(GLOB_RECURSE test_sources "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc" )

foreach( test_source ${test_sources} )

	get_filename_component (unit_test ${test_source} NAME_WE )

	include_directories( ${GTEST_INCLUDE_DIR} )
	add_definitions(-DGTEST_COLOR ) 
	add_executable (${unit_test} ${test_source})
	target_link_libraries(${unit_test} ${GTEST_LIBRARY})
	target_link_libraries(${unit_test} ${GTEST_LIBRARY_WITH_MAIN})
	add_test (${unit_test} ${unit_test} )

endforeach(test_source)


enable_testing()

