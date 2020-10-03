#include "../include/PointLight.h"

PointLight::PointLight() : Light()
{
}

PointLight::PointLight(Vec3f s, double p) : Light(s, p)
{

}

Vec3f PointLight::getSource()
{
	return source;
}

Vec3f PointLight::randomDir()
{
	return Vec3f(0.f);
}

PointLight::~PointLight()
{
}