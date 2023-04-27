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
FIND_PACKAGE( LibArchive REQUIRED )

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
find_package(GTest)

### gtest settings end ###

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_LIST_DIR}/../lib/include )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_LIST_DIR}/src )

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
    GTest::gtest
    LibArchive::LibArchive
    )

  # set warning level
  SET_PROPERTY( TARGET daitest APPEND_STRING PROPERTY COMPILE_FLAGS " /W0")

  # copy dll
  ADD_CUSTOM_COMMAND( TARGET daitest POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.dll ${CMAKE_BINARY_DIR}/test/${CMAKE_BUILD_TYPE}/daiex.dll )
  set_target_properties(daitest PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "\"${CMAKE_CURRENT_SOURCE_DIR}/res\" \"${CMAKE_CURRENT_BINARY_DIR}/result_log\"")
  set_target_properties(daitest PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:daiex>;%PATH%")

ELSEIF( APPLE )
  TARGET_LINK_LIBRARIES( daitest
    libdai
    GTest::gtest
    /usr/local/opt/libarchive/lib/libarchive.dylib
    dl )

  # set warning level
  SET_PROPERTY( TARGET daitest APPEND_STRING PROPERTY COMPILE_FLAGS " -w" )

ELSEIF( UNIX )

  TARGET_LINK_LIBRARIES( daitest
    libdai
    GTest::gtest
    LibArchive::LibArchive
    dl )

  # set warning level
  SET_PROPERTY( TARGET daitest APPEND_STRING PROPERTY COMPILE_FLAGS " -w" )

ENDIF()