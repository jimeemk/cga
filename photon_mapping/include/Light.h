#pragma once

#define CELL_SIZE_X 100
#define CELL_SIZE_Y 100

#include <stdlib.h>
#include <time.h>

#include "../common/math/vec3.h"
#include "niederreiter2.h"
#include <embree3/rtcore_common.h>

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
	Vec3f lightDir(Vec3f);
protected:
	Vec3f source;
	double power;
	bool projectionMap[CELL_SIZE_X][CELL_SIZE_Y];
	int celdas_activas;
	RTCBounds bounds;
};