
# ================================================= multriDrawIndirect===================================================================================

set(multDrawIndirect_Source_Files multriDrawIndirect.cpp)
add_executable(multriDrawIndirect ${multDrawIndirect_Source_Files})
add_dependencies(multriDrawIndirect ${engineName})
source_group("Source Files" FILES ${deferred_Source_Files})
set_target_properties(multriDrawIndirect PROPERTIES PROJECT_LABEL "multriDrawIndirect")						# in Visual Studio
set_target_properties(multriDrawIndirect PROPERTIES OUTPUT_NAME "multriDrawIndirect")
set_target_properties(multriDrawIndirect PROPERTIES FOLDER "examples/optimization")
target_link_libraries(multriDrawIndirect ${engineName}) 
 
#install
install(TARGETS multriDrawIndirect RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


# ================================================= NvmultriDrawIndirect===================================================================================

set(NvmultriDrawIndirect_Source_Files NvmultriDrawIndirect.cpp)
add_executable(NvmultriDrawIndirect ${NvmultriDrawIndirect_Source_Files})
add_dependencies(NvmultriDrawIndirect ${engineName})
source_group("Source Files" FILES ${deferred_Source_Files})
set_target_properties(NvmultriDrawIndirect PROPERTIES PROJECT_LABEL "NvmultriDrawIndirect")						# in Visual Studio
set_target_properties(NvmultriDrawIndirect PROPERTIES OUTPUT_NAME "NvmultriDrawIndirect")
set_target_properties(NvmultriDrawIndirect PROPERTIES FOLDER "examples/optimization")
target_link_libraries(NvmultriDrawIndirect ${engineName}) 
 
#install
install(TARGETS NvmultriDrawIndirect RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)





