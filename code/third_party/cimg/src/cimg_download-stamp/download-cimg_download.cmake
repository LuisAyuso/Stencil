message(STATUS "downloading...
     src='http://cimg.eu/files/CImg_latest.zip'
     dst='/Users/luis/code/stencil_CXX/code/third_party/cimg/src/CImg_latest.zip'
     timeout='none'")




file(DOWNLOAD
  "http://cimg.eu/files/CImg_latest.zip"
  "/Users/luis/code/stencil_CXX/code/third_party/cimg/src/CImg_latest.zip"
  SHOW_PROGRESS
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'http://cimg.eu/files/CImg_latest.zip' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
