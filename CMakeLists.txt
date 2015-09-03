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

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/code/src/*.cpp" )


#	Configure build 
#	===============
#	include path, compiler flags
include_directories ( "code/include/" )
include_directories( ${CIMG_INCLUDE_DIR} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# ========================== SEQUENTIAL =====================================

# 1d exec
add_executable("Stencil1D" "code/src/main1D.cxx" ${sources} )
add_dependencies("Stencil1D" cimg)
target_link_libraries("Stencil1D" ${X11_X11_LIB})

# 2d exec
add_executable("Stencil2D" "code/src/main2D.cxx" ${sources} )
add_dependencies("Stencil2D" cimg)
target_link_libraries("Stencil2D" ${X11_X11_LIB})

# 3d exec
add_executable("Stencil3D" "code/src/main3D.cxx" ${sources} )


# ========================== OPENMP =====================================

if (NOT NO_OMP)
find_package(OpenMP)
if (OPENMP_FOUND)

	# 1d exec 
	add_executable	     ("Stencil1D-omp" "code/src/main1D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil1D-omp" PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
	SET_TARGET_PROPERTIES("Stencil1D-omp" PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})

	# 2d exec
	add_executable	     ("Stencil2D-omp" "code/src/main2D.cxx" ${sources} )
	add_dependencies	 ("Stencil2D-omp" cimg)
	target_link_libraries("Stencil2D-omp" ${X11_X11_LIB})
	SET_TARGET_PROPERTIES("Stencil2D-omp" PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
	SET_TARGET_PROPERTIES("Stencil2D-omp" PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})

	# 3d exec
	add_executable		 ("Stencil3D-omp" "code/src/main3D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil3D-omp" PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
	SET_TARGET_PROPERTIES("Stencil3D-omp" PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})

endif()
endif()

# ========================== CILK =====================================

find_package(Cilk)
if (CILK_FOUND)

	# 1d exec
	add_executable		 ("Stencil1D-cilk" "code/src/main1D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil1D-cilk" PROPERTIES COMPILE_FLAGS "-DCILK ${Cilk_CXX_FLAGS}")
	if (Cilk_LINK_LIBRARIES)
		SET_TARGET_PROPERTIES("Stencil1D-cilk" PROPERTIES LINK_FLAGS ${Cilk_LINK_LIBRARIES})
	endif()

	# 2d exec
	add_executable		 ("Stencil2D-cilk" "code/src/main2D.cxx" ${sources} )
	add_dependencies	 ("Stencil2D-cilk" cimg)
	target_link_libraries("Stencil2D-cilk" ${X11_X11_LIB})
	SET_TARGET_PROPERTIES("Stencil2D-cilk" PROPERTIES COMPILE_FLAGS "-DCILK ${Cilk_CXX_FLAGS}")
	if (Cilk_LINK_LIBRARIES)
		SET_TARGET_PROPERTIES("Stencil2D-cilk" PROPERTIES LINK_FLAGS ${Cilk_LINK_LIBRARIES})
	endif()

	# 3d exec
	add_executable		 ("Stencil3D-cilk" "code/src/main3D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil3D-cilk" PROPERTIES COMPILE_FLAGS "-DCILK ${Cilk_CXX_FLAGS}")
	if (Cilk_LINK_LIBRARIES)
		SET_TARGET_PROPERTIES("Stencil3D-cilk" PROPERTIES LINK_FLAGS ${Cilk_LINK_LIBRARIES})
	endif()

endif()

# ========================== CXX_ASYNC PARALLELISM =====================================

	# 1d exec
	add_executable		 ("Stencil1D-cxx" "code/src/main1D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil1D-cxx" PROPERTIES COMPILE_FLAGS "-DCXX_ASYNC")

	# 2d exec
	add_executable		 ("Stencil2D-cxx" "code/src/main2D.cxx" ${sources} )
	add_dependencies	 ("Stencil2D-cxx" cimg)
	target_link_libraries("Stencil2D-cxx" ${X11_X11_LIB})
	SET_TARGET_PROPERTIES("Stencil2D-cxx" PROPERTIES COMPILE_FLAGS "-DCXX_ASYNC")

	# 3d exec
	add_executable		 ("Stencil3D-cxx" "code/src/main3D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil3D-cxx" PROPERTIES COMPILE_FLAGS "-DCXX_ASYNC")


# ========================== INSIEME RT =====================================

if(INSIEME_CODE_PATH)

	message(STATUS "compile InsiemeRT version")

	set(RT_HEADERS ${INSIEME_CODE_PATH}/runtime/include)
	set(COMMON_HEADERS ${INSIEME_CODE_PATH}/common/include)

	message(STATUS "INSIEME RT: ${RT_HEADERS}")

	# 1d exec
	add_executable		 ("Stencil1D-insiemert" "code/src/main1D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil1D-insiemert" PROPERTIES COMPILE_FLAGS "-I ${RT_HEADERS} -I ${COMMON_HEADERS} -DINSIEME_RT -DIRT_STEAL_OTHER_POP_FRONT")

	# 3d exec
	add_executable		 ("Stencil3D-insiemert" "code/src/main3D.cxx" ${sources} )
	SET_TARGET_PROPERTIES("Stencil3D-insiemert" PROPERTIES COMPILE_FLAGS "-I ${RT_HEADERS} -I ${COMMON_HEADERS} -DINSIEME_RT -DIRT_STEAL_OTHER_POP_FRONT")


endif()


# ========================== MPI =====================================

find_package(MPI)
if(MPI_CXX_FOUND)

	# 3d exec
	add_executable		 ("Stencil3D-mpi" "code/src/main3D.cxx" ${sources} )

endif()


# ================================= TESTS =================================

if (NOT NO_TEST)

    file(GLOB_RECURSE test_sources "${CMAKE_CURRENT_SOURCE_DIR}/code/test/*.cc" )

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

