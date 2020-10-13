#include "../include/Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
	objects.clear();
	lights.clear();
}

Scene::Scene(std::string n)
{
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
void Scene::addObject(Object* obj)
{
	objects.push_back(obj);
}
void Scene::addLight(Light* lig)
{
	lights.push_back(lig);
}
Object* Scene::getObject(int id)
{
	return objects.at(id);
}