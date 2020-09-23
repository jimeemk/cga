// prueba_embree.cpp: define el punto de entrada de la aplicación.
//

#include "prueba_embree.h"

using namespace embree;
using namespace std;

Vec3fa colores_vertices[8];
Vec3fa colores_caras[12];

struct Triangulo {
	unsigned int v1;
	unsigned int v2;
	unsigned int v0;
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
	
	RTCGeometry cubo = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

	Vec3fa* vertices = (Vec3fa*)rtcSetNewGeometryBuffer(cubo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vec3fa), 8);

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
	colores_caras[tri] = Vec3fa(1, 1, 0);  triangulos[tri].v0 = 2; triangulos[tri].v1 = 3; triangulos[tri].v2 = 6; tri++;
	colores_caras[tri] = Vec3fa(1, 1, 0);  triangulos[tri].v0 = 3; triangulos[tri].v1 = 7; triangulos[tri].v2 = 6; tri++;

	// front side
	colores_caras[tri] = Vec3fa(0, 0, 1); triangulos[tri].v0 = 0; triangulos[tri].v1 = 2; triangulos[tri].v2 = 4; tri++;
	colores_caras[tri] = Vec3fa(0, 0, 1); triangulos[tri].v0 = 2; triangulos[tri].v1 = 6; triangulos[tri].v2 = 4; tri++;

	// back side
	colores_caras[tri] = Vec3fa(1, 1, 0); triangulos[tri].v0 = 1; triangulos[tri].v1 = 5; triangulos[tri].v2 = 3; tri++;
	colores_caras[tri] = Vec3fa(1, 1, 0); triangulos[tri].v0 = 3; triangulos[tri].v1 = 5; triangulos[tri].v2 = 7; tri++;

	rtcSetGeometryVertexAttributeCount(cubo, 1);

	rtcSetSharedGeometryBuffer(cubo, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, colores_vertices, 0, sizeof(Vec3fa), 8);

	rtcCommitGeometry(cubo);
	unsigned int geomID = rtcAttachGeometry(escena, cubo);
	rtcCommitScene(escena);
	rtcReleaseGeometry(cubo);
	return geomID;
}



void renderizarPixel(
	int x, int y,
	Vec3fa* pixels,
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
	pixels[y * width + x].x = r;
	pixels[y * width + x].y = g;
	pixels[y * width + x].z = b;

}



int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(160, 160, "Imagen", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glClearColor(0.4f, 0.3f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	unsigned int data[160][160][3];
	/* Loop until the user closes the window */
	
	//inicializacion
	RTCDevice device = rtcNewDevice("threads=0");
	RTCScene escena = rtcNewScene(device);
	rtcRetainDevice(device);
	//rtcRetainScene(escena);
	//creacion escena
	unsigned int cuboID = agregarCubo(device, escena);

	const unsigned int tileY = 0;
	const unsigned int tileX = 0;
	const unsigned int x0 = 0;
	const unsigned int x1 = 160;
	const unsigned int y0 = 0;
	const unsigned int y1 = 160;
	Vec3fa pixels[160*160];


	float time = 0.5f;
	Camera camara;
	camara.from = Vec3fa(1.5f, 1.5f, -1.5f);
	camara.to = Vec3fa(0.0f, 0.0f, 0.0f);

	for (unsigned int y = y0; y < y1; y++) for (unsigned int x = x0; x < x1; x++)
	{
		renderizarPixel(x, y, pixels, 160, 160, time, camara.getISPCCamera(160, 160), escena);
		int pixLocation = (y * 160) + (x);
		data[y][x][0] = pixels[pixLocation].x * 256 * 256 * 256;
		data[y][x][1] = pixels[pixLocation].y * 256 * 256 * 256;
		data[y][x][2] = pixels[pixLocation].z * 256 * 256 * 256;
	}

	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		
		glClear(GL_COLOR_BUFFER_BIT);
		/* Swap front and back buffers */
		glDrawPixels(160, 160, GL_RGB, GL_UNSIGNED_INT, data);
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
