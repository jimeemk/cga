#pragma once 

#include "Object.h"
#include <embree3/rtcore_geometry.h>
#include "../common/ray.h"
#include <math.h>

class Esfera : public Object
{
public:
	Esfera();
	Esfera(Material, Vec3fa, float);
	~Esfera();
	unsigned int agregarObjeto(RTCDevice, RTCScene);
	float getRadio();
	Vec3fa getCentro();
	RTCBounds getBounds();
private:
	float radio;
};