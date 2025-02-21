#pragma once

#include <embree3/rtcore.h>
#include "tiny_obj_loader.h"

#include "../common/math/vec3.h"

using namespace embree;

using namespace std;

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
struct Material {
	float coef_ambiente;
	float coef_difuso;
	float coef_especular;
	float coef_reflexion;
	float coef_transparencia;
	float indice_refraccion;
	Vec3fa color;
};
class Object
{
public:
	Object();
	Object(string, Material, Vec3fa, float, Vec3fa);
	Object(Material, Vec3fa, float, Vec3fa);
	Object(RTCGeometry, Vec3fa);
	Object(Material, Vec3fa);
	~Object();
	RTCGeometry getGeometry();
	Vec3fa* getColoresCaras();
	Vec3fa* getColoresVertices();
	Material getMaterial();
	virtual unsigned int agregarObjeto(RTCDevice, RTCScene);
	virtual RTCBounds getBounds();
protected:
	RTCGeometry geometry;
	Vec3fa* colores_caras;
	Vec3fa* colores_vertices;
	string path;
	Vec3fa centro;
	float escalamiento;
	Vec3fa rotacion;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	Material material;
	float maxX, minX, maxY, minY, maxZ, minZ;

	//funciones
	void* alignedMalloc2(size_t size, size_t align);
	Vec3fa trasladarVertice(Vec3fa inicial, Vec3fa t);
	Vec3fa rotarVertice(Vec3fa inicial, Vec3fa r);
	Vec3fa escalarVertice(Vec3fa inicial, float e);
	float getLadoMax(float a, float b, float c);

	//falta el resto de la info que queramos del material
};