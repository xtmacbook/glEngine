
#include "renderNode.h"
#include "shader.h"
#include "log.h"
#include "helpF.h"

void RenderNode::render(Shader*shader, PassInfo&pif)
{
	if (geometry_)
	{
		DrawInfo di;
		di.draw_shader_ = shader;
		di.needBind_ = true;
		di.matrix_ = pif.tranform_;
		geometry_->drawGeoemtry(di);
	}

}


void RenderNode::update(const CameraBase*camera)
{
	if (geometry_)
		geometry_->updateGeometry(camera);
}

base::AABB RenderNode::getBox() const
{
	if (geometry_)
		return geometry_->boundingBox();
	return AABB();
}

math::Matrixf RenderNode::getModelMatix() const
{
	if (geometry_) return geometry_->getModelMatrix();
	return Matrixf();
}


void RenderNode::setGeometry(CommonGeometry_Sp g)
{
	geometry_ = g;
}


CommonGeometry_Sp RenderNode::getGeometry()
{
	return geometry_;
}

void RenderNode::bindVAO()
{
	if (geometry_) geometry_->bindVAO();
}

void RenderNode::unBindVAO()
{
	if (geometry_) geometry_->unBindVAO();
}

void RenderNode::loadTextures()
{
	std::vector<Texture_Sp>::iterator iter = textures_.begin();
	for (; iter != textures_.end(); iter++)
	{
		if ((*iter)->loadData())
			(*iter)->createObj();
			(*iter)->bind();
			(*iter)->mirrorRepeat();
			(*iter)->filterLinear();
			if ((*iter)->context(NULL))
			{
				(*iter)->unBind();
			}
			CHECK_GL_ERROR;
	}
}

