#pragma once

#include "Object.h"

using namespace embree;

class Plano : public Object
{
public:
	unsigned int agregarObjeto(RTCDevice, RTCScene);
	Plano();
	Plano(Material, Vec3fa, float, Vec3fa, Vec3fa);
	~Plano();
private:
	Vec3fa normal;
};