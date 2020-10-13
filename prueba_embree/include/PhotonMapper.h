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

const int MAX_INTERSECTIONS = 5;

using namespace embree;



class PhotonMapper
{
public:
	PhotonMapper();
	~PhotonMapper();
	PhotonKDTree* emitPhotons(Scene*, unsigned int);
	Vec3f randomDir(int*, Vec3f);
	float getMax(float a, float b, float c);
private:

};