#pragma once

#include "Light.h"

class PointLight: public Light
{
public:
	PointLight();
	~PointLight();
	PointLight(Vec3f, double);
	Vec3f getSource();
	Vec3f randomDir();
private:

};