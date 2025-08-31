# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MedicalSystemVersionTwo_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MedicalSystemVersionTwo_autogen.dir\\ParseCache.txt"
  "MedicalSystemVersionTwo_autogen"
  )
endif()
