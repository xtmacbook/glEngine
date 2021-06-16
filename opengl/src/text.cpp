
#include "text.h"
#include "shader.h"
#include "gls.h"

Text::Text()
{
    
}

Text::~Text()
{
	if(shader_)
	{
		delete shader_;
		shader_ = NULL;
	}
}

bool Text::init(void)
{
	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);
	glBindVertexArray(vao_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); 

	initShader();

	return true;
}
Text g_text_engine_;
bool initText()
{
	bool result = Text::initTextLib();
	if (!result) return false;
	return g_text_engine_.init();
}
 
void LIBENIGHT_EXPORT printScreen(const std::string&text, unsigned int x, unsigned int y,float w,float h)
{
	g_text_engine_.drawText(text, x, y,w,h, 0.5, math::V3f(0.0f, 0.5f, 0.0f));
}


