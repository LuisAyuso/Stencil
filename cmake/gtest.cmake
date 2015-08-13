########################### GTEST
# Enable ExternalProject CMake module
INCLUDE(ExternalProject)

# Set default ExternalProject root directory
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/third_party/gtest)

# Add gtest
# http://stackoverflow.com/questions/9689183/cmake-googletest
ExternalProject_Add(
    googletest
    URL http://googletest.googlecode.com/files/gtest-1.6.0.zip
    # TIMEOUT 10
    # # Force separate output paths for debug and release builds to allow easy
    # # identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
    # CMAKE_ARGS -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
    #            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
    #            -Dgtest_force_shared_crt=ON
    #CMAKE_ARGS -DGTEST_USE_OWN_TR1_TUPLE=1
    CMAKE_ARGS -DGTEST_HAS_TR1_TUPLE=0
    # Disable install step
    INSTALL_COMMAND ""
    # Wrap download, configure and build steps in a script to log output
    LOG_DOWNLOAD OFF
    LOG_CONFIGURE OFF
    LOG_BUILD OFF
	)

# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR ${source_dir}/include)

# Library
ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARY_PATH ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a)
set(GTEST_LIBRARY gtest)
add_library(${GTEST_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION
                ${GTEST_LIBRARY_PATH} )
add_dependencies(${GTEST_LIBRARY} googletest)

# Library
set(GTEST_LIBRARY_PATH ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a)
set(GTEST_LIBRARY_WITH_MAIN gtest_main)
add_library(${GTEST_LIBRARY_WITH_MAIN} UNKNOWN IMPORTED)
set_property(TARGET ${GTEST_LIBRARY_WITH_MAIN} PROPERTY IMPORTED_LOCATION
                ${GTEST_LIBRARY_PATH} )
add_dependencies(${GTEST_LIBRARY_WITH_MAIN} googletest)
