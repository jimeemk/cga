#pragma once

#include <embree3/rtcore.h>
#include <vector>

#include "SquareLight.h"
#include "Object.h"
#include "Plano.h"
#include "Esfera.h"

class Scene
{
public:
	Scene();
	Scene(std::string);
	~Scene();
	std::vector<Object*> getObjects();
	std::vector<Light*> getLights();
	
	std::string getNombre();
	void addObject(Object* obj);
	void addLight(Light* lig);
	void setNombre(std::string);
	Object* getObject(int id);
	Light* getLight(int id);

private:
	std::vector<Object*> objects;
	std::vector<Light*> lights;
	std::string nombre;
};