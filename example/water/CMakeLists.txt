# ================================================= Gertner water  =================================================
add_executable(GertnerWave "gertnerWave.cpp"
							"wave.cpp"
							"wave.h"
							"water.cpp"
							"water.h"
							"${EXTERN_DIR}/json/json.h"
							"${EXTERN_DIR}/json/json.cpp"
							"${EXTERN_DIR}/json/json-forwards.h")
add_dependencies(GertnerWave ${engineName})
source_group("Source Files" FILES "gertnerWave.cpp")

set_target_properties(GertnerWave PROPERTIES PROJECT_LABEL "GertnerWave")						# in Visual Studio
set_target_properties(GertnerWave PROPERTIES OUTPUT_NAME "GertnerWave")
set_target_properties(GertnerWave PROPERTIES FOLDER "examples/water")

if(APPLE)
    target_link_libraries(GertnerWave "-framework OpenGL")
endif()
target_include_directories(GertnerWave PRIVATE ${EXTERN_DIR}/json)

target_link_libraries(GertnerWave ${engineName})
target_include_directories(GertnerWave PRIVATE  ${EXTERN_DIR}/imgui/)
install(TARGETS GertnerWave RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

