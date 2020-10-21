// prueba_embree.cpp: define el punto de entrada de la aplicación.
//

#include "prueba_embree.h"

#define TILE_SIZE_X 10
#define TILE_SIZE_Y 8
#define HEIGHT 800
#define WIDTH 800


using namespace embree;
using namespace std;

unsigned int datos[WIDTH][HEIGHT][3];
unsigned int pixels[WIDTH][HEIGHT][3];
FIBITMAP* bitmap = FreeImage_Allocate(WIDTH, HEIGHT, 24);

struct Rayo {
	Vec3fa camara;
	Vec3fa dir;
	//Vec3fa org;
	float tnear;
	float tfar;

};

Vec3fa lightArray[3];

RTCRay crearRayo(Vec3fa origen, Vec3fa dir) {
	RTCRay rayo = { origen.x, origen.y, origen.z, 0.f, dir.x, dir.y, dir.z, 0.f, inf };
	return rayo;
}

void renderizarPixel(
	int x, int y,
	const unsigned int width,
	const unsigned int height,
	const float time, const ISPCCamera& camara, RTCScene escena, PhotonKDTree* kdtree) {
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	Vec3fa color = Vec3fa(0.0f);
	
	Raytracer raytracer;
	color = raytracer.raytrace(camara, x, y, escena, context);

	/* write color to framebuffer */
	unsigned int r = (unsigned int)(255.0f * clamp(color.x, 0.0f, 1.0f));
	unsigned int g = (unsigned int)(255.0f * clamp(color.y, 0.0f, 1.0f));
	unsigned int b = (unsigned int)(255.0f * clamp(color.z, 0.0f, 1.0f));
	pixels[y][x][0] = r;
	pixels[y][x][1] = g;
	pixels[y][x][2] = b;
	
}

void renderTiles(int taskIndex, int threadIndex,
	const unsigned int width,
	const unsigned int height,
	const float time,
	const ISPCCamera& camara,
	const RTCScene escena,
	const int numTilesX,
	const int numTilesY, PhotonKDTree* kdtree)
{
	const unsigned int tileY = taskIndex / numTilesX;
	const unsigned int tileX = taskIndex - tileY * numTilesX;
	const unsigned int x0 = tileX * TILE_SIZE_X;
	const unsigned int x1 = min(x0 + (INT64)TILE_SIZE_X, (INT64)WIDTH);
	const unsigned int y0 = tileY * TILE_SIZE_Y;
	const unsigned int y1 = min(y0 + (INT64)TILE_SIZE_Y, (INT64)HEIGHT);
	RGBQUAD color;
	glClear(GL_COLOR_BUFFER_BIT);
	for (unsigned int y = y0; y < y1; y++) for (unsigned int x = x0; x < x1; x++)
	{
		renderizarPixel(x, y, WIDTH, HEIGHT, time, camara, escena, kdtree);
		datos[HEIGHT - y - 1][x][0] = pixels[y][x][0] * 256 * 256 * 256;
		datos[HEIGHT - y - 1][x][1] = pixels[y][x][1] * 256 * 256 * 256;
		datos[HEIGHT - y - 1][x][2] = pixels[y][x][2] * 256 * 256 * 256;
		color.rgbRed = pixels[y][x][0];
		color.rgbGreen = pixels[y][x][1];
		color.rgbBlue = pixels[y][x][2];

		FreeImage_SetPixelColor(bitmap, x, HEIGHT - y - 1, &color);
	}
}

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	FreeImage_Initialise();
	
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIDTH, HEIGHT, "Imagen", NULL, NULL);
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
	Settings* s = Settings::getInstance();
	s->cargarConfiguraciones();
	//rtcRetainScene(escena);
	//creacion escena

	float time = 0.5f;
	
	const int numTilesX = (WIDTH + TILE_SIZE_X - 1) / TILE_SIZE_X;
	const int numTilesY = (HEIGHT + TILE_SIZE_Y - 1) / TILE_SIZE_Y;
	parallel_for(size_t(0), size_t(numTilesX * numTilesY), [&](const range<size_t>& range) {
		const int threadIndex = (int)TaskScheduler::threadIndex();
		for (size_t i = range.begin(); i < range.end(); i++)
		{
			renderTiles((int)i, threadIndex, WIDTH, HEIGHT, time, Settings::getInstance()->getCamara().getISPCCamera(WIDTH, HEIGHT), s->getEscena(), numTilesX, numTilesY, s->getKdTree());
			glClear(GL_COLOR_BUFFER_BIT);
			/* Swap front and back buffers */
			glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_INT, datos);
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	});

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y-%H-%M-%S");
	string str = oss.str();
	string nombre = str + ".png";

	if (FreeImage_Save(FIF_PNG, bitmap, nombre.c_str(), 0)) {
		cout << "Image saved" << endl;
		//Mati le puse ese nombre para que quede con la fecha/hora, tambien lo subi para que se genere antes del while -Jime 
	}

	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		
		glClear(GL_COLOR_BUFFER_BIT);
		/* Swap front and back buffers */
		glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_INT, datos);
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

