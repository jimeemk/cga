#include "../include/Render.h"

Render::Render(){}
Render::~Render(){}

using namespace embree;
using namespace std;



void Render::renderizarPixel(
	int x, int y,
	const unsigned int width,
	const unsigned int height,
	const float time, const ISPCCamera& camara, RTCScene escena, PhotonKDTree* kdtree,
	FIBITMAP* &bitmap, FIBITMAP* &bitmap_directa, FIBITMAP* &bitmap_indirecta, FIBITMAP* &bitmap_causticas, FIBITMAP* &bitmap_especular,
	tbb::concurrent_vector<tbb::concurrent_vector<Vec3i>> &pixels) {
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	Vec3fa color = Vec3fa(0.0f);

	Raytracer raytracer;
	color = raytracer.raytrace(camara, x, y, escena, context);

	/* write color to framebuffer */
	unsigned int r = (unsigned int)(255.0f * clamp(color.x, 0.0f, 1.0f));
	unsigned int g = (unsigned int)(255.0f * clamp(color.y, 0.0f, 1.0f));
	unsigned int b = (unsigned int)(255.0f * clamp(color.z, 0.0f, 1.0f));
	pixels.at(y).at(x).x = r;
	pixels.at(y).at(x).y = g;
	pixels.at(y).at(x).z = b;

	RGBQUAD color_especular;
	color_especular.rgbRed = 255.0f * clamp(raytracer.rt_especular.x, 0.0f, 1.0f);
	color_especular.rgbGreen = 255.0f * clamp(raytracer.rt_especular.y, 0.0f, 1.0f);
	color_especular.rgbBlue = 255.0f * clamp(raytracer.rt_especular.z, 0.0f, 1.0f);


	RGBQUAD color_indirecta;
	color_indirecta.rgbRed = 255.0f * clamp(raytracer.rt_indirecta.x, 0.0f, 1.0f);
	color_indirecta.rgbGreen = 255.0f * clamp(raytracer.rt_indirecta.y, 0.0f, 1.0f);
	color_indirecta.rgbBlue = 255.0f * clamp(raytracer.rt_indirecta.z, 0.0f, 1.0f);


	FreeImage_SetPixelColor(bitmap_especular, x, height - y - 1, &color_especular);
	FreeImage_SetPixelColor(bitmap_indirecta, x, height - y - 1, &color_indirecta);

}

void Render::renderTiles(int taskIndex, int threadIndex,
	const unsigned int width,
	const unsigned int height,
	const float time,
	const ISPCCamera& camara,
	const RTCScene escena,
	const int numTilesX,
	const int numTilesY, PhotonKDTree* kdtree, 
	FIBITMAP* &bitmap, FIBITMAP* &bitmap_directa, FIBITMAP* &bitmap_indirecta, FIBITMAP* &bitmap_causticas, FIBITMAP* &bitmap_especular,
	tbb::concurrent_vector<tbb::concurrent_vector<Vec3i>>& pixels, tbb::concurrent_vector<tbb::concurrent_vector<Vec3i>>& datos)
{
	const unsigned int tileY = taskIndex / numTilesX;
	const unsigned int tileX = taskIndex - tileY * numTilesX;
	const unsigned int x0 = tileX * TILE_SIZE_X;
	const unsigned int x1 = min(x0 + (INT64)TILE_SIZE_X, (INT64)width);
	const unsigned int y0 = tileY * TILE_SIZE_Y;
	const unsigned int y1 = min(y0 + (INT64)TILE_SIZE_Y, (INT64)height);
	RGBQUAD color;
	glClear(GL_COLOR_BUFFER_BIT);
	for (unsigned int y = y0; y < y1; y++) for (unsigned int x = x0; x < x1; x++)
	{
		renderizarPixel(x, y, width, height, time, camara, escena, kdtree, bitmap, bitmap_directa, bitmap_indirecta, bitmap_causticas, bitmap_especular, pixels);
		datos.at(height - y - 1).at(x).x = pixels.at(y).at(x).x * 256 * 256 * 256;
		datos.at(height - y - 1).at(x).y = pixels.at(y).at(x).y * 256 * 256 * 256;
		datos.at(height - y - 1).at(x).z = pixels.at(y).at(x).z * 256 * 256 * 256;
		color.rgbRed = pixels.at(y).at(x).x;
		color.rgbGreen = pixels.at(y).at(x).y;
		color.rgbBlue = pixels.at(y).at(x).z;

		FreeImage_SetPixelColor(bitmap, x, height - y - 1, &color);
	}
}
