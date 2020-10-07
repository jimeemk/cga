// prueba_embree.cpp: define el punto de entrada de la aplicación.
//

#include "prueba_embree.h"

#define TILE_SIZE_X 10
#define TILE_SIZE_Y 8
#define HEIGHT 800
#define WIDTH 800


using namespace embree;
using namespace std;

Vec3fa* colores_caras;
Vec3fa* colores_vertices;
std::vector<Object*> objetos;
unsigned int datos[WIDTH][HEIGHT][3];
unsigned int pixels[WIDTH][HEIGHT][3];
FIBITMAP* bitmap = FreeImage_Allocate(WIDTH, HEIGHT, 24);

void* alignedMalloc3(size_t size, size_t align)
{
	if (size == 0)
		return nullptr;

	assert((align & (align - 1)) == 0);
	void* ptr = _mm_malloc(size, align);

	if (size != 0 && ptr == nullptr)
		throw std::bad_alloc();

	return ptr;
}
struct Rayo {
	Vec3fa camara;
	Vec3fa dir;
	//Vec3fa org;
	float tnear;
	float tfar;

};

//struct Material {
//	float coefAmbiente;
//	float coefDifuso;
//	float coefEspecular;
//	float coefReflexion;
//	float coefTransparencia;
//	float indiceRefraccion;
//	Vec3fa color;
//};

Vec3fa lightArray[3];

//struct Object {
//	int geomId;
//	Material mat;
//};

RTCRay crearRayo(Vec3fa origen, Vec3fa dir) {
	RTCRay rayo = { origen.x, origen.y, origen.z, 0.f, dir.x, dir.y, dir.z, 0.f, inf };
	return rayo;
}

