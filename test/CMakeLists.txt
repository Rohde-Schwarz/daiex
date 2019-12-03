#
# Mosaik.LibdataImportExport/test
#

IF(APPLE)
 # add extra include directories
 include_directories(/usr/local/include) 
 #add extra lib directories
 link_directories(/usr/local/lib)
ENDIF()

#LINK_DIRECTORIES(/opt/local/lib)

### gtest settings start ###
IF( MSVC AND MSVC_VERSION EQUAL 1700 )
  ADD_DEFINITIONS( /D _VARIADIC_MAX=10 )
ENDIF()

IF( MSVC AND CMAKE_CL_64 )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )
ENDIF()

IF( MSVC AND CMAKE_CL_64 )
  ADD_DEFINITIONS( /D _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
ENDIF()
SET( GTEST_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/googletest)
SET( gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" )
ADD_SUBDIRECTORY( ${GTEST_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/../contrib/gtest )

### gtest settings end ###

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_LIST_DIR}/../lib/include )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_LIST_DIR}/src )
INCLUDE_DIRECTORIES( ${GTEST_ROOT}/include )

ADD_LIBRARY( libdai SHARED IMPORTED )
SET_PROPERTY( TARGET libdai PROPERTY INTERFACE_INCLUDE_DIRECTORIES  ${CMAKE_CURRENT_LIST_DIR}/../lib/include )

IF( WIN32 )
  #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  #set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
  SET_PROPERTY( TARGET libdai PROPERTY IMPORTED_IMPLIB ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.lib )
ELSEIF( APPLE )
  SET_PROPERTY( TARGET libdai PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libdaiex.dylib )
  #SET_PROPERTY( TARGET libdai PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libdaiex.so )
ELSEIF( UNIX )

  SET_PROPERTY( TARGET libdai PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libdaiex.so )

ENDIF()


FILE( GLOB SOURCES 
  src/* 
  ${CMAKE_CURRENT_LIST_DIR}/../lib/src/constants.cpp ) # constants are not exported, include for tests
ADD_EXECUTABLE( daitest ${SOURCES} )


IF( WIN32 )

  TARGET_LINK_LIBRARIES( daitest
    libdai
    gtest 
    LIBARCHIVE::LIBARCHIVESTATIC 
    )

  # set warning level
  SET_PROPERTY( TARGET daitest APPEND_STRING PROPERTY COMPILE_FLAGS " /W0")

  # copy dll
  ADD_CUSTOM_COMMAND( TARGET daitest POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.dll ${CMAKE_BINARY_DIR}/test/${CMAKE_BUILD_TYPE}/daiex.dll )

ELSEIF( APPLE )
  TARGET_LINK_LIBRARIES( daitest
    libdai
    gtest
    /usr/local/opt/libarchive/lib/libarchive.dylib
    dl )

  # set warning level
  SET_PROPERTY( TARGET daitest APPEND_STRING PROPERTY COMPILE_FLAGS " -w" )

ELSEIF( UNIX )

  TARGET_LINK_LIBRARIES( daitest
    libdai
    gtest
    LIBARCHIVE::LIBARCHIVE
    dl )

  # set warning level
  SET_PROPERTY( TARGET daitest APPEND_STRING PROPERTY COMPILE_FLAGS " -w" )

ENDIF()