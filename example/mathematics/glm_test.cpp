#include "type.h"

glm::mat4 computeModelViewMatrix(float Translate, glm::vec2 const & Rotate)
{
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f));
	glm::vec4 c2 = m[3];
	glm::mat4 der(2.0f, -2.0f, 1.0f, 1.0f,
		-3.0f, 3.0f, -2.0f, -1.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f);


	glm::vec4 tv(0.0f, 1.0f, -5.0f, 0.0f);
	glm::vec4 r = tv * der;


	glm::mat4 scalem = glm::scale(glm::mat4(1.0), glm::vec3(1.0, 2.0, 3.0));
	glm::vec4 c1 = scalem[2]; //第三列

	glm::mat4 rotatem = glm::rotate(glm::mat4(1.0), (float)3.14159 / 3.0f, glm::vec3(0.0, 0.0, 1.0));
	glm::vec4 rc0 = rotatem[0]; //第三列
	glm::vec4 rc1 = rotatem[1]; //第三列

	//m[a][b] : a row and b col
	/*
		rotate by x axis:

		[cos -sin 0 0
		sin cos 0 0
		0   0  1  0
		0   0  0  1]
	
	*/

	glm::mat4 result = rotatem * scalem;

	return m;
}

int main()
{
	glm::vec2 t;

	computeModelViewMatrix(1.0, t);
}
