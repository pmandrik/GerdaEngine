#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(SET CMP0011 NEW)

set(TEST_NAME "ShtaEngineTests")
set(CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
SET(BUILD_SE_TESTS FALSE CACHE BOOL "build with Shta engine tests") # cmake -DBUILD_SE_TESTS:BOOL=1
SET(BUILD_SE_HLT_TESTS FALSE CACHE BOOL "build with Shta engine HLT tests") # cmake -DBUILD_SE_HLT_TESTS:BOOL=1

project(${TEST_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
add_definitions( -Wfatal-errors )

#-- Shta TESTS :
set(SE_PATH   ${CMAKE_SOURCE_DIR})
set(TEST_PATH ${SE_PATH}/Tests)

#-- add external
set(SE_EXTERN ${CMAKE_SOURCE_DIR}/../ShtaExtern)
  #-- PMANDRIK_LIBRARY
  add_definitions(-DNO_CERN_ROOT)
  include_directories( ${SE_EXTERN}/PMANDRIK_LIBRARY/ )

  #-- TINYXML2
  include_directories( ${SE_EXTERN}/tinyxml2 )
  file(GLOB subsources ${SE_EXTERN}/tinyxml2/*.cpp)
  set(sources ${sources} ${subsources})

  #-- STB
  include_directories( ${SE_EXTERN}/stb )

  #-- UTF8
  include_directories( ${SE_EXTERN}/utfcpp/source )

  #-- OTHER
  include_directories( ${SE_EXTERN}/Other )

#-- Shta tests based on Shta Engine
if(BUILD_SE_TESTS)
  #-- SHTA ENGINE
  message("add Shta ENGINE ...")
  include(${SE_PATH}/ShtaEngine.cmake)
  
  file( GLOB APP_SOURCES ${TEST_PATH}/*SE.cpp )
  foreach( testsourcefilefull ${APP_SOURCES} )
      get_filename_component( testsourcefile ${testsourcefilefull} NAME )
      string( REPLACE ".cpp" "" testname ${testsourcefile} )
      add_executable( ${testname} ${testsourcefilefull} )
      target_link_libraries( ${testname} ShtaEngine )
  endforeach( testsourcefilefull ${APP_SOURCES} )
endif()

#-- Shta modules tests without core Shta Engine
if(BUILD_SE_HLT_TESTS)
  enable_testing()
  include_directories( ${SE_PATH}/ShtaHLT )
  file( GLOB APP_SOURCES ${TEST_PATH}/*HLT.cpp )
  foreach( testsourcefilefull ${APP_SOURCES} )
      get_filename_component( testsourcefile ${testsourcefilefull} NAME )
      string( REPLACE ".cpp" "" testname ${testsourcefile} )
      add_executable( ${testname} ${testsourcefilefull} )
      add_test(NAME ${testsourcefilefull} COMMAND ${testname} )
  endforeach( testsourcefilefull ${APP_SOURCES} )
endif()

file(GLOB ENGINE_DATA ${GE_PATH}/Data/*)
file(COPY ${ENGINE_DATA} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Data)


