
#include "shader.h"
#include "camera.h"
#include "log.h"
#include "resource.h"
#include "gls.h"
#include <algorithm>


#include <algorithm>


Shader::Shader(ShaderType type) :type_(type)
{
	shaderProgramId_ = glCreateProgram();
}

Shader::Shader(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path /*= NULL*/) :
	Shader((ShaderType)(DEFAULT | ((geometry_file_path == NULL) ? NONE : GEOMETRY)))
{
	loadShaders(vertex_file_path, fragment_file_path, geometry_file_path);
}

bool getShaderStream(const char* file, std::string& stream, bool log)
{
	std::ifstream VertexShaderStream(file, std::ios::in);

	if (VertexShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			stream += "\n" + Line;
		VertexShaderStream.close();
		return true;
	}
	else if (log)
	{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", file);
		return false;
	}
	else return false;
}


std::string Shader::loadMultShaderInOneFile(const char* file)
{
	std::string code;
	if (!getShaderStream(file, code, false))
	{
		if (!getShaderStream((get_shader_BasePath() + file).c_str(), code, true))
			return false;
	}
	return code;
}

GLuint Shader::loadShaders(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path) {
	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::string FragmentShaderCode;
	std::string GeometryShaderCode;

	if (!getShaderStream(vertex_file_path, VertexShaderCode, false))
	{
		if (!getShaderStream((get_shader_BasePath() + vertex_file_path).c_str(), VertexShaderCode, true)) return -1;
	}

	if (!getShaderStream(fragment_file_path, FragmentShaderCode, false))
	{
		if (!getShaderStream((get_shader_BasePath() + fragment_file_path).c_str(), FragmentShaderCode, true)) return -1;
	}

	if (geometry_file_path != NULL)
	{
		if (!getShaderStream(geometry_file_path, GeometryShaderCode, false))
		{
			if (!getShaderStream((get_shader_BasePath() + geometry_file_path).c_str(), GeometryShaderCode, true)) return -1;
			return loadShaderSouce(VertexShaderCode.c_str(), FragmentShaderCode.c_str(), GeometryShaderCode.c_str());
		}
	}
	return loadShaderSouce(VertexShaderCode.c_str(), FragmentShaderCode.c_str(), NULL);

}

GLuint Shader::loadShaderSouce(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	GLint vsr = GL_FALSE;
	GLint fsr = GL_FALSE;
	GLint gsr = GL_FALSE;
	vertexShaderId_ = loadShaderSource(vertexSource, GL_VERTEX_SHADER, vsr);
	fragmentShaderId_ = loadShaderSource(fragmentSource, GL_FRAGMENT_SHADER, fsr);
	if (geometrySource) geometryShaderId_ = loadShaderSource(geometrySource, GL_GEOMETRY_SHADER, gsr);

	if (!vsr || !fsr)
	{
		Log::instance()->printMessage("shader error!");
		getchar();
		return -1;
	}

	linkProgram();
	checkProgram();

	return shaderProgramId_;
}

bool Shader::getShaderFromMultCode(ShaderType type, const char* shaderName, const std::string& code)
{
	std::string start_mark("--");

	std::string name;

	switch (type)
	{
	case Shader::NONE:
		break;
	case Shader::VERTEX:
		name = start_mark + "VERTEX-" + shaderName;
		break;
	case Shader::FRAGMENT:
		name = start_mark + "FRAGMENT-" + shaderName;
		break;
	case Shader::GEOMETRY:
		name = start_mark + "GEOMETRY-" + shaderName;
		break;
	case Shader::TESS_TCL:
		name = start_mark + "TESS_TCL-" + shaderName;
		break;
	case Shader::TESS_TES:
		name = start_mark + "TESS_TES-" + shaderName;
		break;
	case Shader::DEFAULT:
		break;
	default:
		break;
	}

	size_t pos = code.find(name);
	if (pos == std::string::npos) return false;

	pos = code.find("\n", pos + 1);
	if (pos == std::string::npos) return false;

	pos++;

	size_t e_pos = code.find(start_mark, pos);

	std::string shader_code = code.substr(pos, e_pos - pos);

	return addShader(type, shader_code.c_str(), false);
}


bool Shader::addShader(ShaderType type, const char* src, bool file /*= true*/)
{
	type_ = (ShaderType)(static_cast<int>(type_) | static_cast<int>(type));

	std::string stream;
	if (file)
	{
		bool result = getShaderStream(src, stream, false);
		if (!result)
		{
			std::string fileName = get_shader_BasePath() + src;
			result = getShaderStream(fileName.c_str(), stream, true);
			if (!result)
				return false;
		}
	}
	else
		stream = std::string(src);
	GLint Result = GL_FALSE;
	GLuint shaderId = loadShaderSource(stream.c_str(), fetchShaderTypeFromType(type), Result);
	fetchShaderIdFromType(type) = shaderId;

	return Result;
}

