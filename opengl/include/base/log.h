//
//  log.h
//  openGLTest
//
//  Created by xt on 18/7/6.
//  Copyright © 2018年 xt. All rights reserved.
//

#ifndef log_h
#define log_h

#include "decl.h"
#include <fstream>
class LIBENIGHT_EXPORT Log
{

private:
    static Log* pInstance;
    Log();
public:

    static int printOglError(char *file, int line);

        //just print message
	static void printMessage(const char*message,...);

	static void printError(const char*err);

        //shader error
    static void printShaderInfoLog(unsigned int shader);

    static Log* instance();

    static void destroy();

    static void gLFWError(int error, const char* description);

    static std::ofstream out_stream;
};

#define CHECK_GL_ERROR Log::printOglError(__FILE__, __LINE__)
#define PRINT_ERROR(E) Log::printMessage(E)
#define PRINT_MSG(M) Log::printMessage(M)
#define LOGI(...)  Log::printMessage(__VA_ARGS__)
#define LOGE(...)  Log::printMessage(__VA_ARGS__)


#endif /* log_h */
