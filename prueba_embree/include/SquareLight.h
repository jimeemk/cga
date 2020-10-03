#pragma once

#include "Light.h"

class SquareLight: public Light
{
public:
	SquareLight();
	~SquareLight();
	SquareLight(Vec3f, double, double, Vec3f);
	Vec3f getNormal();
private:
	double width;
	Vec3f normal;
};