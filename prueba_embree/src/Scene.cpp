#include "../include/Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
	objects.clear();
}

Scene::Scene(std::vector<Object*> objs, Light* l)
{
	objects = std::vector<Object*>();
	std::vector<Object*>::iterator it;
	if (objs != NULL) for (it = objs.begin(); it != objs.end(); it++) objects.push_back(*it);
	light = l;
}

std::vector<Object*> Scene::getObjects()
{
	std::vector<Object*> objs;
	std::vector<Object*>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++) objs.push_back(*it);
	return objs;
}

Light* Scene::getLight()
{
	return light;
}

void Scene::addObject(Object* obj)
{
	objects.push_back(obj);
}

void Scene::addLight(Light* l)
{
	//por ahora nada
}