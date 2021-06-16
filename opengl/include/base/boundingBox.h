//
//  BaseRender.h
//  
//
//  Created by xt on 2019/12/26.
//
//

#ifndef ____BaseBoundingBox__
#define ____BaseBoundingBox__

#include "decl.h"
#include "type.h"
using namespace math;
namespace base {

	class  LIBENIGHT_EXPORT BoundingBox
	{
	public:

		V3f min_;
		V3f max_;

		inline BoundingBox() :
			min_(FLT_MAX,
				FLT_MAX,
				FLT_MAX),
			max_(-FLT_MAX,
				-FLT_MAX,
				-FLT_MAX)
		{}

		inline BoundingBox(const BoundingBox& bb) :
			min_(bb.min_),
			max_(bb.max_)
		{}

		inline BoundingBox(float xmin, float ymin, float zmin,
			float xmax, float ymax, float zmax) :
			min_(xmin, ymin, zmin),
			max_(xmax, ymax, zmax) {}

		inline BoundingBox(const V3f& min, const V3f& max) :
			min_(min),
			max_(max) {}

		inline void init()
		{
			min_ = V3f(FLT_MAX,
				FLT_MAX,
				FLT_MAX);
			max_ = V3f(-FLT_MAX,
				-FLT_MAX,
				-FLT_MAX);
		}

		inline bool operator == (const BoundingBox& rhs) const { return min_ == rhs.min_ && max_ == rhs.max_; }
		inline bool operator != (const BoundingBox& rhs) const { return min_ != rhs.min_ || max_ != rhs.max_; }

		inline bool valid() const
		{
			return max_[0] >= min_[0] && max_[1] >= min_[1] && max_[2] >= min_[2];
		}

		inline float & xMin() { return min_[0]; }
		inline float  xMin() const { return min_[0]; }
		inline float & yMin() { return min_[1]; }
		inline float  yMin() const { return min_[1]; }
		inline float & zMin() { return min_[2]; }
		inline float  zMin() const { return min_[2]; }
		inline float & xMax() { return max_[0]; }
		inline float  xMax() const { return max_[0]; }
		inline float & yMax() { return max_[1]; }
		inline float  yMax() const { return max_[1]; }
		inline float & zMax() { return max_[2]; }
		inline float  zMax() const { return max_[2]; }

		inline const V3f center() const
		{
			return  (min_ + max_) * 0.5f;
		}

		inline float radius() const
		{
			return distance(max_, min_);
		}

		inline const V3f corner(unsigned int pos) const
		{
			return V3f(pos & 1 ? max_[0] : min_[0], pos & 2 ? max_[1] : min_[1], pos & 4 ? max_[2] : min_[2]);
		}

		inline void expandBy(const V3f& v)
		{
			if (v[0] < min_[0]) min_[0] = v[0];
			if (v[0] > max_[0]) max_[0] = v[0];

			if (v[1] < min_[1]) min_[1] = v[1];
			if (v[1] > max_[1]) max_[1] = v[1];

			if (v[2] < min_[2]) min_[2] = v[2];
			if (v[2] > max_[2]) max_[2] = v[2];
		}

		inline void expandBy(float x, float y, float z)
		{
			if (x < min_[0]) min_[0] = x;
			if (x > max_[0]) max_[0] = x;

			if (y < min_[1]) min_[1] = y;
			if (y > max_[1]) max_[1] = y;

			if (z < min_[2]) min_[2] = z;
			if (z > max_[2]) max_[2] = z;
		}

		void expandBy(const BoundingBox& bb)
		{
			if (!bb.valid()) return;

			if (bb.min_[0] < min_[0]) min_[0] = bb.min_[0];
			if (bb.max_[0] > max_[0]) max_[0] = bb.max_[0];

			if (bb.min_[1] < min_[1]) min_[1] = bb.min_[1];
			if (bb.max_[1] > max_[1]) max_[1] = bb.max_[1];

			if (bb.min_[2] < min_[2]) min_[2] = bb.min_[2];
			if (bb.max_[2] > max_[2]) max_[2] = bb.max_[2];
		}

		inline bool contains(const V3f& v) const
		{
			return valid() &&
				(v[0] >= min_[0] && v[0] <= max_[0]) &&
				(v[1] >= min_[1] && v[1] <= max_[1]) &&
				(v[2] >= min_[2] && v[2] <= max_[2]);
		}

		inline bool contains(const V3f& v, float epsilon) const
		{
			return valid() &&
				((v[0] + epsilon) >= min_[0] && (v[0] - epsilon) <= max_[0]) &&
				((v[1] + epsilon) >= min_[1] && (v[1] - epsilon) <= max_[1]) &&
				((v[2] + epsilon) >= min_[2] && (v[2] - epsilon) <= max_[2]);
		}

		float		distanceFromPoint(const V3f&pos)const;
	};

	inline float BoundingBox::distanceFromPoint(const V3f&pos) const
	{
		return 1.0f;
	}

	typedef BoundingBox AABB;
}
#endif
