
set (engineName "kangaroo")

set(libengines_base_files
	${Engine_SOURCE_DIR}/OpenGL/src/base/image.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/base/log.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/base/rgbe.c
	${Engine_SOURCE_DIR}/OpenGL/src/base/synchron.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/base/thread_win.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/base/blockDXT.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/base/fence.cpp  
	${Engine_SOURCE_DIR}/OpenGL/src/base/filter.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/base/debug.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/base/str.cpp 
	
	${Engine_SOURCE_DIR}/OpenGL/include/base/image.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/color.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/BaseObject.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/callback.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/exception.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/log.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/smartPointer.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/boundingBox.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/rgbe.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/half.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/plane.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/synchron.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/messQue.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/thread.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/blockDXT.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/colorBlock.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/fence.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/filter.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/dataQueue.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/debug.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/str.h
	
)

set(libengines_math_files
	${Engine_SOURCE_DIR}/OpenGL/include/math/helpF.h
	${Engine_SOURCE_DIR}/OpenGL/include/math/type.h
	${Engine_SOURCE_DIR}/OpenGL/include/math/alg.h
	${Engine_SOURCE_DIR}/OpenGL/include/math/occlusion.h
	${Engine_SOURCE_DIR}/OpenGL/src/math/occlusion.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/math/alg.cpp
)

set(libengines_base_io_files
	${Engine_SOURCE_DIR}/OpenGL/include/io/engineLoad.h
	${Engine_SOURCE_DIR}/OpenGL/include/io/imgLoad.h
	${Engine_SOURCE_DIR}/OpenGL/include/io/meshLoad.h
	${Engine_SOURCE_DIR}/OpenGL/include/io/stream.h
	${Engine_SOURCE_DIR}/OpenGL/src/io/engineLoad.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/imgLoad.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/ddsImage.cpp
	
	${Engine_SOURCE_DIR}/OpenGL/src/io/ddsImage.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/bmpImage.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/hdrImage.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/jpgImage.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/ktxImage.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/pngImage.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/ddsImage.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/tgaImage.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/openExrImage.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/meshLoad.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/io/assimpLoad.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/io/stream.cpp
)

set(libengines_base_compiler_files
	${Engine_SOURCE_DIR}/OpenGL/include/base/compiler/gCCspecific.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/compiler/clangspecific.h
	${Engine_SOURCE_DIR}/OpenGL/include/base/compiler/mSVCspecific.h
)

set(libengines_geometry_files
	${Engine_SOURCE_DIR}/OpenGL/src/geometry/geometry.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/geometry/mesh.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/geometry/dynamicMesh.cpp 
	${Engine_SOURCE_DIR}/OpenGL/include/geometry/IGeometry.h
	${Engine_SOURCE_DIR}/OpenGL/include/geometry/mesh.h
	${Engine_SOURCE_DIR}/OpenGL/include/geometry/dynamicMesh.h
)

set(libengines_mesh_files
	${Engine_SOURCE_DIR}/OpenGL/src/mesh/baseMesh.cpp 
	${Engine_SOURCE_DIR}/OpenGL/include/mesh/baseMesh.h
)

set(libengines_shader_files
	${Engine_SOURCE_DIR}/OpenGL/include/shader/shader.h
	${Engine_SOURCE_DIR}/OpenGL/src/shader/shader.cpp
)

set(libengines_camera_files
	${Engine_SOURCE_DIR}/OpenGL/include/camera/cameraBase.h
	${Engine_SOURCE_DIR}/OpenGL/src/camera/cameraBase.cpp
	${Engine_SOURCE_DIR}/OpenGL/include/camera/camera.h
	${Engine_SOURCE_DIR}/OpenGL/src/camera/camera.cpp
	${Engine_SOURCE_DIR}/OpenGL/include/camera/fspCamera.h
	${Engine_SOURCE_DIR}/OpenGL/src/camera/fspCamera.cpp
	
)

