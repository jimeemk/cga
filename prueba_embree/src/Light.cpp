#include "../include/Light.h"

Light::Light()
{
}

Light::Light(Vec3f s, Vec3f n, double p)
{
	source = s;
	normal = normalize(n);
	power = p;
}

Vec3f Light::randomDir() //genera un vector unitario desde la fuente luminosa "random" 
{
	return Vec3f(0.f);
}

Vec3f Light::getSource()
{
	return source;
}

double Light::getPower()
{
	return power;
}

Light::~Light()
{
}