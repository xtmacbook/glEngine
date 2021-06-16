
#include <shader.h>
#include <cameraBase.h>
#include <gls.h>
#include <BaseObject.h>
#include <smartPointer.h>

class SkyDom :public base::BaseObject
{
public:
	SkyDom();

	virtual ~SkyDom();

	void	init();

	void    render(const CameraBase*);

	void    initShader();

private:
	Shader_SP	shader_ = nullptr;
	GLuint		vertexBuffer_Ao_ = 0;
	GLuint		vb_[2];
	unsigned int trianglesCount_;
};