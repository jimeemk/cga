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
private:

};