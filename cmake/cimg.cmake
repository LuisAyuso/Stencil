########################### CIMG

# Enable ExternalProject CMake module
INCLUDE(ExternalProject)

# Set default ExternalProject root directory
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/third_party/cimg)

# Add CImg https://github.com/dtschump/CImg.git
ExternalProject_Add(
    cimg
    GIT_REPOSITORY https://github.com/dtschump/CImg.git
    # TIMEOUT 10
    # do not configure
    CONFIGURE_COMMAND  ""
    # do not build
    BUILD_COMMAND  ""
    # Disable install step
    INSTALL_COMMAND ""
    )

# Specify include dir
ExternalProject_Get_Property(cimg source_dir)
set(CIMG_INCLUDE_DIR ${source_dir})

