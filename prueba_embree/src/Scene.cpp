#include "../include/Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
	objects.clear();
	lights.clear();
}

Scene::Scene(std::string n, std::vector<Object*> objs, std::vector<Light*> ls)
{
	objects = objs;
	lights = ls;
	nombre = n;
}

std::vector<Object*> Scene::getObjects()
{
	std::vector<Object*> objs = objects;
	return objs;
}

std::vector<Light*> Scene::getLights()
{
	std::vector<Light*> ls = lights;
	return ls;
}

std::string Scene::getNombre()
{
	return nombre;
}