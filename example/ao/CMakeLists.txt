
# =================================================SSAO===================================================================================

set(ssao_Source_Files ssao.cpp)
add_executable(ssao ${ssao_Source_Files})
add_dependencies(ssao ${engineName})
source_group("Source Files" FILES ${ssao_Source_Files})
set_target_properties(ssao PROPERTIES PROJECT_LABEL "ssao")						# in Visual Studio
set_target_properties(ssao PROPERTIES OUTPUT_NAME "ssao")
set_target_properties(ssao PROPERTIES FOLDER "examples/ao")
target_link_libraries(ssao ${engineName}) 
 
#install
install(TARGETS ssao RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)




