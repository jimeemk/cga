#include "../include/Object.h"

Object::Object()
{
}

Object::~Object()
{
}

RTCGeometry* Object::getGeometry()
{
	return geometry;
}

Vec3f Object::getDiffuseColor() 
{
	return diffuse_color;
}