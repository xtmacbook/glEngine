
#ifndef __OpenGL__ColorBlock__
#define __OpenGL__ColorBlock__

#include "color.h"
/// Uncompressed 4x4 color block.
namespace base
{
	struct ColorBlock
	{
		// Accessors
		const Color32 * colors() const;

		Color32 color(uint32 i) const;
		Color32 & color(uint32 i);

		Color32 color(uint32 x, uint32 y) const;
		Color32 & color(uint32 x, uint32 y);

	private:

		Color32 m_color[4 * 4];

	};


	/// Get pointer to block colors.
	inline const Color32 * ColorBlock::colors() const
	{
		return m_color;
	}

	/// Get block color.
	inline Color32 ColorBlock::color(uint32 i) const
	{
		return m_color[i];
	}

	/// Get block color.
	inline Color32 & ColorBlock::color(uint32 i)
	{
		return m_color[i];
	}

	/// Get block color.
	inline Color32 ColorBlock::color(uint32 x, uint32 y) const
	{
		return m_color[y * 4 + x];
	}

	/// Get block color.
	inline Color32 & ColorBlock::color(uint32 x, uint32 y)
	{
		return m_color[y * 4 + x];
	}

}
#endif // 
