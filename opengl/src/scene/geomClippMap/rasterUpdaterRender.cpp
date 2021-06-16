#include "rasterTile.h"
#include "rasterUpdater.h"
#include "shader.h"
#include "log.h"
#include "framebuffers.h"
#include "helpF.h"
#include "texture.h"
#include "IGeometry.h"
#include "gls.h"

namespace scene
{
	namespace internal
	{
		Pass::Pass()
		{
			shader_ = new Shader;
			fbuffer_ = new FrameBufferObject;
		}

		Pass::~Pass()
		{
			if (shader_)
			{
				delete shader_;
				shader_ = nullptr;
			}
			if (fbuffer_)
			{
				delete fbuffer_;
				fbuffer_ = nullptr;
			}
		}

		void Pass::setQuad(Quad*q)
		{
			quad_ = q;
		}

		void Pass::draw(Texture * texture)
		{
			Texture * textures = { texture };

			fbuffer_->colorTextureAttachments(textures,1);

			fbuffer_->enableRenderToColorAndDepth(0);

			DrawInfo info;
			info.draw_shader_ = shader_;

			quad_->drawGeoemtry(info);

			fbuffer_->disableRenderToColorDepth();
		}

		void Pass::initStateAndObjs(const char*shader)
		{

			std::string code = Shader::loadMultShaderInOneFile(shader);

			shader_->getShaderFromMultCode(Shader::VERTEX, "base", code);
			shader_->getShaderFromMultCode(Shader::FRAGMENT, "base", code);
			shader_->linkProgram();
			shader_->checkProgram();

			texture0_ = shader_->getVariable("texture0");
			viewportOrthographicMatrix_ = shader_->getVariable("viewportOrthographicMatrix");
			initUniform();

			math::Matrixf orthMat = math::ortho(0.0f, width_ * 1.0f, 0.0f, height_* 1.0f, 0.0f, 1.0f);
			shader_->turnOn();
			shader_->setInt(texture0_, 0);
			shader_->setMatrix4(viewportOrthographicMatrix_, 1, 0, math::value_ptr(orthMat));
			shader_->turnOff();

			CHECK_GL_ERROR;
		}

		void UpdatePass::initUniform()
		{
			sourceOrigin_ = shader_->getVariable("sourceOrigin");
			updateSize_ = shader_->getVariable("updateSize");
			destinationOffset_ = shader_->getVariable("destinationOffset");
		}

		void SamplePass::initUniform()
		{
			sourceOrigin_ = shader_->getVariable("sourceOrigin");
			updateSize_ = shader_->getVariable("updateSize");
			destinationOffset_ = shader_->getVariable("destinationOffset");
			oneOverTextureSize_ = shader_->getVariable("oneOverTextureSize");
		}

		void ComputeNormalPass::initUniform()
		{
			origin_ = shader_->getVariable("origin");
			updateSize_ = shader_->getVariable("updateSize");
			oneOverHeightMapSize_ = shader_->getVariable("oneOverHeightMapSize");
			heightExaggeration_ = shader_->getVariable("heightExaggeration");
			postDelta_ = shader_->getVariable("postDelta");
		}

		UpdatePass::UpdatePass()
		{
		}

		SamplePass::SamplePass()
		{
		}

		ComputeNormalPass::ComputeNormalPass()
		{

		}
	}
}
