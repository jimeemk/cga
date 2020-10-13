#pragma once

#include "Object.h"

using namespace embree;

class Plano : public Object
{
public:
	unsigned int agregarObjeto(RTCDevice, RTCScene);
	Plano();
	Plano(string path, Material material, Vec3fa centro, float escala, Vec3fa rot);
	~Plano();
};