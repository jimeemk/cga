#pragma once

#include <embree3/rtcore.h>
#include <vector>

#include "SquareLight.h"
#include "Object.h"

class Scene
{
public:
	Scene();
	Scene(std::vector<Object*>, SquareLight*);
	~Scene();
	std::vector<Object*> getObjects();
	void addObject(Object*);
	SquareLight* getLight();
	void addLight(Light*);

private:
	std::vector<Object*> objects;
	SquareLight* light; //en futuro seran multiples luces
};