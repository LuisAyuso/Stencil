project(Stencil)
cmake_minimum_required(VERSION 2.8)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (cimg)
include (gtest)
include (FindX11)

# -------------------------------------------------------------------

if(APPLE)
    include_directories(AFTER "/opt/X11/include")
    set(NO_TEST True)
endif()


if (CMAKE_COMPILER_IS_GNUCC)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
    string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
    list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
    list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)

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

# ========================== SEQUENTIAL =====================================

# 2d exec
add_executable("Stencil2D" "src/main2D.cxx" ${sources} )
add_dependencies("Stencil2D" cimg)
target_link_libraries("Stencil2D" ${X11_X11_LIB})

# 3d exec
add_executable("Stencil3D" "src/main3D.cxx" ${sources} )


# ========================== OPENMP =====================================

if (NOT NO_OMP)
find_package(OpenMP)
if (OPENMP_FOUND)

	# 2d exec
	add_executable	     ("Stencil2D-omp" "src/main2D.cxx" ${sources} )
	add_dependencies	 ("Stencil2D-omp" cimg)
	target_link_libraries("Stencil2D-omp" ${X11_X11_LIB})
	SET_TARGET_PROPERTIES("Stencil2D-omp" PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
	SET_TARGET_PROPERTIES("Stencil2D-omp" PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})

	# 3d exec
	add_executable		 ("Stencil3D-omp" "src/main3D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil3D-omp" PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
	SET_TARGET_PROPERTIES("Stencil3D-omp" PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})

endif()
endif()

# ========================== OPENMP =====================================

find_package(Cilk)
if (CILK_FOUND)

	# 2d exec
	add_executable		 ("Stencil2D-cilk" "src/main2D.cxx" ${sources} )
	add_dependencies	 ("Stencil2D-cilk" cimg)
	target_link_libraries("Stencil2D-cilk" ${X11_X11_LIB})
	SET_TARGET_PROPERTIES("Stencil2D-cilk" PROPERTIES COMPILE_FLAGS "-DCILK")

	# 3d exec
	add_executable		 ("Stencil3D-cilk" "src/main3D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil3D-cilk" PROPERTIES COMPILE_FLAGS "-DCILK")

endif()

# ================================= TESTS =================================

if (NOT NO_TEST)

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

endif()

