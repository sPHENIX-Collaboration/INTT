#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(lib)

# for debugging
set(CMAKE_VERBOSE_MAKEFILE TRUE)

#----------------------------------------------------------------------------
# ROOT
find_package(ROOT)
include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries

#----------------------------------------------------------------------------
# make a dummy executable, it's also good for a test running
add_executable( dummy dummy.cc StepMC.cc TrackMC.cc )
target_link_libraries( dummy ${ROOT_LIBRARIES} TrackMC StepMC )
# link_directories(${ROOT_LIBRARIES} ${CMAKE_CURRENT_SOURCE_DIR} )

# StepMC class
add_library( StepMC SHARED StepMC_dict )
target_link_libraries( StepMC ${ROOT_LIBRARIES} )
ROOT_GENERATE_DICTIONARY( StepMC_dict StepMC.cc LINKDEF LinkDef_StepMC.h )

# TrackMC class
ROOT_GENERATE_DICTIONARY( TrackMC_dict TrackMC.cc LINKDEF LinkDef_TrackMC.h)
add_library( TrackMC SHARED TrackMC_dict )
target_link_libraries( TrackMC ${ROOT_LIBRARIES} StepMC )

# Event class, useless
#ROOT_GENERATE_DICTIONARY( Eventa_dict Eventa.cc LINKDEF LinkDef_Event.h)
#add_library( Eventa SHARED Eventa_dict )
#target_link_libraries( Eventa ${ROOT_LIBRARIES} TrackMC )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build ED. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
# set(EXAMPLEED_SCRIPTS  test.cc ana.cc TrackMC.hh TrackMC.cc StepMC.hh StepMC.cc )
 set(EXAMPLEED_SCRIPTS  test.cc ana.cc TrackMC.hh StepMC.hh StepMC.cc )

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
# add_custom_target(ED DEPENDS exampleED)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS TrackMC Event DESTINATION bin)
