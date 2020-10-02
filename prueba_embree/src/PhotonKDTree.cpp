#include "../include/PhotonKDTree.h"

PhotonKDTree::PhotonKDTree() 
{
    kdtree = new KDTree<3, Photon>();
}

PhotonKDTree::PhotonKDTree(const vector<Photon>& ps)
{
    vector<pair<Point<3>, Photon>> points;
    vector<Photon>::iterator it;
    for (it = ps.begin(); it != ps.end(); ++it)
    {
        Point<3> pt = Point<3>();
        pt[0] = p.x; pt[1] = p.y; pt[2] = p.z;
        pair<Point<3>, Photon> par = pair<Point<3>, Photon>(pt, *it);
        points.insert(par);
    }
    kdtree = new KDTree<3, Photon>(points);
    
}

void PhotonKDTree::insert(const Photon& p)
{
    Point<3> pt = Point<3>();
    pt[0] = p.x; pt[1] = p.y; pt[2] = p.z;
    if(!this->kdtree) kdtree = new KDTree<3, Photon>();
    kdtree->insert(pt, p);
}

bool PhotonKDTree::contains(const Photon& p)
{
    Point<3> pt = Point<3>();
    pt[0] = p.x; pt[1] = p.y; pt[2] = p.z;
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
    pt[0] = p.x; pt[1] = p.y; pt[2] = p.z;
    return kdtree->kNNValue(pt, k);
}

PhotonKDTree::~PhotonKDTree()
{
    delete kdtree;
}