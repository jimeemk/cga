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

Vec3f SquareLight::getSource()
{
	return Vec3f(0.f); //aca tiene que retornar un punto random en el cuadrado
}

Vec3f SquareLight::randomDir()
{
	return Vec3f(0.f); //retorna direccion random en funcion de la dist que siguen estas luces
}

Vec3f SquareLight::getNormal()
{
	return normal;
}

Vec3f SquareLight::randomDir()
{

}