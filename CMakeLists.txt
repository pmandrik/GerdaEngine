#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(SET CMP0011 NEW)

set(TEST_NAME "Tests")
set(CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR}")

project(${TEST_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
add_definitions( -Wfatal-errors )

#-- GERDA ENGINE
message("add GERDA ENGINE ...")
set(GE_PATH ${CMAKE_SOURCE_DIR})
include(${GE_PATH}/GerdaEngine.cmake)

#-- GERDA TESTS :
set(TEST_PATH ${GE_PATH}/Tests)

if(FALSE)
  file( GLOB APP_SOURCES ${TEST_PATH}/*.cpp )
  foreach( testsourcefilefull ${APP_SOURCES} )
      get_filename_component( testsourcefile ${testsourcefilefull} NAME )
      string( REPLACE ".cpp" "" testname ${testsourcefile} )
      add_executable( ${testname} ${testsourcefilefull} )
      target_link_libraries( ${testname} GerdaEngine )
  endforeach( testsourcefilefull ${APP_SOURCES} )
else()
  add_executable( Test03 ${TEST_PATH}/Test03.cpp )
  add_dependencies(Test03 GerdaEngine)
  target_link_libraries( Test03 GerdaEngine )
  install(TARGETS Test03 RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
endif()




