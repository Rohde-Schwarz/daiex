#
# Mosaik.LibdataImportExport/wrapper_test
#

set(PROJNAME daiex_wrapper_test)
set(SRC_FILES
    "src/InterfaceTest.cs"
    "src/SettingsTest.cs"
    "src/AssemblyInfo.cs"
    "src/Common.cs"
)

add_library(${PROJNAME} SHARED 
    ${SRC_FILES} 
)


target_compile_definitions(${PROJNAME} PRIVATE _UNICODE UNICODE)
target_include_directories(${PROJNAME} PUBLIC include)
target_link_libraries(${PROJNAME} PRIVATE daiex_wrapper)
set_target_properties(${PROJNAME} PROPERTIES VS_GLOBAL_KEYWORD "ManagedCProj")
set_property(TARGET ${PROJNAME} PROPERTY VS_GLOBAL_CLRSupport "true")
set_target_properties(${PROJNAME} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
set_target_properties(${PROJNAME} PROPERTIES VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.0")
set_property(TARGET ${PROJNAME} PROPERTY VS_DOTNET_REFERENCES "System" "System.Core" "System.Xml.Linq" "System.Data.DataSetExtensions" "Microsoft.CSharp" "System.Data" "System.Xml")
set_property(TARGET ${PROJNAME} PROPERTY  VS_PACKAGE_REFERENCES "nunit_3.13.*")