#
# Mosaik.LibdataImportExport/lib
#

add_subdirectory(iqxformat) 
FIND_PACKAGE( Archive REQUIRED )
if (APPLE)
else()
FIND_PACKAGE( ZLib REQUIRED )
FIND_PACKAGE( LibHdf5 REQUIRED )
endif()
FIND_PACKAGE( LibMatIo REQUIRED )

### doxygen configuration start ###

FIND_PACKAGE( Doxygen )
FIND_PACKAGE( HTMLHelp )
IF( DOXYGEN )
  CONFIGURE_FILE (
    ${CMAKE_SOURCE_DIR}/lib/doc/Doxyfile.in
    ${CMAKE_BINARY_DIR}/lib/doc/Doxyfile
  )
  ADD_CUSTOM_TARGET(
    doc ALL
    ${DOXYGEN} ${CMAKE_BINARY_DIR}/lib/doc/Doxyfile
    DEPENDS ${CMAKE_SOURCE_DIR}/lib/doc/Doxyfile.in
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    SOURCES
    ${CMAKE_SOURCE_DIR}/lib/doc/mainpage.dox
  )
  SET_PROPERTY(
    TARGET doc
    PROPERTY FOLDER "doc"
  )

ENDIF( DOXYGEN )

### doxygen configuration end ###

### lib daiex ####

FILE( GLOB SOURCES src/*.cpp )

IF( WIN32 )

  FILE( GLOB SOURCES_PLATFORM
    src/platform/platform_win.cpp )

ELSEIF( UNIX )

  FILE( GLOB SOURCES_PLATFORM
    src/platform/platform_linux.cpp )

ENDIF()
FILE( GLOB AID_SOURCES aid/src/*.cpp )

FILE( GLOB INCLUDES include/*.h)
FILE( GLOB AID_INCLUDES aid/include/*.h)

FILE( GLOB MMF
  ${CMAKE_SOURCE_DIR}/3rdparty/mmf/*.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/mmf/*.hpp )

FILE( GLOB PUGIXML
  ${CMAKE_SOURCE_DIR}/3rdparty/PugiXml/*.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/PugiXml/*.hpp )

SET( ALL_COMMON
  ${SOURCES}
  ${SOURCES_PLATFORM}
  ${AID_SOURCES}
  ${INCLUDES}
  ${AID_INCLUDES}
  ${MMF}
  ${PUGIXML} )

SET( RES_FILES libdai.rc )

IF( WIN32 )

  ADD_LIBRARY( daiex SHARED 
    ${ALL_COMMON} 
    ${RES_FILES} )

  SOURCE_GROUP( "Resource Files" ${RES_FILES} )

ELSEIF( UNIX )

  ADD_LIBRARY( daiex SHARED ${ALL_COMMON} )
  
  # set default symbol export to hidden, only export R/S interfaces.
  TARGET_COMPILE_OPTIONS( daiex PRIVATE -fvisibility=hidden )

ENDIF()

SET( INC_DIRS 
  include/
  aid/include/
  ${CMAKE_SOURCE_DIR}/3rdparty/mmf/
  ${CMAKE_SOURCE_DIR}/3rdparty/PugiXml/ )

TARGET_INCLUDE_DIRECTORIES( daiex PRIVATE ${INC_DIRS} )

IF( WIN32 )

  TARGET_COMPILE_DEFINITIONS( daiex
    PUBLIC EXPORT_MODULE )

  TARGET_LINK_LIBRARIES( daiex 
    LIBARCHIVE::LIBARCHIVESTATIC 
    zlibstatic
    LIBHDF5::LIBHDF5STATIC
    libmatio_static 
    Rpcrt4.lib
	  iqxformatstatic)

  # set warning level
  SET_PROPERTY( TARGET daiex APPEND_STRING PROPERTY COMPILE_FLAGS " /W4")

  # copy dll
  ADD_CUSTOM_COMMAND( TARGET daiex POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.dll ${CMAKE_BINARY_DIR}/app/${CMAKE_BUILD_TYPE}/daiex.dll )
  ADD_CUSTOM_COMMAND( TARGET daiex POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.dll ${CMAKE_BINARY_DIR}/test/${CMAKE_BUILD_TYPE}/daiex.dll )
ELSEIF (APPLE)
  TARGET_LINK_LIBRARIES( daiex
    /usr/local/opt/libarchive/lib/libarchive.dylib
    #LIBARCHIVE::LIBARCHIVE
    #hdf5-static
    uuid
    matio 
    iqxformat)

ELSEIF( UNIX )

  TARGET_LINK_LIBRARIES( daiex
    LIBARCHIVE::LIBARCHIVE
    ZLIB::ZLIB
    hdf5-static
    uuid
    libmatio 
	iqxformat)

  # set warning level
  SET_PROPERTY( TARGET daiex APPEND_STRING PROPERTY COMPILE_FLAGS " -Wall")

ENDIF()
    

### installer ###

#IF( WIN32 )
  install( TARGETS daiex 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

  install( FILES 
    include/dataimportexport.h
    include/idataimportexport.h
    include/itempdir.h
    include/aid.h
    include/iqw.h
    include/iqx.h
    include/wv.h
    include/WvScramblerBase.h
    include/iqtar.h
    include/iqmatlab.h
    include/iqcsv.h
    include/iarrayselector.h
    include/icsvselector.h
    include/filetypeservice.h
    include/errorcodes.h
    include/enums.h 
    include/exportdecl.h
    include/enums.h
    include/channelinfo.h
    include/settings.h
    DESTINATION include)
IF( WIN32 )
  if( ${CMAKE_BUILD_TYPE} MATCHES "Debug" )
    install( FILES ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.pdb DESTINATION bin )
  endif()
  install( FILES ${CMAKE_BINARY_DIR}/lib/doc/gen/LibDataImportExport.chm DESTINATION doc )
ENDIF()
  install( FILES ${CMAKE_SOURCE_DIR}/lib/doc/Inventory.xml DESTINATION doc )
  install( FILES ${CMAKE_SOURCE_DIR}/lib/doc/Inventory.xsd DESTINATION doc )
  install( FILES ${CMAKE_SOURCE_DIR}/lib/doc/Inventory_readme.txt DESTINATION doc )
  install( FILES ${CMAKE_SOURCE_DIR}/lib/doc/ReleaseNotes.txt DESTINATION doc )

  #also add modified open-source software -> products has to archive changes in PDM
  install( FILES ${CMAKE_SOURCE_DIR}/3rdparty/mmf/memory_mapped_file.cpp DESTINATION doc/contrib/mmf )
  install( FILES ${CMAKE_SOURCE_DIR}/3rdparty/mmf/memory_mapped_file.hpp DESTINATION doc/contrib/mmf )
  install( FILES ${CMAKE_SOURCE_DIR}/3rdparty/mmf/LICENSE.md DESTINATION doc/contrib/mmf )
  install( FILES ${CMAKE_SOURCE_DIR}/3rdparty/mmf/README.md DESTINATION doc/contrib/mmf )
#ENDIF()