set(libengines_scene_terrain_files
	${Engine_SOURCE_DIR}/OpenGL/include/scene/terrain/rasterTile.h 
	${Engine_SOURCE_DIR}/OpenGL/include/scene/terrain/rasterLoader.h 
	${Engine_SOURCE_DIR}/OpenGL/include/scene/terrain/rasterUpdater.h 
	${Engine_SOURCE_DIR}/OpenGL/include/scene/terrain/rasterSource.h
	${Engine_SOURCE_DIR}/OpenGL/include/scene/terrain/rasterLevel.h  
	${Engine_SOURCE_DIR}/OpenGL/include/scene/terrain/rasterExtent.h
	${Engine_SOURCE_DIR}/OpenGL/include/scene/terrain/terrainRasterLevel.h 
	
	${Engine_SOURCE_DIR}/OpenGL/src/scene/terrain/rasterTile.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/scene/terrain/rasterLoader.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/scene/terrain/rasterUpdater.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/scene/terrain/rasterSource.cpp  
	${Engine_SOURCE_DIR}/OpenGL/src/scene/terrain/rasterLevel.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/scene/terrain/terrainRasterLevel.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/scene/terrain/rasterExtent.cpp
)

set(libengines_scene_geo_clip_terrain_files
	${Engine_SOURCE_DIR}/OpenGL/include/scene/geomClippMap/terrainClipLevel.h
	${Engine_SOURCE_DIR}/OpenGL/src/scene/geomClippMap/terrainClipLevel.cpp
	${Engine_SOURCE_DIR}/OpenGL/include/scene/geomClippMap/footPrints.h
	${Engine_SOURCE_DIR}/OpenGL/src/scene/geomClippMap/footPrints.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/scene/geomClippMap/footPrintsRender.cpp
	${Engine_SOURCE_DIR}/OpenGL/include/scene/geomClippMap/terrainClipUpdater.h
	${Engine_SOURCE_DIR}/OpenGL/src/scene/geomClippMap/terrainClipUpdater.cpp
)

set(libengines_scene_files
	${Engine_SOURCE_DIR}/OpenGL/include/scene/dataQequest.h
)

set(libengines_render_files
	${Engine_SOURCE_DIR}/OpenGL/include/render/IRenderNode.h
	${Engine_SOURCE_DIR}/OpenGL/include/render/renderNode.h
	${Engine_SOURCE_DIR}/OpenGL/include/render/renderMesh.h
	${Engine_SOURCE_DIR}/OpenGL/src/render/IRenderNode.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/render/renderNode.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/render/renderMesh.cpp
)

set(libengines_files
	${Engine_SOURCE_DIR}/OpenGL/src/baseScene.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/bufferObj.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/freetypeText.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/gLApplication.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/windowManager.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/window_w32.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/gLQuery.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/inputmanager.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/scene.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/text.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/texture.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/timerManager.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/resource.cpp
	${Engine_SOURCE_DIR}/OpenGL/src/gbuffer.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/light.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/axes.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/common.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/gli.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/wgl_context.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/win32_platform.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/window.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/framebuffers.cpp 
	${Engine_SOURCE_DIR}/OpenGL/src/stateSet.cpp
	
	
	${Engine_SOURCE_DIR}/OpenGL/src/color/antialias.cpp

	${Engine_SOURCE_DIR}/OpenGL/src/media/ad.cpp
)

