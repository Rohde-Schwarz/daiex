cmake_minimum_required(VERSION 3.0)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)

project(iqxformat VERSION 1.0.0)

if (MSVC)
  add_compile_options(/wd4996 /wd4267)
elseif(APPLE)
  add_compile_options(-Wall -Wpedantic -Werror -Wno-unknown-pragmas -Wno-missing-braces)
else()
  add_compile_options(-Wall -Wpedantic -Werror -Wno-unknown-pragmas)
endif()
add_compile_options("$<$<NOT:$<CONFIG:debug>>:-DNO_DEBUG>")

file(GLOB_RECURSE API_HEADER  ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/*.h ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/*.hpp)
file(GLOB_RECURSE LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.h ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp)
list(APPEND IQXFORMAT_SOURCES ${API_HEADER} ${LIB_SOURCES})

add_library(${PROJECT_NAME} SHARED ${IQXFORMAT_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
    PRIVATE src
)
target_compile_definitions(${PROJECT_NAME} PUBLIC _FILE_OFFSET_BITS=64)
if (MSVC)
  target_compile_definitions(${PROJECT_NAME} PRIVATE EXPORT_MODULE)
  target_link_libraries(${PROJECT_NAME} PRIVATE rpcrt4)
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE uuid)
endif()

add_library(${PROJECT_NAME}static STATIC ${IQXFORMAT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
target_include_directories(${PROJECT_NAME}static PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        PRIVATE src
        )
target_compile_definitions(${PROJECT_NAME}static PUBLIC _FILE_OFFSET_BITS=64)
if (MSVC)
    target_compile_definitions(${PROJECT_NAME}static PRIVATE EXPORT_MODULE)
    target_link_libraries(${PROJECT_NAME}static PRIVATE rpcrt4)
else()
    target_link_libraries(${PROJECT_NAME}static PRIVATE uuid)
    set_target_properties(${PROJECT_NAME}static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
endif()


if (ENABLE_TESTS)
    add_subdirectory(test)
endif()

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}static EXPORT ${PROJECT_NAME}Config LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)
install(EXPORT ${PROJECT_NAME}Config DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME})

export(EXPORT ${PROJECT_NAME}Config  FILE ${PROJECT_NAME}Config.cmake)
export(PACKAGE ${PROJECT_NAME})


if (MSVC)
  macro(GroupSources curdir)
   file(GLOB children RELATIVE ${CMAKE_CURRENT_LIST_DIR}/${curdir} ${CMAKE_CURRENT_LIST_DIR}/${curdir}/*)
   foreach(child ${children})
          if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child})
                GroupSources(${curdir}/${child})
          else()
                string(REPLACE "/" "\\" groupname ${curdir})         
				source_group(${groupname} FILES ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child})
          endif()
   endforeach()
 endmacro()
 GroupSources(src)
 GroupSources(include)
 GroupSources(test)
endif()