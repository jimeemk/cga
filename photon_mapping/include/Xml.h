#pragma once

#include "../common/math/vec3.h"
#include "tinyxml.h"
#include "PhotonKDTree.h"
#include "Scene.h"

using namespace embree;

struct config 
{
	Vec3f camara_from;
	Vec3f camara_to;
	unsigned int cant_fotones;
	unsigned int width;
	unsigned int height;
	float fov;
};

//Definicion de funciones
void guardarMapaFotones(const char*, PhotonKDTree*);
PhotonKDTree* cargarMapaFotones(const char*);
void cargarEscena(const char*, Scene*);
config cargarConfiguracion(const char*);