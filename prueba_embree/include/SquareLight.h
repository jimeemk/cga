#pragma once

#include "Light.h"
#include "niederreiter2.h"

class SquareLight: public Light
{
public:
	SquareLight();
	~SquareLight();
	SquareLight(Vec3f, double, double, Vec3f);
	Vec3f getNormal();
	Vec3f getSource();
	Vec3f randomDir(int *);
private:
	double width;
	Vec3f normal;
};