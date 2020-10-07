#pragma once

#include <stdlib.h>
#include <time.h>

#include "../common/math/vec3.h"
#include "niederreiter2.h"

using namespace embree;

//Luz puntual
class Light 
{
public:
	Light();
	Light(Vec3f, double);
	~Light();
	virtual Vec3f getSource();
	double getPower();
	virtual Vec3f randomDir(int *);
protected:
	Vec3f source;
	double power;
};