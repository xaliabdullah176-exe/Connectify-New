# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\ConnectifyUI_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ConnectifyUI_autogen.dir\\ParseCache.txt"
  "ConnectifyUI_autogen"
  )
endif()
