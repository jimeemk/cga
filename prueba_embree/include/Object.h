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

class Object
{
public:
	Object();
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

	unsigned int agregarObjeto(RTCDevice, RTCScene, string);
	
private:
	RTCGeometry geometry;
	Vec3f diffuse_color;
	Vec3fa* colores_caras;
	Vec3fa* colores_vertices;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	//falta el resto de la info que queramos del material
};