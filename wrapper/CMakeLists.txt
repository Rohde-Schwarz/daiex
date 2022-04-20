#
# Mosaik.LibdataImportExport/wrapper
#

  set( GENERATOR "" )
  if( ${CMAKE_GENERATOR} MATCHES "Visual Studio 11" )
    set( GENERATOR "vs11" )
    set (PLATFORMTOOLSET "v110")
  endif()
  if( ${CMAKE_GENERATOR} MATCHES "Visual Studio 12" )
    set( GENERATOR "vs12" )
    set (PLATFORMTOOLSET "v120")
  endif()
  if( ${CMAKE_GENERATOR} MATCHES "Visual Studio 15" )
    set( GENERATOR "vs15" )
    set (PLATFORMTOOLSET "v141")
  endif()

  if( ${CMAKE_GENERATOR} MATCHES "Visual Studio 16" )
    set( GENERATOR "vs16" )
    set (PLATFORMTOOLSET "v142")
  endif()
# message(STATUS "PLATFORMTOOLSET: " ${PLATFORMTOOLSET})


file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/wrapper )

set( CONFIGURATION ${CMAKE_BUILD_TYPE} )

set( PLATFORM x64 )

set( INC_DIR ${CMAKE_CURRENT_LIST_DIR}/include )
set( SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src )
set( RES_DIR ${CMAKE_CURRENT_LIST_DIR} )
set( NATIVE_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/../lib/include )
set( NATIVE_LIB ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.lib )
set( BUILD_DIR ${CMAKE_BINARY_DIR}/wrapper/${CMAKE_BUILD_TYPE} )
set( INTERMEDIATE_DIR ${CMAKE_BINARY_DIR}/wrapper/${CMAKE_BUILD_TYPE}/intermediate )
set( LOG_DIR ${CMAKE_BINARY_DIR}/wrapper/${CMAKE_BUILD_TYPE} )

set( CS_PROJECT ${CMAKE_BINARY_DIR}/daiex_wrapper.vcxproj )
configure_file( ${CMAKE_CURRENT_LIST_DIR}/daiex_wrapper.vcxproj.in ${CS_PROJECT} @ONLY )
configure_file( ${CMAKE_CURRENT_LIST_DIR}/daiex_wrapper.vcxproj.filters.in ${CMAKE_BINARY_DIR}/daiex_wrapper.vcxproj.filters @ONLY )

include_external_msproject( wrapper ${CS_PROJECT}
  TYPE 8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942 
  PLATFORM ${PLATFORM} )


### installer ###

install( FILES ${CMAKE_BINARY_DIR}/wrapper/${CMAKE_BUILD_TYPE}/daiex_wrapper.dll DESTINATION bin )