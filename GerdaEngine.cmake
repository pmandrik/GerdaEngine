
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

message(STATUS "GerdaEngine.cmake loaded from ${GE_PATH}")

#-- OPENGL
find_package(OpenGL)

#-- SDL
include_directories(/home/me/my/gd/rstmn/engine_progress/SDL2-2.0.5/include/)
include_directories(/home/me/my/gd/rstmn/engine_progress/SDL2-2.0.5/build/include/)
link_directories(/home/me/my/gd/rstmn/engine_progress/SDL2-2.0.5/build/)
set(SDL2_LIBRARIES SDL2-2.0.so)

#-- PMANDRIK_LIBRARY
add_definitions(-DNO_CERN_ROOT)
include_directories( ${GE_PATH}/Extern/PMANDRIK_LIBRARY/ )

#-- TINYXML2
include_directories( ${GE_PATH}/Extern/tinyxml2 )
file(GLOB subsources ${GE_PATH}/Extern/tinyxml2/*.cpp)
set(sources ${sources} ${subsources})

#-- STB
include_directories( ${GE_PATH}/Extern/stb )

#-- UTF8
include_directories( ${GE_PATH}/Extern/utf8_v2_3_4/source )

#-- OTHER
include_directories( ${GE_PATH}/Extern/Other )

#-- GERDA ENGINE
include_directories( ${GE_PATH} )
include_directories( ${GE_PATH}/GerdaMath )
include_directories( ${GE_PATH}/GerdaCore )
include_directories( ${GE_PATH}/GerdaOpenGL )
include_directories( ${GE_PATH}/GerdaHLT )
include_directories( ${GE_PATH}/Tests )
set(GERDA_SOURCES ${sources} )

add_library(GerdaEngine SHARED ${GE_PATH}/GerdaEngine.cpp ${sources})
#target_link_libraries(Gerda ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
target_link_libraries(GerdaEngine ${SDL2_LIBRARIES})

file(GLOB ENGINE_DATA ${GE_PATH}/Data/*)
file(COPY ${ENGINE_DATA} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Data)


