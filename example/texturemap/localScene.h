
#include "scene.h"

class lScene :public Scene
{
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initKeyCallback();
	void							refreshShader(int);
public:
	virtual void					render(PassInfo&);
};