// This function loads a vertex and fragment shader file and creates them
void Shader::initialize(const char* strVertexFile, const char* strFragmentFile)
{
	loadShaders(strVertexFile, strFragmentFile);
}

// This function returns a variable Id for a shader variable.  This is used
// when we need to pass in variables into a shader like numbers and matrices.
int Shader::getVariable(const char* strVariable, bool printMsg)const
{
	// If we don't have an active program object, let's return -1
	if (!shaderProgramId_)
		return -1;

	// This returns the variable Id for a variable in a shader file
	GLint loc = glGetUniformLocation(shaderProgramId_, strVariable);
	if (printMsg && (loc == -1))
	{
		printf("%s dose not exist \n", strVariable);
	}
	return loc;
}

int Shader::getVariable(const std::string& v, bool check /*= true*/) const
{
	return getVariable(v.c_str(), check);
}

unsigned int Shader::getUniformBlockIndex(const char* uniformVariable)
{
	if (!shaderProgramId_)
		return -1;
	GLuint loc = -1;

	loc = glGetUniformBlockIndex(shaderProgramId_, uniformVariable);

	return loc;
}

void Shader::setInt(int id, int newValue)const
{
	glUniform1i(id, newValue);
}


void Shader::setUInt(int id, int newValue) const
{
	glUniform1ui(id, newValue);
}

void Shader::setFloat(int id, float newValue)const
{
	glUniform1f(id, newValue);
}

void Shader::setFloat2(int id, float v0, float v1)
{
	glUniform2f(id, v0, v1);
}

void Shader::setFloat3(int id, float v0, float v1, float v2)
{
	glUniform3f(id, v0, v1, v2);
}

void Shader::setFloat4(int id, float v0, float v1, float v2, float v3)
{
	glUniform4f(id, v0, v1, v2, v3);
}

void Shader::setVec3f(int id, unsigned int count, const float *values)
{
	glUniform3fv(id, count, values);
}


void Shader::setVec2f(int id, unsigned int count, const float *values)
{
	glUniform2fv(id, count, values);
}

void Shader::setVec2Int(int id, unsigned int count, const int *values)
{
	glUniform2iv(id, count, values);
}

void Shader::setVec2UInt(int id, unsigned int count, const unsigned int *values)
{
	glUniform2uiv(id, count, values);
}

// This function frees all of our shader data
void Shader::destroy()
{
	// If our vertex shader pointer is valid, free it
	if (vertexShaderId_)
	{
		glDetachShader(shaderProgramId_, vertexShaderId_);
		glDeleteShader(vertexShaderId_);
		vertexShaderId_ = 0;
	}

	// If our fragment shader pointer is valid, free it
	if (fragmentShaderId_)
	{
		glDetachShader(shaderProgramId_, fragmentShaderId_);
		glDeleteShader(fragmentShaderId_);
		fragmentShaderId_ = 0;
	}

	// If our program object pointer is valid, free it
	if (shaderProgramId_)
	{
		glDeleteShader(shaderProgramId_);
		shaderProgramId_ = 0;
	}
}

void Shader::setUniformBlocking(GLint id, GLuint value)
{
	glUniformBlockBinding(shaderProgramId_, id, value);
}


void Shader::setFloat3V(int id, unsigned int count, const float* values)
{
	glUniform3fv(id, count, values);
}

void Shader::setMatrix4(int id, unsigned int count, bool transpose, const float* value)
{
	glUniformMatrix4fv(id, count, transpose, value);
}

void Shader::setMatrix3(int id, unsigned int count, bool transpose, const float* value)
{
	glUniformMatrix3fv(id, count, transpose, value);
}

void Shader::turnOn()
{
	glUseProgram(shaderProgramId_);
}

void Shader::turnOff()
{
	glUseProgram(0);
}

void Shader::gLBindAttribLocation()
{
	//    glBindAttribLocation(ShaderProgramId,0,"vertexPosition_modelspace");
	//    glBindAttribLocation(ShaderProgramId,1,"vertexColor");
	//    glBindAttribLocation(ShaderProgramId,2,"vertexUV");
}


GLuint Shader::loadShaderSource(const char* source, unsigned int shaderType, int& result)
{
	// Create the shaders
	GLuint shaderId = glCreateShader(shaderType);
	char const* SourcePointer = source;
	glShaderSource(shaderId, 1, &SourcePointer, NULL);
	glCompileShader(shaderId);
	glAttachShader(shaderProgramId_, shaderId);

	GLint Result = GL_FALSE;
	// Check Shader
	CHECK_GL_ERROR;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
	Log::instance()->printShaderInfoLog(shaderId);
	result = Result;
	return shaderId;
}


