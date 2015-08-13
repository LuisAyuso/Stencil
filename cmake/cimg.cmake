########################### CIMG

# Enable ExternalProject CMake module
INCLUDE(ExternalProject)

# Set default ExternalProject root directory
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/third_party/cimg)

# Add CImg https://github.com/dtschump/CImg.git
ExternalProject_Add(
    cimg_download
	URL http://cimg.eu/files/CImg_latest.zip
    #GIT_REPOSITORY https://github.com/dtschump/CImg.git
    # TIMEOUT 10
    # do not configure
    CONFIGURE_COMMAND  ""
    # do not build
    BUILD_COMMAND  ""
    # Disable install step
    INSTALL_COMMAND ""
    LOG_DOWNLOAD OFF
    LOG_CONFIGURE OFF
    LOG_BUILD OFF
)



# Specify include dir
ExternalProject_Get_Property(cimg_download source_dir)
set(CIMG_DIR ${source_dir})


add_custom_target( cimg DEPENDS cimg_download)
set_property(TARGET cimg PROPERTY header_dir ${CIMG_DIR})
get_property(CIMG_INCLUDE_DIR TARGET cimg PROPERTY header_dir)