set(libengines_include
	${Engine_SOURCE_DIR}/OpenGL/include/baseScene.h
	${Engine_SOURCE_DIR}/OpenGL/include/bufferObj.h
	${Engine_SOURCE_DIR}/OpenGL/include/flashBuffer.h
	${Engine_SOURCE_DIR}/OpenGL/include/gLApplication.h
	${Engine_SOURCE_DIR}/OpenGL/include/windowManager.h
	${Engine_SOURCE_DIR}/OpenGL/include/window_w32.h
	${Engine_SOURCE_DIR}/OpenGL/include/glQuery.h
	${Engine_SOURCE_DIR}/OpenGL/include/inputmanager.h
	${Engine_SOURCE_DIR}/OpenGL/include/resource.h
	${Engine_SOURCE_DIR}/OpenGL/include/scene.h
	${Engine_SOURCE_DIR}/OpenGL/include/sys.h
	${Engine_SOURCE_DIR}/OpenGL/include/text.h
	${Engine_SOURCE_DIR}/OpenGL/include/texture.h
	${Engine_SOURCE_DIR}/OpenGL/include/timerManager.h
	${Engine_SOURCE_DIR}/OpenGL/include/decl.h
	${Engine_SOURCE_DIR}/OpenGL/include/gbuffer.h
	${Engine_SOURCE_DIR}/OpenGL/include/light.h
	${Engine_SOURCE_DIR}/OpenGL/include/axes.h
	${Engine_SOURCE_DIR}/OpenGL/include/gli.h
	${Engine_SOURCE_DIR}/OpenGL/include/gls.h
	${Engine_SOURCE_DIR}/OpenGL/include/wgl_context.h
	${Engine_SOURCE_DIR}/OpenGL/include/glinter.h
	${Engine_SOURCE_DIR}/OpenGL/include/win32_platform.h
	${Engine_SOURCE_DIR}/OpenGL/include/glu.h
	${Engine_SOURCE_DIR}/OpenGL/include/vertex.h
	${Engine_SOURCE_DIR}/OpenGL/include/framebuffers.h

	${Engine_SOURCE_DIR}/OpenGL/include/color/antialias.h
	${Engine_SOURCE_DIR}/OpenGL/include/common.h
	${Engine_SOURCE_DIR}/OpenGL/include/util.h
	${Engine_SOURCE_DIR}/OpenGL/include/stateSet.h
	
	${Engine_SOURCE_DIR}/OpenGL/include/media/ad.h
	
)

				
 if(SHARED_LIBS)
	ADD_DEFINITIONS(-DENGINE_DLLS)
 endif()
  
add_library(${engineName} ${SHARED_OR_STATIC}  
						${libengines_files} 
						${libengines_include} 
						${libengines_base_files}
						${libengines_base_compiler_files}
						${libengines_geometry_files}
						${libengines_mesh_files}
						${libengines_base_io_files}
						${libengines_math_files}
						${libengines_render_files}
						${libengines_shader_files}
						${libengines_camera_files}
						${libengines_scene_terrain_files}
						${libengines_scene_geo_clip_terrain_files}
						)


source_group("Header Files\\\\compiler" FILES ${libengines_base_compiler_files})
source_group("Header Files\\\\math" FILES ${libengines_math_files})
source_group("Source Files\\\\base" FILES ${libengines_base_files})
source_group("Source Files\\\\geometry" FILES ${libengines_geometry_files})
source_group("Source Files\\\\stream" FILES ${libengines_base_io_files})
source_group("Source Files\\\\render" FILES ${libengines_render_files})
source_group("Source Files\\\\mesh" FILES ${libengines_mesh_files})
source_group("Source Files\\\\camera" FILES ${libengines_camera_files})
source_group("Source Files\\\\scene\\\\terrain" FILES ${libengines_scene_terrain_files})
source_group("Source Files\\\\scene\\\\terrain\\\\geoClip" FILES ${libengines_scene_geo_clip_terrain_files})



if(GLFW_FOUND) 
	target_link_libraries(${engineName} ${GLFW_LIBRARY})
	target_include_directories(libengine PUBLIC ${GLFW_INCLUDE_PATH})
endif()
if(GLEW_FOUND) 
	target_link_libraries(${engineName} ${GLEW_LIBRARY})
	target_include_directories(${engineName} PUBLIC ${GLEW_INCLUDE_PATH})
endif()
if(JPEG_FOUND)
	target_link_libraries(${engineName} ${JPEG_LIBRARIES})
	target_include_directories(${engineName} PUBLIC ${JPEG_INCLUDE_DIR})
	ADD_DEFINITIONS(-DENGINE_JPEG)
