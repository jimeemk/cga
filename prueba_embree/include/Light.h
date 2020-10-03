#pragma once

#include "../common/math/vec3.h"
#include <random>

using namespace embree;

class Light //por ahora es una luz cuadrada
{
public:
	Light();
	Light(Vec3f, double);
	~Light();
	Vec3f randomDir();
	Vec3f getSource();
	double getPower();
protected:
	Vec3f source;
	double power;
};