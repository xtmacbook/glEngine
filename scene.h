//
//  Model.h
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__Scene__
#define __OpenGL__Scene__

#include "baseScene.h"
#include "IGeometry.h"
#include "IRenderNode.h"
#include "shader.h"

class LIBENIGHT_EXPORT Scene :public BaseScene
{
public:
	Scene();
	virtual ~Scene();

	virtual void					preRender(PassInfo&);

	virtual bool					initialize(const SceneInitInfo&);
	virtual bool					update();
	virtual void					render(PassInfo&);
	
	void							home();
	void							stepRotate(int key);
	virtual void					refreshShader(int num);

	virtual void					processKeyboard(int key, int st, int action, int mods, float deltaTime);
	virtual void					mouse_move(const V2f& pt, const float& z_scale);
	virtual void					mouse_down(int button, const V2f& pt, int state);
	virtual void					mouse_up(int button, const V2f& pt, int state);
	virtual void					mouse_scroll(double xoffset, double yoffset);
protected:
	virtual bool					initKeyCallback();
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initOpengl(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual bool					initLight(const SceneInitInfo&);

	
	void							clearRenderNode();
	void							addRenderNode(IRenderNode_SP);
	IRenderNode_SP					getRenderNode(unsigned int indice);
	unsigned int					renderNodeNum()const;

	//添加测试场景模型用来测试
	void							createSceneData(void);
	// This draws the Model to the backbuffer
	void							destroy();

	void							expandSceneByBox(const base::BoundingBox&);

protected:

	std::vector<IRenderNode_SP>		renderNodes_;
	IRenderNode_SP					axesNodes_;
	Shader_SP 						axisShder_;

	base::BoundingBox				scene_box_;
};


#endif /* defined(__OpenGL__Model__) */
