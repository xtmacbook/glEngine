
# ================================================= image_Synthesizer ===================================================================================

set(image_Synthesizer_Source_Files image_Synthesizer.cpp perlinNoise.cpp)
add_executable(image_Synthesizer ${image_Synthesizer_Source_Files})
add_dependencies(image_Synthesizer ${engineName})
source_group("Source Files" FILES ${image_Synthesizer_Source_Files})
set_target_properties(image_Synthesizer PROPERTIES PROJECT_LABEL "image_Synthesizer")						# in Visual Studio
set_target_properties(image_Synthesizer PROPERTIES OUTPUT_NAME "image_Synthesizer")
set_target_properties(image_Synthesizer PROPERTIES FOLDER "examples/noise")
target_link_libraries(image_Synthesizer ${engineName}) 
#install
install(TARGETS image_Synthesizer RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# ================================================= 1D Noise ===================================================================================

set(1D_noise_Source_Files 1D_noise.cpp)
add_executable(1D_noise ${1D_noise_Source_Files})
add_dependencies(1D_noise ${engineName})
source_group("Source Files" FILES ${1D_noise_Source_Files})
set_target_properties(1D_noise PROPERTIES PROJECT_LABEL "1D_noise")						# in Visual Studio
set_target_properties(1D_noise PROPERTIES OUTPUT_NAME "1D_noise")
set_target_properties(1D_noise PROPERTIES FOLDER "examples/noise")
target_link_libraries(1D_noise ${engineName}) 
#install
install(TARGETS 1D_noise RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= Worley Noise ===================================================================================

set(Worley_noise_Source_Files Worley_noise.cpp worley.cpp worley.h)
add_executable(Worley_noise ${Worley_noise_Source_Files})
add_dependencies(Worley_noise ${engineName})
source_group("Source Files" FILES ${Worley_noise_Source_Files})
set_target_properties(Worley_noise PROPERTIES PROJECT_LABEL "Worley_noise")						# in Visual Studio
set_target_properties(Worley_noise PROPERTIES OUTPUT_NAME "Worley_noise")
set_target_properties(Worley_noise PROPERTIES FOLDER "examples/noise")
target_link_libraries(Worley_noise ${engineName}) 
#install
install(TARGETS Worley_noise RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= noise_patterns ===================================================================================
set(noise_patterns_Source_Files noise_patterns.cpp)
add_executable(noise_patterns ${noise_patterns_Source_Files})
add_dependencies(noise_patterns ${engineName})
source_group("Source Files" FILES ${noise_patterns_Source_Files})
set_target_properties(noise_patterns PROPERTIES PROJECT_LABEL "noise_patterns")						# in Visual Studio
set_target_properties(noise_patterns PROPERTIES OUTPUT_NAME "noise_patterns")
set_target_properties(noise_patterns PROPERTIES FOLDER "examples/noise")
target_link_libraries(noise_patterns ${engineName}) 
#install
install(TARGETS noise_patterns RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= PerlinNoise  ===================================================================================
set(perlinNoise_Source_Files perlinNoise.cpp perlineNoise.h testPerlinNoise.cpp)
add_executable(perlinNoise ${perlinNoise_Source_Files})
add_dependencies(perlinNoise ${engineName})
source_group("Source Files" FILES ${perlinNoise_Source_Files})
set_target_properties(perlinNoise PROPERTIES PROJECT_LABEL "perlinNoise")						# in Visual Studio
set_target_properties(perlinNoise PROPERTIES OUTPUT_NAME "perlinNoise")
set_target_properties(perlinNoise PROPERTIES FOLDER "examples/noise")
target_link_libraries(perlinNoise ${engineName}) 
#install
install(TARGETS perlinNoise RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
  




