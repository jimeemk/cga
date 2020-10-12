#pragma once

#include <embree3/rtcore.h>
#include <vector>

#include "SquareLight.h"
#include "Object.h"

class Scene
{
public:
	Scene();
	Scene(std::string, std::vector<Object*>, std::vector<Light*>);
	~Scene();
	std::vector<Object*> getObjects();
	std::vector<Light*> getLights();
	std::string getNombre();

private:
	std::vector<Object*> objects;
	std::vector<Light*> lights;
	std::string nombre;
};