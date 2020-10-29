#pragma once

#include "Object.h"

using namespace embree;

class Cubo : public Object
{
public:
	Cubo();
	Cubo(string path, Material material, Vec3fa centro, float escala, Vec3fa rot);
	~Cubo();
	unsigned int agregarObjeto(RTCDevice, RTCScene);
	
};