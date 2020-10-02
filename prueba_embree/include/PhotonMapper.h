#pragma once

#include <random>
#include <vector>
#include <map>
#include <embree3/rtcore.h>

#include "PhotonKDTree.h"
#include "Scene.h"

using namespace embree;

class PhotonMapper
{
public:
	PhotonMapper();
	~PhotonMapper();
	PhotonKDTree* emitPhotons(Scene*, unsigned int);
private:

};