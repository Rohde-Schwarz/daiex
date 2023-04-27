#
# Mosaik.LibdataImportExport/wrapper
#

set(SRC_FILES
    src/assemblyInfo.cpp
    src/channelinfo_wrapper.cpp
    src/dataimportexportbase_wrapper.cpp
    src/filetypeservice_wrapper.cpp
    src/helpers.cpp
    src/iqcsv_wrapper.cpp
    src/iqmatlab_wrapper.cpp
    src/iqtar_wrapper.cpp
    src/iqw_wrapper.cpp
    src/settings_wrapper.cpp
)
set(export_includes
    include/assemblyInfo.h
    include/channelinfo_wrapper.h
    include/dataimportexportbase_wrapper.h
    include/enums_wrapper.h
    include/exceptions.h
    include/filetypeservice_wrapper.h
    include/helpers.h
    include/iarrayselector_wrapper.h
    include/icsvselector_wrapper.h
    include/idataimportexport_wrapper.h
    include/iqcsv_wrapper.h
    include/iqmatlab_wrapper.h
    include/iqtar_wrapper.h
    include/iqw_wrapper.h
    include/settings_wrapper.h
)

add_library(daiex_wrapper SHARED 
    ${SRC_FILES} 
    ${export_includes}
    libdai_wrapper.rc
)
target_compile_definitions(daiex_wrapper PRIVATE _UNICODE UNICODE)
target_include_directories(daiex_wrapper PUBLIC include)
target_link_libraries(daiex_wrapper PRIVATE daiex)
set_target_properties(daiex_wrapper PROPERTIES VS_GLOBAL_KEYWORD "ManagedCProj")
set_property(TARGET daiex_wrapper PROPERTY VS_GLOBAL_CLRSupport "true")
set_target_properties(daiex_wrapper PROPERTIES COMMON_LANGUAGE_RUNTIME "")
set_target_properties(daiex_wrapper PROPERTIES VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.0")
set_property(TARGET daiex_wrapper PROPERTY VS_DOTNET_REFERENCES "System" "System.Data")

### installer ###
install( TARGETS daiex_wrapper )
