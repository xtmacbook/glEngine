#ifndef __OpenGL__Resource__ 
 #define __OpenGL__Resource__

#include <string>
#include "decl.h"

LIBENIGHT_EXPORT std::string get_media_BasePath();
LIBENIGHT_EXPORT std::string get_shader_BasePath();
LIBENIGHT_EXPORT std::string get_texture_BasePath() ; 
LIBENIGHT_EXPORT std::string get_model_BasePath();
LIBENIGHT_EXPORT std::string get_log_file_name() ;


#endif
