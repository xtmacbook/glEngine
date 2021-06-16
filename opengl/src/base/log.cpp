
#include "sys.h"
#include "log.h"
#include "exception.h"
#include "resource.h"
#include "gls.h"
#include <stdarg.h> 

#include <iostream>



#if defined EN_OS_WIN32 || defined EN_OS_WIN64
#pragma comment (lib, "glu32.lib")     /* link with OpenGL Utility lib */
#endif

std::ofstream Log::out_stream = std::ofstream();

Log* Log::pInstance = NULL;

Log::Log()
{
	out_stream.open(get_log_file_name().c_str());
}


int Log::printOglError(char *file, int line)
{
	GLenum glErr;
	int    retCode = 0;
#ifdef _DEBUG
	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		char buffer[256];
		sprintf(buffer, "glError in file %s @ line %d: %s \n", file, line, gluErrorString(glErr));
		std::cout << buffer;
		out_stream << buffer;
		retCode = 1;
		glErr = glGetError();
	}
#endif
	return retCode;
}
void Log::printMessage(const char*message,...)
{
	const int length = 1024;
	char buffer[length];
	va_list ap;
	va_start(ap, message);
	vsnprintf_s(buffer, length - 1, message, ap);
	va_end(ap);

	out_stream << buffer;
	std::cout << buffer << std::endl;
}


void Log::printError(const char*err)
{
	char buffer[128];
	sprintf(buffer, "Error :%s \n ", err);
	out_stream << buffer;
}


void Log::printShaderInfoLog(unsigned int shader)
{
	int infologLength = 0;
	int charsWritten = 0;
	GLchar * infoLog;
	printOglError(__FILE__, __LINE__);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
	printOglError(__FILE__, __LINE__);

	if (infologLength > 0)
	{
		infoLog = (GLchar*)malloc(infologLength);
		if (infoLog == NULL)
		{
			printf("ERROR: Could not allocate InfoLog buffer\n");
			exit(1);
		}
		glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		printf("Shader InfoLog:\n%s\n\n", infoLog);
		free(infoLog);
	}
	printOglError(__FILE__, __LINE__);
}

Log* Log::instance()
{
	if (NULL == pInstance)
	{
		pInstance = new Log();
	}
	return pInstance;

}

void Log::destroy()
{
	out_stream.flush();
	out_stream.close();

	delete pInstance;
	pInstance = NULL;


}

void Log::gLFWError(int error, const char* description)
{
	out_stream << "glfw error: " << description << "\n";
}
