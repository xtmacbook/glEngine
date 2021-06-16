
# ================================================= deferred===================================================================================

set(deferred_Source_Files deferred.cpp)
add_executable(deferred ${deferred_Source_Files})
add_dependencies(cubemap ${engineName})
source_group("Source Files" FILES ${deferred_Source_Files})
set_target_properties(deferred PROPERTIES PROJECT_LABEL "deferred")						# in Visual Studio
set_target_properties(deferred PROPERTIES OUTPUT_NAME "deferred")
set_target_properties(deferred PROPERTIES FOLDER "examples/light")
target_link_libraries(deferred ${engineName}) 
 
#install
install(TARGETS deferred RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)




