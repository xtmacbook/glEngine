

#include <shader.h>
#include <cameraBase.h>
#include <gls.h>
#include <BaseObject.h>
#include <smartPointer.h>
#include <texture.h>

class Cloud :public base::BaseObject
{
public:
	Cloud();

	virtual ~Cloud();

	void	init();

	void	initTexture();

	void    render(const CameraBase*);

	void    initShader();

private:
	Shader_SP	shader_ = nullptr;
	Texture_Sp  texture_ = nullptr;

	int trianglesCount_;
	GLuint vertexBuffer_Ao_;
	GLuint vb_[2];
	V2f  mCloudMove_;
};
