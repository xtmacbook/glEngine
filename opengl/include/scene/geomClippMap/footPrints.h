#include "decl.h"
#include "BaseObject.h"
#include <vector>
#include <type.h>

class Shader;
using namespace math;
namespace scene
{
	
	struct LIBENIGHT_EXPORT RenderShaderIndex
	{
		int	uView_;
		int uProj_;
		int uMode_;

		int ulevelOffset_;

		int ulocalOffset_;
		int uScale_;

		int heightMap_;
		int diffMap_;
	};

	struct LIBENIGHT_EXPORT BlockRenderState
	{
		V2f	  offset_;
		float scale_;
	};

	struct TerrVertex
	{
		V3f position_;
		V3f color_;
	};

	typedef  int32 INDICESTYPE;

	class  TerrainElement :public base::BaseObject
	{
	public:

		virtual ~TerrainElement();

		virtual	void	destroryGlObj();
		virtual	void	init(int m);
		virtual void	initGlObj();
		virtual void	render(Shader*, const RenderShaderIndex*, BlockRenderState*);
		virtual	void	initGeometry(int size);
		virtual void	pushGLData();

		unsigned int	vao_;

		std::vector<TerrVertex>			vertices_;
		std::vector<INDICESTYPE>		indices_;
	};

	class  LIBENIGHT_EXPORT TerrainBlock :public TerrainElement
	{
	public:
		virtual	void	initGeometry(int size);
		static  void	initOffset(int cmapsize);
		virtual	void	initGlObj();

		static V2i		block_offset_from_level_center_[16];

	};

	class LIBENIGHT_EXPORT FixUp :public TerrainElement
	{
	public:
		virtual	void	initGeometry(int size);
		static  void	initOffset(int cmapsize);
		virtual void	render(Shader*, const RenderShaderIndex*, BlockRenderState*, bool vertical);
		virtual	void	initGlObj();
		unsigned		int  indices_vbo_[2];
		static			V2i local_offset_[4];
	};

	class LIBENIGHT_EXPORT InteriorTrim :public TerrainElement
	{
	public:
		virtual	void	initGeometry(int size);
		static  void	initOffset(int cmapsize);
		virtual void	render(Shader*, const RenderShaderIndex*, BlockRenderState*, bool vertical);
		virtual	void	initGlObj();
		unsigned		int  indices_vbo_[2];
		int				num_vertices_indices_;
		static V4i		local_offset_[4]; // v4i :

	};

	class LIBENIGHT_EXPORT OutDegenerateTriangles :public TerrainElement
	{
	public:
		virtual	void	initGeometry(int size);
		virtual void	render(Shader*, const RenderShaderIndex*, BlockRenderState*, int flag);
		virtual	void	initGlObj();
		unsigned		int  indices_vbo_[4]; // top right buttom left
		static			V2i local_offset_[4]; // top right buttom left

	};

	class LIBENIGHT_EXPORT FinerEstCenter : public TerrainElement
	{
	public:
		virtual	void	initGeometry(int size);
		virtual	void	initGlObj();
		unsigned		int  indices_vbo_; // top right buttom left

	};

	class  LIBENIGHT_EXPORT FootPrints
	{
	public:
		void init(int cmapSize);

		void renderCenterLevel(Shader*, const RenderShaderIndex*, BlockRenderState*);
		void renderBlocks(Shader*, const RenderShaderIndex*, BlockRenderState*, int num);

		void preRenderFixUP();
		void postRenderFixUP();
		void renderFixUP(Shader*, const RenderShaderIndex*, BlockRenderState*, int num, bool vertical = true);

		void preRenderTrim();
		void postRenderTrim();
		void renderTrim(Shader*, const RenderShaderIndex*, BlockRenderState*, bool vertical = true);

		void preRenderDegenerateTriangle();
		void postRenderDegenerateTrianglem();
		void renderDegenerateTriangle(Shader*, const RenderShaderIndex*, BlockRenderState*, int flag);

		TerrainBlock block_;
		FixUp						fixUP_;
		InteriorTrim				trim_;
		OutDegenerateTriangles		degenerates_;
		FinerEstCenter				center_;
		int							cmapSize_;
	};

}