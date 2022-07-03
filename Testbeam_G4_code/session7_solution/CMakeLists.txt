#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(ED)

# for debugging
set(CMAKE_VERBOSE_MAKEFILE TRUE)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

find_package(ROOT)
include(${ROOT_USE_FILE})
#include_directories(${ROOT_INCLUDE_DIRS} lib)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project

include(${Geant4_USE_FILE})
#include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIRS} lib)

#----------------------------------------------------------------------------
# Add subdirectory for shared libraries
add_subdirectory( lib )

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
add_executable(exampleED exampleED.cc ${sources} ${headers} ) #lib/TrackMC.cc lib/StepMC.cc )
# target_link_libraries(exampleED ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} StepMC TrackMC )
target_link_libraries(exampleED ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
link_directories(${ROOT_LIBRARIES} ${CMAKE_CURRENT_SOURCE_DIR} )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build ED. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(EXAMPLEED_SCRIPTS
  init_vis.mac
  run.mac
  run.png
  vis.mac
  INTT_config.mac
  # beam_parameter/Au_20um.root
  # beam_parameter/W_200um.root
  #  beam_parameter/Cu_8mm.root
  # beam_parameter/data/ebl-hb/23/w425.root
  beam_parameter/beam_W_400A.dat
  #beam_parameter/beam_W_400A_original.dat
  beam_parameter/beam_W_400A_mini.dat
  # interface_v4.c
  )

foreach(_script ${EXAMPLEED_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(ED DEPENDS exampleED)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#install(TARGETS exampleED DESTINATION bin)
