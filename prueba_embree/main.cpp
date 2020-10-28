// prueba_embree.cpp: define el punto de entrada de la aplicación.
//

#define TINYOBJLOADER_IMPLEMENTATION

#include <iostream>

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include "common\ray.h"
#include "common\math\vec2fa.h"
#include "common\math\vec3fa.h"
#include "common\math\math.h"
#include "common/camera.h"
#include <GLFW/glfw3.h>
#include <tbb/tbb.h>
#include "common/algorithms/parallel_for.h"
#include "common/tasking/taskscheduler.h"
#include <FreeImage.h>
#include <map>
#include "include/Raytracer.h"
#include "include/PhotonMapper.h"
#include "include/Scene.h"
#include "include/SquareLight.h"
#include "include/Plano.h"
#include "include/Cubo.h"
#include "include/Settings.h"
#include "include/Render.h"
#include "include/Xml.h"


using namespace embree;
using namespace std;

tbb::concurrent_vector<tbb::concurrent_vector<Vec3i>> datos;
tbb::concurrent_vector<tbb::concurrent_vector<Vec3i>> pixels;

int main()
{

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	FreeImage_Initialise();
	
	Settings* s = Settings::getInstance();
	s->cargarConfiguraciones();
	//rtcRetainScene(escena);
	//creacion escena
	int height = s->getHeight();
	int width = s->getWidth();

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Imagen", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glClearColor(0.4f, 0.3f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	/* Loop until the user closes the window */
	
	//Initialize todo
	//Primero inicializo datos y pixels
	for (int j = 0; j < height; j++)
	{
		tbb::concurrent_vector<Vec3i> v;
		for (int k = 0; k < width; k++)
		{
			v.push_back(Vec3i(0, 0, 0));
		}
		datos.push_back(v);
		pixels.push_back(v);
	}

	float time = 0.5f;
	FIBITMAP* bitmap = FreeImage_Allocate(width, height, 24);
	FIBITMAP* bitmap_directa = FreeImage_Allocate(width, height, 24);
	FIBITMAP* bitmap_especular = FreeImage_Allocate(width, height, 24);
	FIBITMAP* bitmap_causticas = FreeImage_Allocate(width, height, 24);
	FIBITMAP* bitmap_indirecta = FreeImage_Allocate(width, height, 24);
	const int numTilesX = (width + TILE_SIZE_X - 1) / TILE_SIZE_X;
	const int numTilesY = (height + TILE_SIZE_Y - 1) / TILE_SIZE_Y;
	embree::parallel_for(size_t(0), size_t(numTilesX * numTilesY), [&](const range<size_t>& range) {
		const int threadIndex = (int)TaskScheduler::threadIndex();
		for (size_t i = range.begin(); i < range.end(); i++)
		{
			Render* r = new Render();
			r->renderTiles((int)i, threadIndex, width, height, time, Settings::getInstance()->getCamara().getISPCCamera(width, height), s->getEscena(), numTilesX, numTilesY, s->getKdTree(), bitmap,bitmap_directa, bitmap_indirecta, bitmap_causticas, bitmap_especular, pixels, datos);
		}
	});

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y-%H-%M-%S");
	string str = oss.str();
	string nombre = str + ".png";
	string nombre_especular = "especular_" + str + ".png";
	string nombre_indirecta = "indirecta_" + str + ".png";

	if (FreeImage_Save(FIF_PNG, bitmap, nombre.c_str(), 0)) {
		cout << "Image saved" << endl;
	}

	if (FreeImage_Save(FIF_PNG, bitmap_especular, nombre_especular.c_str(), 0)) {
		cout << "Image saved" << endl;
	}
	
	if (FreeImage_Save(FIF_PNG, bitmap_indirecta, nombre_indirecta.c_str(), 0)) {
		cout << "Image saved" << endl;
	}
	//Esto es necesario para pasar de concurrent vector a vector normal. Un vector normal no se puede usar en un parallel for.
	std::vector<int>* aux= new std::vector<int>();
	for (int i = 0; i < datos.size(); i++)
	{
		for (int j = 0; j < datos.at(i).size(); j++)
		{
			aux->push_back(datos.at(i).at(j).x);
			aux->push_back(datos.at(i).at(j).y);
			aux->push_back(datos.at(i).at(j).z);
		}
	}
	int* dat = aux->data();
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		
		glClear(GL_COLOR_BUFFER_BIT);
		/* Swap front and back buffers */
		glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_INT,dat);
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	
	rtcCommitScene(Settings::getInstance()->getEscena());
	rtcReleaseScene(Settings::getInstance()->getEscena());
	rtcReleaseDevice(Settings::getInstance()->getDevice());
	glfwTerminate();
	return 0;
}

