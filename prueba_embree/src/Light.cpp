#include "../include/Light.h"

Light::Light()
{
}

Light::Light(Vec3f s, double p)
{
	source = s;
	power = p;
}

double Light::getPower()
{
	return power;
}

Vec3f Light::getSource()
{
	return source;
}

Vec3f Light::randomDir(int *seed)
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

Light::~Light()
{
}

Vec3f Light::lightDir(Vec3f point)
{
	return normalize(source - point);
}