
# =================================================ROAM terrains===================================================================================

set(ROAM_Source_Files roam/roam_terrain.cpp
					  roam/Utility.h
					  roam/Utility.cpp
					  roam/Landscape.h
					  roam/Patch.h
					  roam/Roamsimple.cpp )
add_executable(ROAM_tree ${ROAM_Source_Files})
add_dependencies(ROAM_tree ${engineName})
source_group("Source Files" FILES ${deferred_Source_Files})
set_target_properties(ROAM_tree PROPERTIES PROJECT_LABEL "ROAM_tree")						# in Visual Studio
set_target_properties(ROAM_tree PROPERTIES OUTPUT_NAME "ROAM_tree")
set_target_properties(ROAM_tree PROPERTIES FOLDER "examples/terrains")
target_link_libraries(ROAM_tree ${engineName}) 
 
#install
install(TARGETS ROAM_tree RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= Focus 3d terr===================================================================================

set(focus_3dt_Source_Files focus3dterr/terrain.cpp 
							focus3dterr/terrain.h
							focus3dterr/main.cpp
							focus3dterr/brute_force.h
							focus3dterr/brute_force.cpp
							focus3dterr/geomipmapping.h
							focus3dterr/geomipmapping.cpp
							terrainColor.cpp)
add_executable(focus_3dterrain ${focus_3dt_Source_Files})
add_dependencies(focus_3dterrain ${engineName})
source_group("Source Files" FILES ${deferred_Source_Files})
set_target_properties(focus_3dterrain PROPERTIES PROJECT_LABEL "focus_3dterrain")						# in Visual Studio
set_target_properties(focus_3dterrain PROPERTIES OUTPUT_NAME "focus_3dterrain")
set_target_properties(focus_3dterrain PROPERTIES FOLDER "examples/terrains")
target_link_libraries(focus_3dterrain ${engineName}) 
 
#install
install(TARGETS focus_3dterrain RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= GeoMipMapping terr===================================================================================

set(GeoMip_Source_Files  	terrainColor.cpp
							skydom.h
							skydom.cpp
							cloud.h
							cloud.cpp
							geoMipMapping/main.cpp
							geoMipMapping/geoMipMapping.h
							geoMipMapping/geoMipMapping.cpp)
add_executable(geoMip_3dterrain ${GeoMip_Source_Files})
add_dependencies(geoMip_3dterrain ${engineName})
source_group("Source Files" FILES ${deferred_Source_Files})
set_target_properties(geoMip_3dterrain PROPERTIES PROJECT_LABEL "geoMip_3dterrain")						# in Visual Studio
set_target_properties(geoMip_3dterrain PROPERTIES OUTPUT_NAME "geoMip_3dterrain")
set_target_properties(geoMip_3dterrain PROPERTIES FOLDER "examples/terrains")
target_link_libraries(geoMip_3dterrain ${engineName}) 
 
#install
install(TARGETS geoMip_3dterrain RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# ================================================= GPU Tessellation===================================================================================

set(GPU_Tessellation_Source_Files  	gpu_tess.cpp)
add_executable(gpu_tess_3dterrain ${GPU_Tessellation_Source_Files})
add_dependencies(gpu_tess_3dterrain ${engineName})
source_group("Source Files" FILES ${deferred_Source_Files})
set_target_properties(gpu_tess_3dterrain PROPERTIES PROJECT_LABEL "gpu_tess_3dterrain")						# in Visual Studio
set_target_properties(gpu_tess_3dterrain PROPERTIES OUTPUT_NAME "gpu_tess_3dterrain")
set_target_properties(gpu_tess_3dterrain PROPERTIES FOLDER "examples/terrains")
target_link_libraries(gpu_tess_3dterrain ${engineName}) 
 
#install
install(TARGETS gpu_tess_3dterrain RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


# ================================================= geom_clipMap===================================================================================

set(geom_clipMap_Source_Files  	geoClipMap/main.cpp geoClipMap/geoClipMapping.h geoClipMap/geoClipMapping.cpp)
add_executable(geom_clipMap ${geom_clipMap_Source_Files})
add_dependencies(geom_clipMap ${engineName})
source_group("Source Files" FILES ${geom_clipMap_Source_Files})
set_target_properties(geom_clipMap PROPERTIES PROJECT_LABEL "geom_clipMap")						# in Visual Studio
set_target_properties(geom_clipMap PROPERTIES OUTPUT_NAME "geom_clipMap")
set_target_properties(geom_clipMap PROPERTIES FOLDER "examples/terrains")
target_link_libraries(geom_clipMap ${engineName}) 
 
#install
install(TARGETS geom_clipMap RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


# ================================================= chunklod===================================================================================

set(chunklod_Source_Files  	    chunklod/main.cpp 
								)
add_executable(chunklodMap ${chunklod_Source_Files})
add_dependencies(chunklodMap ${engineName})
source_group("Source Files" FILES ${chunklod_Source_Files})
set_target_properties(chunklodMap PROPERTIES PROJECT_LABEL "chunklodMap")						# in Visual Studio
set_target_properties(chunklodMap PROPERTIES OUTPUT_NAME "chunklodMap")
set_target_properties(chunklodMap PROPERTIES FOLDER "examples/terrains")
target_link_libraries(chunklodMap ${engineName}) 
 
#install
install(TARGETS chunklodMap RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
