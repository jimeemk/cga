#include "../include/PhotonKDTree.h"

PhotonKDTree::PhotonKDTree()
{
    kdtree = new KDTree<3, Photon>();
}

PhotonKDTree::PhotonKDTree(vector<Photon> ps)
{
    vector<std::pair<Point<3>, Photon>> points;
    vector<Photon>::iterator it;
    for (it = ps.begin(); it != ps.end(); ++it)
    {
        Point<3> pt = Point<3>();
        Photon p = *it;
        pt[0] = p.point.x; pt[1] = p.point.y; pt[2] = p.point.z;
        std::pair<Point<3>, Photon> par(pt, *it);
        points.push_back(par);
    }
    kdtree = new KDTree<3, Photon>(points);
}

void PhotonKDTree::insert(const Photon& p)
{
    Point<3> pt = Point<3>();
    pt[0] = p.point.x; pt[1] = p.point.y; pt[2] = p.point.z;
    if (!this->kdtree) kdtree = new KDTree<3, Photon>();
    kdtree->insert(pt, p);
}

bool PhotonKDTree::contains(const Photon& p)
{
    Point<3> pt = Point<3>();
    pt[0] = p.point.x; pt[1] = p.point.y; pt[2] = p.point.z;
    return kdtree->contains(pt);
}

bool PhotonKDTree::empty()
{
    return kdtree->empty();
}

int PhotonKDTree::size()
{
    return (int)kdtree->size();
}

Photon PhotonKDTree::kNNValue(const Photon& p, int k)
{
    Point<3> pt = Point<3>();
    pt[0] = p.point.x; pt[1] = p.point.y; pt[2] = p.point.z;
    return kdtree->kNNValue(pt, k);
}

void PhotonKDTree::saveKDTree(const char* ruta)
{
    //futura persistencia del kdtree para re-utilizacion 
}

PhotonKDTree::~PhotonKDTree()
{
    delete kdtree;
}