#pragma once

#include "../KDTree/KDTree.h"

using namespace std;

typedef struct Photon { //estructura de un foton
    double x, y, z;
    string nombre; 
};

class PhotonKDTree
{
private:
    KDTree<3, Photon>* kdtree = NULL;
public:
    PhotonKDTree(/* args */);
    PhotonKDTree(const vector<Photon>&); //construir arbol a partir de fotones
    ~PhotonKDTree();

    void insert(const Photon&); //insertar foton
    bool contains(const Photon&); //verificar si ya hay un foton en un punto
    int size(); //cantidad de fotones
    bool empty(); //kdtree vacio
    Photon kNNValue(const Photon&, int);
};
