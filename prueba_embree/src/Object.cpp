#include "../include/Object.h"

Object::Object()
{
}

Object::~Object()
{
}

Object::Object(RTCGeometry geo, Vec3f c)
{
	geometry = geo;
	diffuse_color = c;
}

RTCGeometry Object::getGeometry()
{
	return geometry;
}

Vec3f Object::getDiffuseColor() 
{
	return diffuse_color;
}