#pragma once

#include <embree3/rtcore.h>
#include <vector>

#include "SquareLight.h"
#include "Object.h"

class Scene
{
public:
	Scene();
	Scene(std::vector<Object*>, std::vector<Light*>);
	~Scene();
	std::vector<Object*> getObjects();
	std::vector<Light*> getLights();

private:
	std::vector<Object*> objects;
	std::vector<Light*> lights; //en futuro seran multiples luces
};