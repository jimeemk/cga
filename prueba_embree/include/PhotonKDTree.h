#pragma once

#include <vector>

#include "../common/math/vec3.h"
#include "../KDTree/KDTree.h"

using namespace embree;

typedef struct Photon { //estructura de un foton
	Vec3f point;
	Vec3f color; //color o "potencia" foton
	Vec3f dir; //direccion del photon
};

class PhotonKDTree
{
private:
    KDTree<3, Photon>* kdtree = NULL;
public:
    PhotonKDTree(/* args */);
    PhotonKDTree(const std::vector<Photon>&); //construir arbol a partir de fotones (balanceado)
    ~PhotonKDTree();

    void insert(const Photon&); //insertar foton
    bool contains(const Photon&); //verificar si ya hay un foton en un punto
    int size(); //cantidad de fotones
    bool empty(); //kdtree vacio
    Photon kNNValue(const Photon&, int);
	void saveKDTree(const char*); //persistencia del kdtree
};
