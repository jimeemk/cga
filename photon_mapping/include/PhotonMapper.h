#pragma once

#include <random>
#include <vector>
#include <map>
#include <embree3/rtcore.h>

#include "PhotonKDTree.h"
#include "Scene.h"
#include "Settings.h"
#include <iostream>
#include <cmath>
#include <iostream>
#include <math.h>
#include <tbb/tbb.h>

const int MAX_INTERSECTIONS = 100;

using namespace embree;
using namespace std;
using namespace tbb;

class PhotonMapper
{
public:
	PhotonMapper();
	~PhotonMapper();
	PhotonKDTree* emitPhotons(Scene*, unsigned int);
	Vec3f randomDir(int*, Vec3f);
	float getMax(float a, float b, float c);
	Vec3fa pasarAEsfericas(Vec3fa a, Vec3fa centro);
	PhotonKDTree* fotonesCausticas(Scene* scene, unsigned int num_photons);
	void obtenerMinYMaxUV(RTCBounds bounds, Vec3fa center,float& umin, float& umax, float& vmin, float& vmax);
	Vec3fa direccionCausticas(int* seed, Vec3fa centro, float umin, float umax, float vmin, float vmax);
	Vec3fa pasarACartesianas(Vec3fa a, Vec3fa centro);
private:

};