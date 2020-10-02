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
unsigned int datos[WIDTH][HEIGHT][3];
unsigned int pixels[WIDTH][HEIGHT][3];
FIBITMAP* bitmap = FreeImage_Allocate(WIDTH, HEIGHT, 24);

void* alignedMalloc2(size_t size, size_t align)
{
	if (size == 0)
		return nullptr;

	assert((align & (align - 1)) == 0);
	void* ptr = _mm_malloc(size, align);

	if (size != 0 && ptr == nullptr)
		throw std::bad_alloc();

	return ptr;
}

struct Triangulo {
	unsigned int v1;
	unsigned int v2;
	unsigned int v0;
};
struct Vertice {
	float x;
	float y;
	float z;
};
struct Rayo {
	Vec3fa camara;
	Vec3fa direccion;
	float tnear;
	float tfar;

};
RTCRay crearRayo(Vec3fa origen, Vec3fa dir) {
	RTCRay rayo = { origen.x, origen.y, origen.z, 0.f, dir.x, dir.y, dir.z, 0.f, inf };
	return rayo;
}

unsigned int agregarCubo(RTCDevice device, RTCScene escena) {
	
	colores_caras = (Vec3fa*)alignedMalloc2(12 * sizeof(Vec3fa), 16);
	colores_vertices = (Vec3fa*)alignedMalloc2(8 * sizeof(Vec3fa), 16);
	RTCGeometry cubo = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

	Vertice* vertices = (Vertice*)rtcSetNewGeometryBuffer(cubo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertice), 8);

	colores_vertices[0] = Vec3fa(0, 0, 0); vertices[0].x = -1; vertices[0].y = -1; vertices[0].z = -1;
	colores_vertices[1] = Vec3fa(0, 0, 1); vertices[1].x = -1; vertices[1].y = -1; vertices[1].z = +1;
	colores_vertices[2] = Vec3fa(0, 1, 0); vertices[2].x = -1; vertices[2].y = +1; vertices[2].z = -1;
	colores_vertices[3] = Vec3fa(0, 1, 1); vertices[3].x = -1; vertices[3].y = +1; vertices[3].z = +1;
	colores_vertices[4] = Vec3fa(1, 0, 0); vertices[4].x = +1; vertices[4].y = -1; vertices[4].z = -1;
	colores_vertices[5] = Vec3fa(1, 0, 1); vertices[5].x = +1; vertices[5].y = -1; vertices[5].z = +1;
	colores_vertices[6] = Vec3fa(1, 1, 0); vertices[6].x = +1; vertices[6].y = +1; vertices[6].z = -1;
	colores_vertices[7] = Vec3fa(1, 1, 1); vertices[7].x = +1; vertices[7].y = +1; vertices[7].z = +1;

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



void renderizarPixel(
	int x, int y,
	const unsigned int width,
	const unsigned int height,
	const float time, const ISPCCamera& camara, RTCScene escena)
{
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	/* initialize ray */
	Rayo rayo = { Vec3fa(camara.xfm.p), Vec3fa(normalize(x * camara.xfm.l.vx + y * camara.xfm.l.vy + camara.xfm.l.vz)), 0.0f, inf };

	/* intersect ray with scene */
	RTCHit hi = RTCHit();
	hi.geomID = RTC_INVALID_GEOMETRY_ID;
	RTCRay ra = RTCRay({rayo.camara.x,rayo.camara.y, rayo.camara.z, rayo.tnear, rayo.direccion.x,rayo.direccion.y, rayo.direccion.z, 0, rayo.tfar, 0, 1, 0});

	RTCRayHit* rh = new RTCRayHit({ra,hi});
	
	rtcIntersect1(escena, &context, rh);

	//RayStats_addRay(stats);

	/* shade pixels */
	Vec3fa color = Vec3fa(0.0f);
	if (rh->hit.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		Vec3fa diffuse = colores_caras[rh->hit.primID];
		color = color + diffuse * 0.8f;
		//Vec3fa lightDir = normalize(Vec3fa(-1, -1, -1));

		/* initialize shadow ray */
		//Ray shadow(ray.org + ray.tfar * ray.dir, neg(lightDir), 0.001f, inf, 0.0f);

		/* trace shadow ray */
		//rtcOccluded1(data.g_scene, &context, RTCRay_(shadow));
		//RayStats_addShadowRay(stats);

		/* add light contribution */
		//if (shadow.tfar >= 0.0f)
		//	color = color + diffuse * clamp(-dot(lightDir, normalize(ray.Ng)), 0.0f, 1.0f);
	}

	/* write color to framebuffer */
	unsigned int r = (unsigned int)(255.0f * clamp(color.x, 0.0f, 1.0f));
	unsigned int g = (unsigned int)(255.0f * clamp(color.y, 0.0f, 1.0f));
	unsigned int b = (unsigned int)(255.0f * clamp(color.z, 0.0f, 1.0f));
	pixels[y][x][0] = r;
	pixels[y][x][1] = g;
	pixels[y][x][2] = b;

	// pixels[y * width + x].x = r;
	// pixels[y * width + x].y = g;
	// pixels[y * width + x].z = b;
	
}