endif()
if(PNG_FOUND)
	target_link_libraries(${engineName} ${PNG_LIBRARIES})
	target_include_directories(${engineName} PUBLIC ${PNG_PNG_INCLUDE_DIR})
	ADD_DEFINITIONS(-DENGINE_PNG)
endif()
if(ASSIMP_FOUND)
	target_link_libraries(${engineName} ${ASSIMP_LIBRARY})
	target_include_directories(${engineName} PUBLIC ${ASSIMP_INCLUDE_PATH})
	ADD_DEFINITIONS(-DENGINE_ASSIMP)
endif()
if(ZLIB_FOUND)
	target_link_libraries(${engineName} ${ZLIB_LIBARY_RELEASE})
	target_include_directories(${engineName} PUBLIC ${ZLIB_INCLUDE_DIR})
endif()
if(WIN32)
target_link_libraries(${engineName} D:/3rdlib/mesa20.0.5.lib/lib/opengl32.lib)
endif()

# freetype
target_link_libraries(${engineName} ${FREETYPE_LIBRARY})
target_include_directories(${engineName} PUBLIC ${FREETYPE_INCLUDE_DIR_freetype2})

if(APPLE)
target_link_libraries(${engineName} "-framework OpenGL")
endif()

#fmod
if(FMOD_FOUND)
    if(APPLE)
        target_include_directories(${engineName} PRIVATE ${FMOD_INCLUDE_PATH})
        target_include_directories(${engineName} PRIVATE ${FMOD_STUDIO_PATH})
        target_link_libraries(${engineName}  ${FMOD_LIBRARIES})
        ADD_DEFINITIONS(-DFMOD_HAVE)
    endif()
endif()

if(WIN32)
if(USE_MESA3D)
	SET(OPENGL32_LIBRARY "something" CACHE PATH "Location of libraries")
	target_link_libraries(${engineName} ${OPENGL32_LIBRARY})
else(USE_MESA3D)
	target_link_libraries(${engineName} opengl32)
endif()
endif()

if(APPLE)
target_link_libraries(${engineName} "-framework OpenGL")
endif()

if(OPENEXR_FOUND)
	if(WIN32)
		target_include_directories(${engineName} PRIVATE ${OPENEXR_INCLUDE_PATHS}) 
        target_link_libraries(${engineName}  ${OPENEXR_LIBRARIES})
		ADD_DEFINITIONS(-DENGINE_OPENEXR)
	endif()
endif()


 #glm
target_include_directories(${engineName} PUBLIC ${Engine_SOURCE_DIR}/extern/)

target_include_directories(${engineName} PUBLIC ${Engine_SOURCE_DIR}/opengl/include
											${Engine_SOURCE_DIR}/opengl/include/base
											${Engine_SOURCE_DIR}/opengl/include/color
											${Engine_SOURCE_DIR}/opengl/include/math
											${Engine_SOURCE_DIR}/opengl/include/media
											${Engine_SOURCE_DIR}/opengl/include/geometry
											${Engine_SOURCE_DIR}/opengl/include/io
											${Engine_SOURCE_DIR}/opengl/include/math
											${Engine_SOURCE_DIR}/opengl/include/shader
											${Engine_SOURCE_DIR}/opengl/include/render
											${Engine_SOURCE_DIR}/opengl/include/mesh
											${Engine_SOURCE_DIR}/opengl/include/camera
											${Engine_SOURCE_DIR}/opengl/include/scene
											${Engine_SOURCE_DIR}/opengl/include/scene/terrain
											${Engine_SOURCE_DIR}/opengl/include/scene/geomClippMap
											
							)

if(MSVC AND SHARED_LIBS)
  target_compile_definitions(${engineName}
    PUBLIC  ENGINE_USE_DLLS
    PRIVATE LIBENGINE_EXPORTS)
endif()


set_target_properties(${engineName} PROPERTIES
    OUTPUT_NAME ${LIB_PREFIX}engine)

install(TARGETS ${engineName} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)    
