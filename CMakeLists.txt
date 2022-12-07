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

cmake_minimum_required( VERSION 2.8 )
project( Mosaik.LibDataImportExport )

SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/" )

SET( CMAKE_BUILD_TYPE "Debug" CACHE TYPE "build type" )
SET( CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE TYPE INTERNAL FORCE )

IF( UNIX )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -D_FILE_OFFSET_BITS=64" )
ENDIF()


IF( WIN32 )
  #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  #set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")  
  
  add_subdirectory( wrapper )
  add_subdirectory( wrapper_test )
  add_dependencies( wrapper daiex )  
  add_dependencies( wrapper_test wrapper )
ENDIF()


add_subdirectory( lib )
add_subdirectory( app )
add_subdirectory( test )


add_dependencies( daiapp daiex )
add_dependencies( daitest daiex )	


### installer ###

set( PLATFORM "amd64" )

IF( WIN32 )
  set( CPACK_ARCHIVE_COMPONENT_INSTALL ON)
  set( CPACK_GENERATOR "ZIP" )
  set( CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF )

  file( STRINGS "${CMAKE_CURRENT_LIST_DIR}/Version.txt" VERSION )

  set( GENERATOR "vs16" )
  if( ${CMAKE_GENERATOR} MATCHES "Visual Studio 15" )
    set( GENERATOR "vs15" )
    set (PLATFORMTOOLSET "v141")
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
