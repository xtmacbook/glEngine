#ifndef __OpenGL__Water_
#define __OpenGL__Water_


//
//  Water.h
//  openGLTest
//
//  Created by xt on 19/3/14.
//  Copyright © 2019 xt. All rights reserved.
//

#include "wave.h"

#include <vector>


class Shader;
class WaterGemoetry;
class WaterShader;
class CameraBase;
class Water;

class CommonGeometry;

class WaterEnditer
{
public:
	static void SetWater(Water * water);
	static void DisplayEditor();
private:
	WaterEnditer();
	static void MenuBar();
	static void DisplayWave(Wave & wave);
	static void ChangeWaterColor();
	static void ChangeLightColor();
	static void ChangeColor(glm::vec3 * color);
	static void EditWaveName();
 
	static Water * m_Water;

	static bool m_ChangingWaterColor;
	static bool m_ChangingLightColor;

	static bool m_Opening;
};

class Water
{

	struct Vertex
	{
		Vertex(float x, float y, float z) : X_(x), Y_(y), Z_(z) {}
		void Zero() { X_ = 0.0f; Y_ = 0.0f; Z_ = 0.0f; }
		float X_;
		float Y_;
		float Z_;
	};

	struct Triangle
	{
		Triangle(unsigned int index1, unsigned int index2, unsigned int index3) :
			m_Index1(index1), m_Index2(index2), m_Index3(index3) {}
		//! First vertex index.
		unsigned int m_Index1;
		//! Second vertex index.
		unsigned int m_Index2;
		//! Third vertex index.
		unsigned int m_Index3;
	};

public:
	Water(unsigned x_stride, unsigned z_stride);

	void				setStride(unsigned x, unsigned z);
	void				prepareGridData();
	void				prepareShader();
	void				initGeometry();
	void				removeWave(Wave *w);
	void				addWave(Wave w);
	void				update();
	void				render(CameraBase *);

private:
	void				getSumWave(float x,float z,float t, std::pair<V3f, V3f>&full_offset);
private:
	
  unsigned					xStride_; //! The number of vertices in the x direction.
  unsigned					zStride_;//! The number of vertices in the z direction.

  unsigned					num_verts_;

  std::vector<Vertex>		vertexData_;
  std::vector<Wave>			waves_;

  friend class WaterGemoetry;

  WaterShader*				shader_;

  CommonGeometry*	geometry_;

  friend class WaterEnditer;
};

#endif
