
# ================================================= MSAA   =================================================
add_executable(msaa "msaa.cpp")
add_dependencies(msaa ${engineName})
source_group("Source Files" FILES "msaa.cpp")

set_target_properties(msaa PROPERTIES PROJECT_LABEL "msaa")						# in Visual Studio
set_target_properties(msaa PROPERTIES OUTPUT_NAME "msaa")
set_target_properties(msaa PROPERTIES FOLDER "examples/antialias")

target_link_libraries(msaa ${engineName})
#install
install(TARGETS msaa RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= CSAA   =================================================
add_executable(csaa "csaa.cpp")
add_dependencies(csaa ${engineName})
source_group("Source Files" FILES "csaa.cpp")

set_target_properties(csaa PROPERTIES PROJECT_LABEL "csaa")						# in Visual Studio
set_target_properties(csaa PROPERTIES OUTPUT_NAME "csaa")
set_target_properties(csaa PROPERTIES FOLDER "examples/antialias")

target_link_libraries(csaa ${engineName})
#install
install(TARGETS csaa RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
