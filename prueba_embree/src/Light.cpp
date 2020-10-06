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

Light::~Light()
{
}