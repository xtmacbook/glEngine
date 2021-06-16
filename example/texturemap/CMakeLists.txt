# ================================================= texturemap   =================================================
add_executable(texturemap "localScene.cpp" "localScene.h")
add_dependencies(texturemap ${engineName})
source_group("Source Files" FILES  "localScene.cpp" "localScene.h")

set_target_properties(texturemap PROPERTIES PROJECT_LABEL "texturemap")						# in Visual Studio
set_target_properties(texturemap PROPERTIES OUTPUT_NAME "texturemap")
set_target_properties(texturemap PROPERTIES FOLDER "examples/texturemap")

target_link_libraries(texturemap ${engineName})
#install
install(TARGETS texturemap RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= cubemap===================================================================================
add_executable(cubemap "cubemap.cpp")
add_dependencies(cubemap ${engineName})
source_group("Source Files" FILES "cubemap.cpp")

set_target_properties(cubemap PROPERTIES PROJECT_LABEL "cubemap")						# in Visual Studio
set_target_properties(cubemap PROPERTIES OUTPUT_NAME "cubemap")
set_target_properties(cubemap PROPERTIES FOLDER "examples/texturemap")

target_link_libraries(cubemap ${engineName})
#install
install(TARGETS cubemap RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

if(G3D_FOUND) 
# ================================================= Skylands===================================================================================

set(skylandslib_Source_Files
  skylands.cpp
  skylands.h
  VVDoF.cpp
  VVDoF.h
)
add_executable(skyland ${skylandslib_Source_Files})
#add_dependencies(skyland )
source_group("Source Files" FILES ${skylandslib_Source_Files})

target_include_directories(skyland PUBLIC ${G3D_INCLUDE_PATH})

set_target_properties(skyland PROPERTIES PROJECT_LABEL "skylands")						# in Visual Studio
set_target_properties(skyland PROPERTIES OUTPUT_NAME "skylands")
set_target_properties(skyland PROPERTIES FOLDER "examples/texturemap")

target_link_libraries(skyland ${G3D_LIBRARY})
target_link_libraries(skyland )
#install
install(TARGETS skyland RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()



# ================================================= Procedural texture===================================================================================

set(procedural_texture_Source_Files
  procedural_texture.cpp
)
add_executable(procedural_texture ${procedural_texture_Source_Files})
add_dependencies(texturemap ${engineName})
source_group("Source Files" FILES ${procedural_texture_Source_Files})
set_target_properties(procedural_texture PROPERTIES PROJECT_LABEL "procedural_texture")						# in Visual Studio
set_target_properties(procedural_texture PROPERTIES OUTPUT_NAME "procedural_texture")
set_target_properties(procedural_texture PROPERTIES FOLDER "examples/texturemap")

target_link_libraries(procedural_texture ${engineName})
install(TARGETS procedural_texture RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

