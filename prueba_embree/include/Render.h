#pragma once
#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include "../common/camera.h"
#include "../include/PhotonMapper.h"
#include "../include/Raytracer.h"
#include "../include/Scene.h"
#include "../include/SquareLight.h"
#include "../include/Plano.h"
#include "../include/Cubo.h"
#include "../include/Settings.h"

#define TILE_SIZE_X 10
#define TILE_SIZE_Y 8

class Render
{
public:
	Render();
	~Render();
	void renderTiles(int taskIndex, int threadIndex, const unsigned int width, const unsigned int height, const float time, const ISPCCamera& camara, const RTCScene escena, const int numTilesX, const int numTilesY, PhotonKDTree* kdtree, FIBITMAP* &bitmap, FIBITMAP* &bitmap_directa, FIBITMAP* &bitmap_indirecta, FIBITMAP* &bitmap_causticas, FIBITMAP* &bitmap_especular, tbb::concurrent_vector<tbb::concurrent_vector<Vec3i>>& pixels, tbb::concurrent_vector<tbb::concurrent_vector<Vec3i>>& datos);

private:
	void renderizarPixel(int x, int y, const unsigned int width, const unsigned int height, const float time, const ISPCCamera& camara, RTCScene escena, PhotonKDTree* kdtree, FIBITMAP* &bitmap, FIBITMAP* &bitmap_directa, FIBITMAP* &bitmap_indirecta, FIBITMAP* &bitmap_causticas, FIBITMAP* &bitmap_especular, tbb::concurrent_vector<tbb::concurrent_vector<Vec3i>>& pixels);

};