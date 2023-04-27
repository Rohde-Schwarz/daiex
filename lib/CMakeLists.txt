#
# Mosaik.LibdataImportExport/lib
#

add_subdirectory(iqxformat) 
FIND_PACKAGE( LibArchive REQUIRED )
if (APPLE)
else()
FIND_PACKAGE( ZLIB REQUIRED )
FIND_PACKAGE( HDF5 REQUIRED )
endif()
FIND_PACKAGE( matio REQUIRED )
FIND_PACKAGE( pugixml REQUIRED )

### doxygen configuration start ###

FIND_PACKAGE( Doxygen )
FIND_PACKAGE( HTMLHelp )
if ( BUILD_DOC )
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
  ELSE()
      message(FATAL_ERROR "Doc build requested but doxygen not found!")
  ENDIF( DOXYGEN )
ENDIF(BUILD_DOC)

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

SET( ALL_COMMON
  ${SOURCES}
  ${SOURCES_PLATFORM}
  ${AID_SOURCES}
  ${INCLUDES}
  ${AID_INCLUDES}
  ${MMF}
   )

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
  ${CMAKE_SOURCE_DIR}/lib/iqxformat/include/ )

TARGET_INCLUDE_DIRECTORIES( daiex PUBLIC include PRIVATE ${INC_DIRS} )

IF( WIN32 )

  TARGET_COMPILE_DEFINITIONS( daiex
    PRIVATE EXPORT_MODULE )

  TARGET_LINK_LIBRARIES( daiex PRIVATE
    LibArchive::LibArchive
    ZLIB::ZLIB
    HDF5::C
    pugixml::pugixml
    matio::matio
    Rpcrt4.lib
	  iqxformatstatic)

  # set warning level
  SET_PROPERTY( TARGET daiex APPEND_STRING PROPERTY COMPILE_FLAGS " /W4")

  # copy dll
  ADD_CUSTOM_COMMAND( TARGET daiex POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.dll ${CMAKE_BINARY_DIR}/app/${CMAKE_BUILD_TYPE}/daiex.dll )
  ADD_CUSTOM_COMMAND( TARGET daiex POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.dll ${CMAKE_BINARY_DIR}/test/${CMAKE_BUILD_TYPE}/daiex.dll )
ELSEIF (APPLE)
  TARGET_LINK_LIBRARIES( daiex PRIVATE
    /usr/local/opt/libarchive/lib/libarchive.dylib
    #LibArchive::LibArchive
    #HDF5::C
    pugixml::pugixml
    uuid
    matio 
    iqxformat)

ELSEIF( UNIX )

  TARGET_LINK_LIBRARIES( daiex PRIVATE
    LibArchive::LibArchive
    ZLIB::ZLIB
    HDF5::C
    pugixml::pugixml
    uuid
    matio::matio
    iqxformat)

  # set warning level
  SET_PROPERTY( TARGET daiex APPEND_STRING PROPERTY COMPILE_FLAGS " -Wall")

ENDIF()
    

### installer ###

set(export_includes
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
)
set_target_properties(daiex PROPERTIES PUBLIC_HEADER "${export_includes}")

#IF( WIN32 )
  install( TARGETS daiex 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

IF( WIN32 )
  if( ${CMAKE_BUILD_TYPE} MATCHES "Debug" )
    install( FILES ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/daiex.pdb DESTINATION bin )
  endif()
  IF ( BUILD_DOC )
    install( FILES ${CMAKE_BINARY_DIR}/lib/doc/gen/LibDataImportExport.chm DESTINATION doc )
  ENDIF()
ELSE()
  # install( FILES ${CMAKE_BINARY_DIR}/lib/liblibmatio.so DESTINATION lib )
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
