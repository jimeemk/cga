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
	Object(string path, Material material, Vec3fa centro, float escala, Vec3fa rot);
	Object(RTCGeometry, Vec3f);
	~Object();
	RTCGeometry getGeometry();
	Vec3f getDiffuseColor();
	Vec3fa* getColoresCaras();
	Vec3fa* getColoresVertices();
	std::vector<float> getVertices();
	std::vector<float> getVertexWeights();
	std::vector<float> getNormales();
	std::vector<float> getTexCoords();
	std::vector<float> getTexCoordsws();
	std::vector<tinyobj::skin_weight_t> getSkinWeights();
	Material getMaterial();
	virtual unsigned int agregarObjeto(RTCDevice, RTCScene);
	void* alignedMalloc2(size_t size, size_t align);
	Vec3fa trasladarVertice(Vec3fa inicial, Vec3fa t);
	Vec3fa rotarVertice(Vec3fa inicial, Vec3fa r);
	Vec3fa escalarVertice(Vec3fa inicial, float e);
protected:
	RTCGeometry geometry;
	Vec3f diffuse_color;
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

	//falta el resto de la info que queramos del material
};