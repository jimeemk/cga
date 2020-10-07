#pragma once

#include <embree3/rtcore.h>

#include "../common/math/vec3.h"

using namespace embree;

class Object
{
public:
	Object();
	Object(RTCGeometry, Vec3f);
	~Object();
	RTCGeometry getGeometry();
	Vec3f getDiffuseColor();

private:
	RTCGeometry geometry;
	Vec3f diffuse_color;
	//falta el resto de la info que queramos del material
};