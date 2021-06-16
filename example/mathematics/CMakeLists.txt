

# =================================================glm_tutorial===================================================================================

set(glm_tutorial_Source_Files glm_test.cpp)
add_executable(glm_tutorial ${glm_tutorial_Source_Files})
add_dependencies(glm_tutorial ${engineName})
source_group("Source Files" FILES ${glm_tutorial_Source_Files})
set_target_properties(glm_tutorial PROPERTIES PROJECT_LABEL "glm_tutorial")						# in Visual Studio
set_target_properties(glm_tutorial PROPERTIES OUTPUT_NAME "glm_tutorial")
set_target_properties(glm_tutorial PROPERTIES FOLDER "examples/mathematics")
target_link_libraries(glm_tutorial ${engineName}) 
 
#install
install(TARGETS glm_tutorial RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


# =================================================interpolation===================================================================================

set(interpolation_Source_Files interpolation.cpp)
add_executable(interpolation ${interpolation_Source_Files})
add_dependencies(interpolation ${engineName})
source_group("Source Files" FILES ${interpolation_Source_Files})
set_target_properties(interpolation PROPERTIES PROJECT_LABEL "interpolation")						# in Visual Studio
set_target_properties(interpolation PROPERTIES OUTPUT_NAME "interpolation")
set_target_properties(interpolation PROPERTIES FOLDER "examples/mathematics")
target_link_libraries(interpolation ${engineName}) 
 
#install
install(TARGETS interpolation RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)




