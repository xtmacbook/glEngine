#include "perlineNoise.h"
#include <fstream>
#include <iostream>

class PolyMesh
{
public:
	PolyMesh() : vertices(nullptr), st(nullptr), normals(nullptr) {}
	~PolyMesh()
	{
		if (vertices) delete[] vertices;
		if (st) delete[] st;
		if (normals) delete[] normals;
	}
	V3f *vertices;
	V2f *st;
	V3f *normals;
	uint32_t *faceArray;
	uint32_t *verticesArray;
	uint32_t numVertices;
	uint32_t numFaces;
	void exportToObj(const char*objfile);
};

 
void PolyMesh::exportToObj(const char*objfile)
{
	std::ofstream ofs;
	ofs.open(objfile, std::ios_base::out);

	for (uint32_t i = 0; i < numVertices; ++i) {
		ofs << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << std::endl;
	}

	for (uint32_t i = 0; i < numVertices; ++i) {
		ofs << "vt " << st[i].x << " " << st[i].y << std::endl;
	}

	for (uint32_t i = 0; i < numVertices; ++i) {
		ofs << "vn " << normals[i].x << " " << normals[i].y << " " << normals[i].z << std::endl;
	}

	for (uint32_t i = 0, k = 0; i < numFaces; ++i) {
		ofs << "f ";
		for (uint32_t j = 0; j < faceArray[i]; ++j) {
			uint32_t objIndex = verticesArray[k + j] + 1;
			ofs << objIndex << "/" << objIndex << "/" << objIndex << ((j == (faceArray[i] - 1)) ? "" : " ");
		}
		ofs << std::endl;
		k += faceArray[i];
	}

	ofs.close();
}

PolyMesh* createPolyMesh(
	uint32_t width = 1,
	uint32_t height = 1,
	uint32_t subdivisionWidth = 40,
	uint32_t subdivisionHeight = 40)
{
	PolyMesh *poly = new PolyMesh;
	poly->numVertices = (subdivisionWidth + 1) * (subdivisionHeight + 1);
	poly->vertices = new V3f[poly->numVertices];
	poly->normals = new V3f[poly->numVertices];
	poly->st = new V2f[poly->numVertices];
	float invSubdivisionWidth = 1.f / subdivisionWidth;
	float invSubdivisionHeight = 1.f / subdivisionHeight;
	for (uint32_t j = 0; j <= subdivisionHeight; ++j) {
		for (uint32_t i = 0; i <= subdivisionWidth; ++i) {
			poly->vertices[j * (subdivisionWidth + 1) + i] = V3f(width * (i * invSubdivisionWidth - 0.5), 0, height * (j * invSubdivisionHeight - 0.5));
			poly->st[j * (subdivisionWidth + 1) + i] = V2f(i * invSubdivisionWidth, j * invSubdivisionHeight);
		}
	}

	poly->numFaces = subdivisionWidth * subdivisionHeight;
	poly->faceArray = new uint32_t[poly->numFaces];
	for (uint32_t i = 0; i < poly->numFaces; ++i)
		poly->faceArray[i] = 4;

	poly->verticesArray = new uint32_t[4 * poly->numFaces];
	for (uint32_t j = 0, k = 0; j < subdivisionHeight; ++j) {
		for (uint32_t i = 0; i < subdivisionWidth; ++i) {
			poly->verticesArray[k] = j * (subdivisionWidth + 1) + i;
			poly->verticesArray[k + 1] = j * (subdivisionWidth + 1) + i + 1;
			poly->verticesArray[k + 2] = (j + 1) * (subdivisionWidth + 1) + i + 1;
			poly->verticesArray[k + 3] = (j + 1) * (subdivisionWidth + 1) + i;
			k += 4;
		}
	}

	return poly;
}


#define ANALYTICAL_NORMALS 1

int main(int argc, char **argv)
{

	ClassPerlineNoise class_noise(1,2,1,99);
	const uint32_t width = 512, height = 512;
	float *noiseMap = new float[width * height];
	for (uint32_t j = 0; j < height; ++j)
	{
		for (uint32_t i = 0; i < width; ++i)
		{
			noiseMap[j * width + i] = (class_noise.eval(i * (1 / 64.0f), j * (1 / 64.0f), 0.0) + 1.0) * 0.5f;
		}
	}
	std::ofstream ofs;
	ofs.open("./class_perline_noise.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned k = 0; k < width * height; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();

	ImprovedPerlinNoise improved_noise;
	for (uint32_t j = 0; j < height; ++j)
	{
		for (uint32_t i = 0; i < width; ++i)
		{
			noiseMap[j * width + i] = (improved_noise.eval(V3f(i, 0, j) * (1 / 64.0f)) + 1) * 0.5;
		}
	}

	ofs.open("./improved_perlin_noise.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned k = 0; k < width * height; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();

	//////////////////////
	for (int i = 0; i < width * height; i++)
	{
		noiseMap[i] = 0.0f;
	}
	for (uint32_t j = 0; j < height; ++j) {
		for (uint32_t i = 0; i < width; ++i)
		{
			V3f tmp;
			noiseMap[j * width + i] = (improved_noise.eval(V3f(i, 0, j) * (1 / 64.0f), tmp) + 1) * 0.5;
		}
	}
	ofs.open("./improved_derivatives_perlin_noise.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned k = 0; k < width * height; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();

	/////
	SimplexPerlinNoise snoise;
	snoise.init();
	for (uint32_t j = 0; j < height; ++j)
	{
		for (uint32_t i = 0; i < width; ++i)
		{
			noiseMap[j * width + i] = snoise.noise3D(i*(1 / 64.0f), 0, j  *(1 / 64.0f));// (snoise.noise3D(i*(1 / 64.0f), 0, j  *(1 / 64.0f)) + 1) * 0.5;
		}
	}

	ofs.open("./simplex_perlin_noise.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned k = 0; k < width * height; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();

	return 0;
}
