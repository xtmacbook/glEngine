#include<resource.h> 

static std::string g_mediaBasePath  = "D:/workspace/xtmacbook/glEngine/media/"; 
static std::string g_shaderBasePath = g_mediaBasePath + "shader/"; 

static std::string g_textureBasePath = g_mediaBasePath + "textures/"; 
static std::string g_objsBasePath    = g_mediaBasePath + "models/";

static std::string g_log_file_name = "D:/workspace/xtmacbook/glEngine/log.txt"; 

std::string get_media_BasePath(){return g_mediaBasePath;}
std::string get_shader_BasePath(){return g_shaderBasePath;}
std::string get_texture_BasePath(){return g_textureBasePath;}
std::string get_model_BasePath(){return g_objsBasePath;}
std::string get_log_file_name(){return g_log_file_name;}