unsigned int agregarCubo(RTCDevice device, RTCScene escena) {
	
	colores_caras = (Vec3fa*)alignedMalloc3(12 * sizeof(Vec3fa), 16);
	colores_vertices = (Vec3fa*)alignedMalloc3(8 * sizeof(Vec3fa), 16);
	RTCGeometry cubo = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

	Vec3fa* vertices = (Vec3fa*)rtcSetNewGeometryBuffer(cubo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vec3fa), 8);
	int x_offset = 0;
	int y_offset = 3;
	int z_offset = 0;
	colores_vertices[0] = Vec3fa(0, 0, 0); vertices[0].x = -1 + x_offset; vertices[0].y = -1 + y_offset; vertices[0].z = -1 + z_offset;
	colores_vertices[1] = Vec3fa(0, 0, 1); vertices[1].x = -1 + x_offset; vertices[1].y = -1 + y_offset; vertices[1].z = +1 + z_offset;
	colores_vertices[2] = Vec3fa(0, 1, 0); vertices[2].x = -1 + x_offset; vertices[2].y = +1 + y_offset; vertices[2].z = -1 + z_offset;
	colores_vertices[3] = Vec3fa(0, 1, 1); vertices[3].x = -1 + x_offset; vertices[3].y = +1 + y_offset; vertices[3].z = +1 + z_offset;
	colores_vertices[4] = Vec3fa(1, 0, 0); vertices[4].x = +1 + x_offset; vertices[4].y = -1 + y_offset; vertices[4].z = -1 + z_offset;
	colores_vertices[5] = Vec3fa(1, 0, 1); vertices[5].x = +1 + x_offset; vertices[5].y = -1 + y_offset; vertices[5].z = +1 + z_offset;
	colores_vertices[6] = Vec3fa(1, 1, 0); vertices[6].x = +1 + x_offset; vertices[6].y = +1 + y_offset; vertices[6].z = -1 + z_offset;
	colores_vertices[7] = Vec3fa(1, 1, 1); vertices[7].x = +1 + x_offset; vertices[7].y = +1 + y_offset; vertices[7].z = +1 + z_offset;

	int tri = 0;
	Triangulo* triangulos = (Triangulo*)rtcSetNewGeometryBuffer(cubo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangulo), 12);
	
	// left side
	colores_caras[tri] = Vec3fa(1, 0, 0); triangulos[tri].v0 = 0; triangulos[tri].v1 = 1; triangulos[tri].v2 = 2; tri++;
	colores_caras[tri] = Vec3fa(1, 0, 0); triangulos[tri].v0 = 1; triangulos[tri].v1 = 3; triangulos[tri].v2 = 2; tri++;

	// right side
	colores_caras[tri] = Vec3fa(0, 1, 0); triangulos[tri].v0 = 4; triangulos[tri].v1 = 6; triangulos[tri].v2 = 5; tri++;
	colores_caras[tri] = Vec3fa(0, 1, 0); triangulos[tri].v0 = 5; triangulos[tri].v1 = 6; triangulos[tri].v2 = 7; tri++;

	// bottom side
	colores_caras[tri] = Vec3fa(0.5f);  triangulos[tri].v0 = 0; triangulos[tri].v1 = 4; triangulos[tri].v2 = 1; tri++;
	colores_caras[tri] = Vec3fa(0.5f);  triangulos[tri].v0 = 1; triangulos[tri].v1 = 4; triangulos[tri].v2 = 5; tri++;

	// top side
	colores_caras[tri] = Vec3fa(1.0f);  triangulos[tri].v0 = 2; triangulos[tri].v1 = 3; triangulos[tri].v2 = 6; tri++;
	colores_caras[tri] = Vec3fa(1.0f);  triangulos[tri].v0 = 3; triangulos[tri].v1 = 7; triangulos[tri].v2 = 6; tri++;

	// front side
	colores_caras[tri] = Vec3fa(0, 0, 1); triangulos[tri].v0 = 0; triangulos[tri].v1 = 2; triangulos[tri].v2 = 4; tri++;
	colores_caras[tri] = Vec3fa(0, 0, 1); triangulos[tri].v0 = 2; triangulos[tri].v1 = 6; triangulos[tri].v2 = 4; tri++;

	// back side
	colores_caras[tri] = Vec3fa(1, 1, 0); triangulos[tri].v0 = 1; triangulos[tri].v1 = 5; triangulos[tri].v2 = 3; tri++;
	colores_caras[tri] = Vec3fa(1, 1, 0); triangulos[tri].v0 = 3; triangulos[tri].v1 = 5; triangulos[tri].v2 = 7; tri++;


	rtcSetGeometryVertexAttributeCount(cubo, 1);
	Vec3fa* colVertices = &colores_vertices[0];
	rtcSetSharedGeometryBuffer(cubo, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, colVertices, 0, sizeof(Vec3fa), 8);

	rtcCommitGeometry(cubo);
	unsigned int geomID = rtcAttachGeometry(escena, cubo);
	rtcCommitScene(escena);
	rtcReleaseGeometry(cubo);
	return geomID;
}

/* adds a ground plane to the scene */
unsigned int addGroundPlane(RTCDevice device, RTCScene scene) {
	/* create a triangulated plane with 2 triangles and 4 vertices */
	RTCGeometry mesh = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
	/* set vertices */
	Vec3fa* vertices = (Vec3fa*) rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vec3fa), 4);

	vertices[0].x = -20; vertices[0].y = -3; vertices[0].z = -20;
	vertices[1].x = -20; vertices[1].y = -3; vertices[1].z = +20;
	vertices[2].x = 20; vertices[2].y = -3; vertices[2].z = -20;
	vertices[3].x = +20; vertices[3].y = -3; vertices[3].z = +20;

	colores_vertices[0] = Vec3fa(0, 0, 0);
	colores_vertices[1] = Vec3fa(0, 0, 1);
	colores_vertices[2] = Vec3fa(0, 1, 0);
	colores_vertices[3] = Vec3fa(1, 0, 0);

	/* set triangles */
	Triangulo* triangles = (Triangulo*) rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangulo), 2);
	triangles[0].v0 = 0; triangles[0].v1 = 1; triangles[0].v2 = 2; 
	triangles[1].v0 = 1; triangles[1].v1 = 3; triangles[1].v2 = 2;
	colores_caras[0] = Vec3fa(0,0,0.5);
	colores_caras[1] = Vec3fa(0.5,0,0);
	
	rtcSetGeometryVertexAttributeCount(mesh, 1);
	rtcSetSharedGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, colores_vertices, 0, sizeof(Vec3fa), 4);

	rtcCommitGeometry(mesh);
	unsigned int geomID = rtcAttachGeometry(scene, mesh);
	rtcCommitScene(scene);
	rtcReleaseGeometry(mesh);

	return geomID;
}

