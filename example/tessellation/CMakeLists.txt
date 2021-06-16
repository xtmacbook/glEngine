
# ================================================= tess===================================================================================

set(tess_Source_Files tess.cpp)
add_executable(tess ${tess_Source_Files})
add_dependencies(tess ${engineName})
source_group("Source Files" FILES ${tess_Source_Files})
set_target_properties(tess PROPERTIES PROJECT_LABEL "tess")						# in Visual Studio
set_target_properties(tess PROPERTIES OUTPUT_NAME "tess")
set_target_properties(tess PROPERTIES FOLDER "examples/tessellation")
target_link_libraries(tess ${engineName}) 
 
#install
install(TARGETS tess RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= pn_triangle_tess===================================================================================

set(pn_triangle_tess_Source_Files pn_triangles_tess.cpp)
add_executable(pn_triangle_tess ${pn_triangle_tess_Source_Files})
add_dependencies(pn_triangle_tess ${engineName})
source_group("Source Files" FILES ${pn_triangle_tess_Source_Files})
set_target_properties(pn_triangle_tess PROPERTIES PROJECT_LABEL "pn_triangle_tess")						# in Visual Studio
set_target_properties(pn_triangle_tess PROPERTIES OUTPUT_NAME "pn_triangle_tess")
set_target_properties(pn_triangle_tess PROPERTIES FOLDER "examples/tessellation")
target_link_libraries(pn_triangle_tess ${engineName}) 
 
#install
install(TARGETS pn_triangle_tess RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= pn_triangle_tess_Geo===================================================================================

set(pn_triangle_tess_Geo_Source_Files pn_triangles_tess_geo.cpp )
add_executable(pn_triangle_tess_geo ${pn_triangle_tess_Geo_Source_Files})
add_dependencies(pn_triangle_tess_geo ${engineName})
source_group("Source Files" FILES ${pn_triangle_tess_Geo_Source_Files})
set_target_properties(pn_triangle_tess_geo PROPERTIES PROJECT_LABEL "pn_triangle_tess_geo")						# in Visual Studio
set_target_properties(pn_triangle_tess_geo PROPERTIES OUTPUT_NAME "pn_triangle_tess_geo")
set_target_properties(pn_triangle_tess_geo PROPERTIES FOLDER "examples/tessellation")
target_link_libraries(pn_triangle_tess_geo ${engineName}) 
 
#install
install(TARGETS pn_triangle_tess_geo RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= tess_bezier_curves===================================================================================

set(tess_line_Source_Files tess_bezier_curves.cpp)
add_executable(tess_bezier_curves ${tess_line_Source_Files})
add_dependencies(tess_bezier_curves ${engineName})
source_group("Source Files" FILES ${tess_line_Source_Files})
set_target_properties(tess_bezier_curves PROPERTIES PROJECT_LABEL "tess_bezier_curves")						# in Visual Studio
set_target_properties(tess_bezier_curves PROPERTIES OUTPUT_NAME "tess_bezier_curves")
set_target_properties(tess_bezier_curves PROPERTIES FOLDER "examples/tessellation")
target_link_libraries(tess_bezier_curves ${engineName}) 
 
#install
install(TARGETS tess_bezier_curves RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= tess_bezier_surface===================================================================================

set(tess_bezier_quad_surface_Source_Files tess_bezier_surface.cpp teapot.cpp)
add_executable(tess_bezier_surface ${tess_bezier_quad_surface_Source_Files})
add_dependencies(tess_bezier_surface ${engineName})
source_group("Source Files" FILES ${tess_bezier_quad_surface_Source_Files})
set_target_properties(tess_bezier_surface PROPERTIES PROJECT_LABEL "tess_bezier_surface")						# in Visual Studio
set_target_properties(tess_bezier_surface PROPERTIES OUTPUT_NAME "tess_bezier_surface")
set_target_properties(tess_bezier_surface PROPERTIES FOLDER "examples/tessellation")
target_link_libraries(tess_bezier_surface ${engineName}) 
 
#install
install(TARGETS tess_bezier_surface RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)