void renderTiles(int taskIndex, int threadIndex,
	const unsigned int width,
	const unsigned int height,
	const float time,
	const ISPCCamera& camara,
	const RTCScene escena,
	const int numTilesX,
	const int numTilesY)
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
		renderizarPixel(x, y, WIDTH, HEIGHT, time, camara, escena);
		datos[HEIGHT - y - 1][x][0] = pixels[y][x][0] * 256 * 256 * 256;
		datos[HEIGHT - y - 1][x][1] = pixels[y][x][1] * 256 * 256 * 256;
		datos[HEIGHT - y - 1][x][2] = pixels[y][x][2] * 256 * 256 * 256;

		color.rgbRed = pixels[y][x][0];
		color.rgbGreen = pixels[y][x][1];
		color.rgbBlue = pixels[y][x][2];
		FreeImage_SetPixelColor(bitmap, x, y, &color);
	}
}


unsigned int cargarObjeto(RTCDevice device, RTCScene escena, string path) {

	

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
		throw std::runtime_error(warn + err);
	}

	RTCGeometry objeto = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

	colores_caras = (Vec3fa*)alignedMalloc2(shapes[0].mesh.num_face_vertices.size() * sizeof(Vec3fa), 16);
	colores_vertices = (Vec3fa*)alignedMalloc2(attrib.vertices.size() * sizeof(Vec3fa), 16);

	Vec3fa* vertices = (Vec3fa*)rtcSetNewGeometryBuffer(objeto, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vec3fa), attrib.vertices.size());
	for (int i = 0, ver = 0; i < attrib.vertices.size(); ver++)
	{
		float colores[3];
		vertices[ver].x = attrib.vertices.at(i); colores[0]= attrib.colors.at(i); i++;
		vertices[ver].y = attrib.vertices.at(i); colores[1] = attrib.colors.at(i); i++;
		vertices[ver].z = attrib.vertices.at(i); colores[2] = attrib.colors.at(i); i++;
		Vec3fa color = { 0.5, 0.5,0.5 };
		colores_vertices[ver] = color;
		//colores_vertices.push_back(color);
	}

	int tri = 0;
	Triangulo* triangulos = (Triangulo*)rtcSetNewGeometryBuffer(objeto, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangulo), shapes[0].mesh.num_face_vertices.size());

	for (int i=0; tri < shapes[0].mesh.num_face_vertices.size(); tri++)
	{
		triangulos[tri].v0 = shapes[0].mesh.indices.at(i).vertex_index; i++;
		triangulos[tri].v1 = shapes[0].mesh.indices.at(i).vertex_index; i++;
		triangulos[tri].v2 = shapes[0].mesh.indices.at(i).vertex_index; i++;
		//colores_caras.push_back({ (float)shapes[0].mesh.material_ids.at(tri), (float)shapes[0].mesh.material_ids.at(tri), (float)shapes[0].mesh.material_ids.at(tri) });
		//colores_caras.push_back({0.2,0.7,0.9});
		Vec3fa color = { 0.5, 0.5,0.5 };
		colores_caras[tri] = color;
	}
		rtcSetGeometryVertexAttributeCount(objeto, 1);
		Vec3fa* colVertices = &colores_vertices[0];
		rtcSetSharedGeometryBuffer(objeto, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, colVertices, 0, sizeof(Vec3fa), attrib.vertices.size());
		rtcCommitGeometry(objeto);
		unsigned int geomID = rtcAttachGeometry(escena, objeto);
		rtcCommitScene(escena);
		rtcReleaseGeometry(objeto);
		return geomID;
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
	unsigned int objetoID = cargarObjeto(device, escena, "Modelos/gato.obj");


	float time = 0.5f;
	Camera camara;
	camara.from = Vec3fa(10.5f, 10.5f, -10.5f);
	camara.to = Vec3fa(0.0f, 0.0f, 0.0f);
	int width = 800;
	int height = 800;
	



	const int numTilesX = (WIDTH + TILE_SIZE_X - 1) / TILE_SIZE_X;
	const int numTilesY = (HEIGHT + TILE_SIZE_Y - 1) / TILE_SIZE_Y;
	parallel_for(size_t(0), size_t(numTilesX * numTilesY), [&](const range<size_t>& range) {
		const int threadIndex = (int)TaskScheduler::threadIndex();
		for (size_t i = range.begin(); i < range.end(); i++)
			renderTiles((int)i, threadIndex, WIDTH, HEIGHT, time, camara.getISPCCamera(WIDTH,HEIGHT), escena, numTilesX, numTilesY);
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