/* mapea geomID a un Material */
map<int, Material> materialMapping;

//Material mat = { 0.2f, 0.3f, 0.3f, 0.2f, 0, 0, Vec3fa(0.3, 0, 0.3) };

void renderizarPixel(
	int x, int y,
	const unsigned int width,
	const unsigned int height,
	const float time, const ISPCCamera& camara, RTCScene escena, PhotonKDTree* kdtree) {
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	Vec3fa color = Vec3fa(0.0f);
	
	Raytracer raytracer;
	color = raytracer.Raytrace(camara, x, y, escena, context);

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
	
	for (unsigned int y = y0; y < y1; y++) for (unsigned int x = x0; x < x1; x++)
	{
		renderizarPixel(x, y, WIDTH, HEIGHT, time, camara, escena, kdtree);
		datos[HEIGHT - y - 1][x][0] = pixels[y][x][0] * 256 * 256 * 256;
		datos[HEIGHT - y - 1][x][1] = pixels[y][x][1] * 256 * 256 * 256;
		datos[HEIGHT - y - 1][x][2] = pixels[y][x][2] * 256 * 256 * 256;

		color.rgbRed = pixels[y][x][0];
		color.rgbGreen = pixels[y][x][1];
		color.rgbBlue = pixels[y][x][2];
		FreeImage_SetPixelColor(bitmap, x, y, &color);
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
	
	//inicializacion
	RTCDevice device = rtcNewDevice("threads=0");
	RTCScene escena = rtcNewScene(device);
	rtcRetainDevice(device);
	//rtcRetainScene(escena);
	//creacion escena
	//unsigned int cuboID = agregarCubo(device, escena);
	Object* obj = new Object();
	objetos.push_back(obj);

	Object* obj2 = new Object();
	objetos.push_back(obj2);

	unsigned int objetoID = obj->agregarObjeto(device, escena, "Modelos/12221_Cat_v1_l3");
	unsigned int objetoID2 = obj2->agregarObjeto(device, escena, "Modelos/face");

	unsigned int cuboID = agregarCubo(device, escena);
	unsigned int planoID = addGroundPlane(device, escena);

	float time = 0.5f;
	Camera camara;
	camara.from = Vec3fa(10.5f, 10.5f, -10.5f);
	camara.to = Vec3fa(0.0f, 0.0f, 0.0f);
	camara.fov = 90;
	int width = 800;
	int height = 800;

	//prueba photon mapping
	std::vector<Light*> lights;
	SquareLight* light = new SquareLight(Vec3f(0.f, 15.0, 0.f), 10, 6, Vec3f(0.f, -1.f, 0.f), Vec3f(1.f, 0.f, 0.f));
	lights.push_back(light);

	Scene* scene = new Scene(objetos, lights);
	
	PhotonMapper* photon_mapper = new PhotonMapper();
	PhotonKDTree* kdtree = photon_mapper->emitPhotons(scene, 10000);
	std::vector<Photon> neighbors = kdtree->kNNValue(Vec3f(0.f), 10);

	for (int i = 0; i < neighbors.size(); i++)
	{
		cout << neighbors.at(i).point.x << " " << neighbors.at(i).point.y << " " << neighbors.at(i).point.z << "\n";
	}


	const int numTilesX = (WIDTH + TILE_SIZE_X - 1) / TILE_SIZE_X;
	const int numTilesY = (HEIGHT + TILE_SIZE_Y - 1) / TILE_SIZE_Y;
	parallel_for(size_t(0), size_t(numTilesX * numTilesY), [&](const range<size_t>& range) {
		const int threadIndex = (int)TaskScheduler::threadIndex();
		for (size_t i = range.begin(); i < range.end(); i++)
			renderTiles((int)i, threadIndex, WIDTH, HEIGHT, time, camara.getISPCCamera(WIDTH,HEIGHT), escena, numTilesX, numTilesY, kdtree);
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
	
	
		
	rtcCommitScene(escena);
	rtcReleaseScene(escena);
	rtcReleaseDevice(device);
	glfwTerminate();
	return 0;
}

