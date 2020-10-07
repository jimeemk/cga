#include "../include/Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
	objects.clear();
	lights.clear();
}

Scene::Scene(std::vector<Object*> objs, std::vector<Light*> ls)
{
	objects = objs;
	lights = ls;
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