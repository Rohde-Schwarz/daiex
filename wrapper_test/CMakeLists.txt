#
# Mosaik.LibdataImportExport/wrapper_test
#

file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/wrapper )

set( CONFIGURATION ${CMAKE_BUILD_TYPE} )

set( PLATFORM win32 )
set( TARGET_PLATFORM x86 )
if( ${CMAKE_GENERATOR} MATCHES "Win64" )
  set( PLATFORM x64 )
  set( TARGET_PLATFORM x64 )
endif()

string( REPLACE "/" "\\" SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src/" )

set( DAIEX_WRAPPER ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.lib )
set( BUILD_DIR ${CMAKE_BINARY_DIR}/wrapper_test/${CMAKE_BUILD_TYPE} )

set( CS_PROJECT ${CMAKE_BINARY_DIR}/daiex_wrapper_test.csproj )
configure_file( ${CMAKE_CURRENT_LIST_DIR}/daiex_wrapper_test.csproj.in ${CS_PROJECT} @ONLY )

include_external_msproject( wrapper_test ${CS_PROJECT}
  TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC )