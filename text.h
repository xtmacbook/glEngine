//
//  text.h
//  OpenGL
//
//  Created by xt on 8/12/16
//  Copyright (c) 2016ƒÍ xt. All rights reserved.
//

#ifndef __OpenGL__text__
#define __OpenGL__text__

#include "decl.h"
#include "type.h"

#include <map>
#include <string>
struct Character;
class Shader;

class LIBENIGHT_EXPORT Text
{
public:
	bool				init(void);
	static bool			initTextLib(void);

	Text();
	~Text();

	void				drawText(const std::string& text, float x, float y, float w, float h, float scale, math::V3f color);

private:
	void				initShader(void);

	static std::map<unsigned char, Character*>	characters_;

	Shader *									shader_;
	unsigned int								vao_;
	unsigned int								vbo_;

};

bool	initText();
void	LIBENIGHT_EXPORT printScreen(const std::string&text, unsigned int x, unsigned int y,float w,float h);

#endif
