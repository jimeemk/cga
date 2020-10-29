#pragma once

#include "Light.h"

using namespace embree;

class SquareLight: public Light
{
public:
	SquareLight();
	~SquareLight();
	SquareLight(Vec3f, double, double, Vec3f, Vec3f);
	Vec3f getNormal();
	Vec3f getSource();
	Vec3f randomDir(int *);
	std::vector<Vec3f> samplePositions();

private:
	double size;
	Vec3f normal;
	Vec3f right;
};