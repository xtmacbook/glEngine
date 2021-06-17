//
//  shader.h
//  openGLTest
//
//  Created by xt on 18/7/6.
//  Copyright © 2018年 xt. All rights reserved.
//

#ifndef shader_h
#define shader_h

#include "BaseObject.h"
#include "smartPointer.h"

#include <map>
#include <string>
#include <vector>



class LIBENIGHT_EXPORT Shader :public base::BaseObject
{
public:

	enum  ShaderType : unsigned int
	{
		NONE = 0x00000000,
		VERTEX = 1 << 1,
		FRAGMENT = 1 << 2,
		GEOMETRY = 1 << 3,
		TESS_TCL = 1 << 4,
		TESS_TES = 1 << 5,
		DEFAULT = VERTEX | FRAGMENT

	};

	Shader(ShaderType = DEFAULT);

	Shader(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path = NULL);

	~Shader() { destroy(); }
	static std::string	loadMultShaderInOneFile(const char* file);
	unsigned int    loadShaders(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path = NULL);

	unsigned int    loadShaderSouce(const char* vertexSource, const char* fragmentSource, const char* geometrySource);

	bool			addShader(ShaderType, const char* str, bool file = true);
	bool			getShaderFromMultCode(ShaderType, const char* shaderName, const std::string& code);
	// This loads a vertex and fragment shader from a text file (relative or full path)
	void            initialize(const char* strVertexFile, const char* strFragmentFile);

	// This returns an ID for a variable in our shader, to be used with a Set*() function
	int             getVariable(const char* strVariable, bool check = true)const;
	int             getVariable(const std::string&, bool check = true)const;

	int             getAttribute(const char* attribute);

	unsigned int    getUniformBlockIndex(const char* uniformVariable);

	// Below are functions to set an integer, a set of floats or a matrix (float[16])
	void            setInt(int id, int newValue)const;
	void            setUInt(int id, int newValue)const;
	
	void            setFloat(int id, float newValue)const;
	void            setFloat2(int id, float v0, float v1);
	void            setFloat3(int id, float v0, float v1, float v2);
	void            setFloat4(int id, float v0, float v1, float v2, float v3);
	
	void			setVec3f(int id, unsigned int count, const float *values);
	void			setVec2f(int id, unsigned int count, const float *values);
	void			setVec2Int(int id, unsigned int count, const int *values);
	void			setVec2UInt(int id, unsigned int count, const unsigned int *values);

	void            setFloat3V(int id, unsigned int count, const float* values);
	// This allows us to pass in 4x4 matrix (float array of 16) to a shader by the Id received from GetVariable()
	void            setMatrix4(int id, unsigned int count, bool transpose, const float* value);
	void            setMatrix3(int id, unsigned int count, bool transpose, const float* value);

	// These 2 functions turn on and off our shader, which uses the OpenGL glUseProgram() function
	void            turnOn();
	void            turnOff();

	// This releases our memory for our shader
	void            destroy();

	unsigned int    getShaderId()const { return shaderProgramId_; }

	//uniform block
	void            setUniformBlocking(int id, unsigned int value);
	int             getUniformBlock(const char* uniformName, int& uboDataSize);
	bool            getUniformsiv(unsigned int uboIndex, const char** uniformNames, unsigned int* indices, int* size, int* offset, int* type, int uboElementsNum);


	//subroutine uniform

	bool            bebforeSubroutine();
	void            afterSubroutine();
	bool            setSubroutine(unsigned int shaderType, const char* uniformName, std::vector<std::string>&);
	bool            choseSubroutine(const char* uniform, const char* choseName, unsigned int* indices);

	bool			linkProgram();
	bool			checkProgram();
private:
	void            gLBindAttribLocation();
	unsigned int          loadShaderSource(const char* source, unsigned int shaderType, int& result);
	unsigned int& fetchShaderIdFromType(ShaderType);
	unsigned int	        fetchShaderTypeFromType(ShaderType);
private:

	unsigned int										 vertexShaderId_;
	unsigned int										 fragmentShaderId_;
	unsigned int										 geometryShaderId_;
	unsigned int										 tess_tcl_ShaderId_;
	unsigned int										 tess_tes_ShaderId_;
	unsigned int										 shaderProgramId_ = 0;

	std::map<std::string, unsigned int>                         uniformLocations_;
	unsigned int* subroutinesActives_;
	int                                              subroutinesActivesNum_;
	std::map<std::string, std::map<std::string, unsigned int> >  subroutines_;

	ShaderType											 type_;
};
typedef base::SmartPointer<Shader> Shader_SP;



#endif /* shader_h */
