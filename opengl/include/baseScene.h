//
//  BaseRender.h
//  
//
//  Created by xt on 16/1/31.
//
//

#ifndef ____BaseScene__
#define ____BaseScene__

#include "light.h"

#include <stdio.h>
#include <vector>


class Shader;
class CameraBase;
class Texture;

struct LIBENIGHT_EXPORT SceneInitInfo
{
	unsigned int w_;
	unsigned int h_;
};

class LIBENIGHT_EXPORT PassInfo
{
public :
	PassInfo();
	Matrixf  tranform_;
};

class LIBENIGHT_EXPORT BaseScene :public base::BaseObject
{
public:
	BaseScene();
	~BaseScene();

	CameraBase *            getCamera()const;
	void                	setMasterCamera(CameraBase* pCamera);
	void					addLight(Light_SP);
	void					setShader(int i, Shader*);
protected:
	void					initUniformVal(Shader*shader);
	void					preRenderShaderData();

protected:

	CameraBase *                    camera_;
	int                         uboBlock_;
	std::vector<Texture*>		texturesObj_;
	std::vector<Shader*>        shaders_;
	std::vector<Light_SP>		dlights_;
	std::vector<Light_SP>		slights_;
	std::vector<Light_SP>		plights_;

};
#endif /* defined(____BaseRender__) */
