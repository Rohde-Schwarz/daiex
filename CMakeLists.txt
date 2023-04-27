#
# Mosaik.LibdataImportExport
#

IF(APPLE)
 # add extra include directories
 include_directories(/usr/local/opt/libarchive/include) 
 include_directories(/usr/local/include) 
 #add extra lib directories
 link_directories(/usr/local/opt/libarchive/lib)
 link_directories(/usr/local/lib)
ENDIF()

cmake_minimum_required( VERSION 3.8 ) # >= 3.8 for better csharp support
IF( WIN32 )
project( Mosaik.LibDataImportExport  LANGUAGES CXX CSharp)
ELSE()
project( Mosaik.LibDataImportExport  LANGUAGES CXX)
ENDIF()


option(BUILD_DOC "Build doc" ON)
option(BUILD_TEST "Build test" ON)
option(BUILD_DOT_NET_WRAPPER "Build .Net Wrapper" ON)

SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/" )

# SET( CMAKE_BUILD_TYPE "Debug" CACHE TYPE "build type" )
SET( CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE TYPE INTERNAL FORCE )

IF( UNIX )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -D_FILE_OFFSET_BITS=64" )
ENDIF()

add_subdirectory( lib )
add_subdirectory( app )
if (BUILD_TEST)
  add_subdirectory( test )
endif()


add_dependencies( daiapp daiex )
if (BUILD_TEST)
  add_dependencies( daitest daiex )	
endif()

IF( WIN32 )
  # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi" CACHE STRING "" FORCE)
  # set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF" CACHE STRING "" FORCE)  
  # set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF" CACHE STRING "" FORCE)  
  IF (BUILD_DOT_NET_WRAPPER)
    add_subdirectory( wrapper )
    IF (BUILD_TEST)
      add_subdirectory( wrapper_test )
    ENDIF()
  ENDIF()
ENDIF()


### installer ###

set( PLATFORM "amd64" )

IF( WIN32 )
  set( CPACK_ARCHIVE_COMPONENT_INSTALL ON)
  set( CPACK_GENERATOR "ZIP" )
  set( CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF )

  file( STRINGS "${CMAKE_CURRENT_LIST_DIR}/Version.txt" VERSION )

  set( GENERATOR "vs16" )
  if( ${CMAKE_GENERATOR} MATCHES "Visual Studio 15" ) # VS2017
    set( GENERATOR "vs15" )
    set (PLATFORMTOOLSET "v141")
  elseif( ${CMAKE_GENERATOR} MATCHES "Visual Studio 16" ) # VS2019
    set( GENERATOR "vs16" )
    set (PLATFORMTOOLSET "v142")
  elseif( ${CMAKE_GENERATOR} MATCHES "Visual Studio 17" ) # VS2022
    set( GENERATOR "vs17" )
    set (PLATFORMTOOLSET "v143")
  else()
  endif()

  set( CPACK_PACKAGE_FILE_NAME "LibDataImportExport-${VERSION}-${GENERATOR}-${PLATFORM}-${CMAKE_BUILD_TYPE}" )

  include( CPack )
ELSE()
  set( CPACK_ARCHIVE_COMPONENT_INSTALL ON)
  set( CPACK_GENERATOR "TGZ" )
  set( CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF )


  file( STRINGS "${CMAKE_CURRENT_LIST_DIR}/Version.txt" VERSION )

  set( GENERATOR "ubu18-04" )
  set( CPACK_PACKAGE_FILE_NAME "LibDataImportExport-${VERSION}-${GENERATOR}-${PLATFORM}-${CMAKE_BUILD_TYPE}" )
  include( CPack )
ENDIF()
