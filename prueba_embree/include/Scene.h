#pragma once

#include <embree3/rtcore.h>
#include <vector>

#include "Light.h"
#include "Object.h"

class Scene
{
public:
	Scene();
	Scene(std::vector<Object*>, Light*);
	~Scene();
	std::vector<Object*> getObjects();
	void addObject(Object*);
	Light* getLight();
	void addLight(Light*);

private:
	std::vector<Object*> objects;
	Light* light; //en futuro seran multiples luces
};