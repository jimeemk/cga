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
	virtual Vec3f randomDir()=0;
	virtual Vec3f getSource()=0;
	double getPower();
protected:
	Vec3f source;
	double power;
};