#include "../include/SquareLight.h"

SquareLight::SquareLight() : Light()
{
}

SquareLight::~SquareLight()
{
}

SquareLight::SquareLight(Vec3f s, double p, double w, Vec3f n) : Light(s, p)
{
	width = w;
	normal = normalize(n);
}

Vec3f SquareLight::getSource() //por ahora se asume siempre paralela a los ejes
{
	return source;
}

Vec3f SquareLight::randomDir(int *seed)
{
	double nums[3];
	Vec3f dir = Vec3f(0.f);
	do
	{
		niederreiter2(3, seed, nums);
		dir.x = (nums[0] * 2) + 1;
		dir.y = (nums[1] * 2) + 1;
		dir.z = (nums[2] * 2) + 1;
	} while (distance(dir, Vec3f(0.f)) > 1.0);
	return normalize(dir);
}

Vec3f SquareLight::getNormal()
{
	return normal;
}