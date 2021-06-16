
# ================================================= flashBuffer===================================================================================

set(TFlashBUffer_Source_Files test_flash_buffer.cpp)
add_executable(test_flash_buffer ${TFlashBUffer_Source_Files})
add_dependencies(test_flash_buffer ${engineName})
source_group("Source Files" FILES ${TFlashBUffer_Source_Files})
set_target_properties(test_flash_buffer PROPERTIES PROJECT_LABEL "test_flash_buffer")						# in Visual Studio
set_target_properties(test_flash_buffer PROPERTIES OUTPUT_NAME "test_flash_buffer")
set_target_properties(test_flash_buffer PROPERTIES FOLDER "test")
target_link_libraries(test_flash_buffer ${engineName}) 
 
#install
install(TARGETS test_flash_buffer RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


# ================================================= drawApi===================================================================================

set(drawApi_Source_Files drawApi.cpp)
add_executable(test_drawApi ${drawApi_Source_Files})
add_dependencies(test_drawApi ${engineName})
source_group("Source Files" FILES ${drawApi_Source_Files})
set_target_properties(test_drawApi PROPERTIES PROJECT_LABEL "test_drawApi")						# in Visual Studio
set_target_properties(test_drawApi PROPERTIES OUTPUT_NAME "test_drawApi")
set_target_properties(test_drawApi PROPERTIES FOLDER "test")
target_link_libraries(test_drawApi ${engineName}) 
 
#install
install(TARGETS test_drawApi RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)



# ================================================= multThread===================================================================================

set(multThread_Source_Files multThread.cpp render.h render.cpp)
add_executable(test_multThread ${multThread_Source_Files})
add_dependencies(test_multThread ${engineName})
source_group("Source Files" FILES ${multThread_Source_Files})
set_target_properties(test_multThread PROPERTIES PROJECT_LABEL "test_multThread")						# in Visual Studio
set_target_properties(test_multThread PROPERTIES OUTPUT_NAME "test_multThread")
set_target_properties(test_multThread PROPERTIES FOLDER "test")
target_link_libraries(test_multThread ${engineName}) 
 
#install
install(TARGETS test_multThread RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# ================================================= test_JPeg_decoding_on_GPU===================================================================================
set(testImage_Source_Files   testImage.cpp jpgd.cpp jpeg_gpu.cpp)
add_executable(testImage ${testImage_Source_Files})
add_dependencies(testImage ${engineName})
source_group("Source Files" FILES ${testImage_Source_Files})
set_target_properties(testImage PROPERTIES PROJECT_LABEL "testJPeg_decoding_on_GPU")						# in Visual Studio
set_target_properties(testImage PROPERTIES OUTPUT_NAME "testJPeg_decoding_on_GPU")
set_target_properties(testImage PROPERTIES FOLDER "test")
target_link_libraries(testImage ${engineName}) 
 
#install
install(TARGETS testImage RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# ================================================= testDataQequest===================================================================================
set( testDataQequest_Source_Files    testDataQequest.cpp)
add_executable( testDataQequest ${testDataQequest_Source_Files})
add_dependencies(testDataQequest ${engineName})
source_group("Source Files" FILES ${testDataQequest_Source_Files})
set_target_properties(testDataQequest PROPERTIES PROJECT_LABEL "testDataQequest")						# in Visual Studio
set_target_properties(testDataQequest PROPERTIES OUTPUT_NAME "testDataQequest")
set_target_properties(testDataQequest PROPERTIES FOLDER "test")
target_link_libraries(testDataQequest ${engineName}) 
 
#install
install(TARGETS testDataQequest RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= testClipmap===================================================================================
set( testClipmap_Source_Files    testClipmap.cpp jpgd.cpp clipmap_prepprocessor.cpp jpeg_gpu.cpp)
add_executable( testClipmap ${testClipmap_Source_Files})
add_dependencies(testClipmap ${engineName})
source_group("Source Files" FILES ${testClipmap_Source_Files})
set_target_properties(testClipmap PROPERTIES PROJECT_LABEL "testClipmap")						# in Visual Studio
set_target_properties(testClipmap PROPERTIES OUTPUT_NAME "testClipmap")
set_target_properties(testClipmap PROPERTIES FOLDER "test")
target_link_libraries(testClipmap ${engineName}) 
 
#install
install(TARGETS testClipmap RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
