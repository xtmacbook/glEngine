	
SET(RESOUCE_CPP_FILE_NAME "${Engine_SOURCE_DIR}OpenGL/src/resource.cpp")

	FILE ( WRITE ${RESOUCE_CPP_FILE_NAME} "#include<resource.h> \n")
	
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "\n")
    FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "static std::string g_mediaBasePath  = \"${Engine_SOURCE_DIR}media/\"; \n")
    FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "static std::string g_shaderBasePath = g_mediaBasePath + \"shader/\"; \n")
	
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "\n")
    FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "static std::string g_textureBasePath = g_mediaBasePath + \"textures/\"; \n")
    FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "static std::string g_objsBasePath    = g_mediaBasePath + \"models/\";\n")
	
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "\n")
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "static std::string g_log_file_name = \"${Engine_SOURCE_DIR}log.txt\"; \n")
	
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "\n") 
	
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "std::string get_media_BasePath(){return g_mediaBasePath;}")
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "\n") 
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "std::string get_shader_BasePath(){return g_shaderBasePath;}")
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "\n") 
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "std::string get_texture_BasePath(){return g_textureBasePath;}")
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "\n") 
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "std::string get_model_BasePath(){return g_objsBasePath;}")
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "\n") 
	FILE ( APPEND ${RESOUCE_CPP_FILE_NAME} "std::string get_log_file_name(){return g_log_file_name;}")
	
