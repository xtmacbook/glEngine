#include "text.h"
#include "shader.h"
#include "sys.h"
#include "log.h"
#include "type.h"
#include "helpF.h"
#include "resource.h"
#include "common.h"
#include "gls.h"

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H




using namespace math;

struct Character
{
	GLuint		textureID;
	glm::ivec2	size;
	glm::ivec2	bearing;
	GLuint		advance;
};

std::map<unsigned char, Character*> Text::characters_;

const char* getFreetypeErrorMessage(FT_Error err)
{
#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  case e: return s;
#define FT_ERROR_START_LIST     switch (err) {
#define FT_ERROR_END_LIST       }
#include FT_ERRORS_H
	return "(Unknown error)";
}

bool Text::initTextLib(void)
{
	FT_Error error;
	FT_Library    library;
	error = FT_Init_FreeType(&library);
	if (error != 0)
		return false;

	const char * fontPath;
#if defined EN_OS_WIN32 || defined EN_OS_WIN64
	fontPath = "c:/Windows/Fonts/calibri.ttf";
#elif  EN_OS_DARWIN
	fontPath = "/System/Library/Fonts/Keyboard.ttf";
#endif

	FT_Face       face;
	error = FT_New_Face(library, fontPath, 0, &face);
	if (error == FT_Err_Unknown_File_Format)
	{
		Log::instance()->printMessage("\
				the font file could be opened and read, but it appears,\
				that its font format is unsupported");
		return false;
	}
	else if (error)
	{
		const char* message = getFreetypeErrorMessage(error);
		Log::instance()->printMessage(message);
		return false;
	}
	//set font's width and height 
	error = FT_Set_Pixel_Sizes(face, 0, 36);
	if (error)
		return false;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			continue;

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character * cr = new Character;
		cr->textureID = texture;
		cr->size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		cr->bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		cr->advance = face->glyph->advance.x;

		characters_.insert(std::pair<unsigned char, Character*>(c, cr));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	return true;
}

void Text::initShader(void)
{
	shader_ = new Shader;
	std::string path = get_shader_BasePath();
	shader_->addShader(Shader::VERTEX, (path + "text/text.vert").c_str());
	shader_->addShader(Shader::FRAGMENT, (path + "text/text.frag").c_str());
	shader_->linkProgram();
	shader_->checkProgram();
	shader_->turnOn();

	GLuint loc = shader_->getVariable("textSample");
	if(loc != -1)
		shader_->setInt(loc, 0);
	shader_->turnOff();
}


void Text::drawText(const std::string& t, float x, float y, float w, float h,float scale, math::V3f color)
{
	GLboolean blendEnabled;
	glGetBooleanv(GL_BLEND, &blendEnabled);

	// save off current state of src / dst blend functions
	GLint blendSrc;
	GLint blendDst;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

	shader_->turnOn();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint projL = glGetUniformLocation(shader_->getShaderId(), "projection");
	Matrixf projection = glm::ortho(0.0f, w, 0.0f, h);
	glUniformMatrix4fv(projL, 1, GL_FALSE, math::value_ptr(projection));

	glUniform3f(glGetUniformLocation(shader_->getShaderId(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao_);

	// Iterate through all characters
	std::string::const_iterator c;
	std::string text(t);

	for (c = text.begin(); c != text.end(); c++)
	{
		Character* ch = characters_[*c];

		GLfloat xpos = x + ch->bearing.x * scale;
		GLfloat ypos = y - (ch->size.y - ch->bearing.y) * scale;

		GLfloat w = ch->size.x * scale;
		GLfloat h = ch->size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch->textureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch->advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	shader_->turnOff();

	if (blendEnabled) 
		glEnable(GL_BLEND);
	else 
		glDisable(GL_BLEND);

	glBlendFunc(blendSrc, blendDst);
}
