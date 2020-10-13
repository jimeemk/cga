#pragma once

#include <random>
#include <vector>
#include <map>
#include <embree3/rtcore.h>

#include "PhotonKDTree.h"
#include "Scene.h"

const int MAX_INTERSECTIONS = 5;

using namespace embree;

#include <iostream>

class PhotonMapper
{
public:
	PhotonMapper();
	~PhotonMapper();
	PhotonKDTree* emitPhotons(Scene*, unsigned int);
	Vec3f randomDir(int*, Vec3f);
private:

};