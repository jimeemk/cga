#pragma once

#include <vector>
#include "../common/math/vec3.h"
#include "../KDTree/KDTree.h"

using namespace embree;

struct Photon { //estructura de un foton
        Vec3f point;
        Vec3f color; //color o "potencia" foton
        Vec3f dir; //direccion del photon
        bool operator==(const Photon& rhs) const noexcept
        {
            // logic here
            return rhs.point==point; // for example
        }
        //Jairo, si un dia ocurre que estas muy enojado con tu kdtree porque da errores raros, no inserta bien las cosas, te toma como repetido cosas
        //Que no estan repetidas, o algo asi, puede ser que internamente use hash y nuestro hash sea cualquier cosa. 
        //Saludos,
        //Jime
};

namespace std {
    template<>
    struct hash<Photon> {
        inline size_t operator()(const Photon& x) const {
            return x.point.x;
        }
    };
}

class PhotonKDTree
{
private:
    KDTree<3, Photon>* kdtree = NULL;
public:
    PhotonKDTree(/* args */);
    PhotonKDTree(std::vector<Photon>); //construir arbol a partir de fotones (balanceado)
    ~PhotonKDTree();


    void insert(const Photon&); //insertar foton
    bool contains(const Photon&); //verificar si ya hay un foton en un punto
    int size(); //cantidad de fotones
    bool empty(); //kdtree vacio
    std::vector<Photon> kNNValue(const Vec3f&, int);
	void saveKDTree(const char*); //persistencia del kdtree
};
