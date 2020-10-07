#pragma once

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include "../common/math/vec3.h"
#include "../common/camera.h"
#include "../common/ray.h"
#include <iostream>


using namespace embree;

struct BRDF {
	float Ns;               /*< specular exponent */
	float Ni;               /*< optical density for the surface (index of refraction) */
	Vec3fa Ka;              /*< ambient reflectivity */
	Vec3fa Kd;              /*< diffuse reflectivity */
	Vec3fa Ks;              /*< specular reflectivity */
	Vec3fa Kt;              /*< transmission filter */
};	

struct Luz {
	Vec3fa pos;
};

struct Material {
	float coefAmbiente;
	float coefDifuso;
	float coefEspecular;
	float coefReflexion;
	float coefTransparencia;
	float indiceRefraccion;
	Vec3fa color;
};

class Raytracer
{
public:
	Raytracer();

	Vec3fa Raytrace(const ISPCCamera& camara, int x, int y, RTCScene escena, RTCIntersectContext& context);
	
	Vec3fa Traza(Ray rayo, int profundidad, RTCScene escena, RTCIntersectContext& context);
	
	Vec3fa Sombra(RTCScene escena, RTCIntersectContext& context, Ray rayo, int profundidad);

private:
	int profundidad_max = 3;
	

};