bool Shader::linkProgram()
{
	printf("Linking program...\n");
	gLBindAttribLocation();
	glLinkProgram(shaderProgramId_);
	return true;
}

bool Shader::checkProgram()
{
	// Check the program
	GLint Result = GL_FALSE;
	GLint InfoLogLength;
	glGetProgramiv(shaderProgramId_, GL_LINK_STATUS, &Result);
	glGetProgramiv(shaderProgramId_, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(shaderProgramId_, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	if (Result != GL_TRUE) return false;

	return true;
}


unsigned int& Shader::fetchShaderIdFromType(ShaderType type)
{
	switch (type)
	{
	case Shader::VERTEX:
		return vertexShaderId_;
	case Shader::FRAGMENT:
		return fragmentShaderId_;
	case Shader::GEOMETRY:
		return geometryShaderId_;
	case Shader::TESS_TCL:
		return tess_tcl_ShaderId_;
	case Shader::TESS_TES:
		return tess_tes_ShaderId_;
	case Shader::DEFAULT:
		return vertexShaderId_;
	default:
		return vertexShaderId_;
	}
}


GLuint Shader::fetchShaderTypeFromType(ShaderType type)
{
	switch (type)
	{
	case Shader::VERTEX:
		return GL_VERTEX_SHADER;
	case Shader::FRAGMENT:
		return GL_FRAGMENT_SHADER;
	case Shader::GEOMETRY:
		return GL_GEOMETRY_SHADER;
	case Shader::TESS_TCL:
		return GL_TESS_CONTROL_SHADER;
	case Shader::TESS_TES:
		return GL_TESS_EVALUATION_SHADER;
	case Shader::DEFAULT:
		return GL_VERTEX_SHADER;
	default:
		return GL_VERTEX_SHADER;
	}
}

GLint Shader::getUniformBlock(const char* uniformBlockName, GLint& uboDataSize)
{
	GLuint uboIndex = glGetUniformBlockIndex(shaderProgramId_, uniformBlockName);

	if (GL_INVALID_INDEX == uboIndex) return -1;

	glGetActiveUniformBlockiv(shaderProgramId_, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboDataSize);

	return  uboIndex;

}

int   Shader::getAttribute(const char* attribute)
{
	// If we don't have an active program object, let's return -1
	if (!shaderProgramId_)
		return -1;

	// This returns the variable Id for a variable in a shader file
	GLint loc = glGetAttribLocation(shaderProgramId_, attribute);
	if (loc == -1)
	{
		printf("%s dose not exist \n", attribute);
	}
	return loc;
}

bool    Shader::getUniformsiv(unsigned int uboIndex, const char** uniformNames,
	unsigned int* indices, int* size, int* offset,
	int* type, int uniformCount)

{
	glGetUniformIndices(shaderProgramId_, uniformCount, uniformNames, indices);

	glGetActiveUniformsiv(shaderProgramId_, uniformCount, indices, GL_UNIFORM_OFFSET, offset);

	glGetActiveUniformsiv(shaderProgramId_, uniformCount, indices, GL_UNIFORM_SIZE, size);

	glGetActiveUniformsiv(shaderProgramId_, uniformCount, indices, GL_UNIFORM_TYPE, type);


	return  false;
}

bool Shader::bebforeSubroutine()
{

	glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &subroutinesActivesNum_);

	subroutinesActives_ = new GLuint[subroutinesActivesNum_];

	return true;
}

void Shader::afterSubroutine()
{
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, subroutinesActivesNum_, subroutinesActives_);
}

bool Shader::setSubroutine(unsigned int shaderType, const char* uniformName, std::vector<std::string>& sub)
{
	GLint uniformLocation = glGetSubroutineUniformLocation(shaderProgramId_, shaderType, uniformName);

	if (uniformLocation == -1) return false;

	uniformLocations_[uniformName] = uniformLocation;

	for (std::vector<std::string>::iterator iter = sub.begin(); iter != sub.end(); iter++)
	{
		GLuint index = glGetSubroutineIndex(shaderProgramId_, shaderType, (*iter).c_str());

		if (GL_INVALID_INDEX != index)
		{
			std::map<std::string, GLuint>& map = subroutines_[uniformName];
			map[*iter] = index;
		}
	}
	return true;
}

bool Shader::choseSubroutine(const char* uniform, const char* choseName, unsigned int* indices)
{
	const std::map<std::string, GLuint>& subroutines = subroutines_[uniform];

	std::map<std::string, GLuint>::const_iterator findIter = subroutines.find(choseName);

	if (findIter == subroutines.cend()) return false;

	indices[uniformLocations_[uniform]] = findIter->second;

	return true;
}
