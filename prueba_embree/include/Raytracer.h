#pragma once

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include "../common/math/vec3.h"
#include "../common/camera.h"
#include "../common/ray.h"
#include <iostream>
#include "Object.h"
#include "Settings.h"
#include <map>
#include <list>

using namespace embree;

struct Luz {
	Vec3fa pos;
};



class Raytracer
{
public:
	float coeficiente_guardado = 1.00029;
	Raytracer();
	Vec3fa raytrace(const ISPCCamera& camara, int x, int y, RTCScene escena, RTCIntersectContext& context);
	Vec3fa traza(Ray rayo, int profundidad, RTCScene escena, RTCIntersectContext& context);
	Vec3fa sombra(RTCScene escena, RTCIntersectContext& context, Ray rayo, int profundidad, int geomID);
	float procesarOclusion(Vec3fa origen, Vec3fa direccion_luz, RTCScene escena, RTCIntersectContext context);
	Vec3f refract(Vec3fa I, Vec3fa N, float indice_refraccion);

private:
	int profundidad_max = 5;

};
