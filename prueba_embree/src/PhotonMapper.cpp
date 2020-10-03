#include "../include/PhotonMapper.h"

PhotonMapper::PhotonMapper()
{

}

PhotonMapper::~PhotonMapper()
{

}

PhotonKDTree* PhotonMapper::emitPhotons(Scene* scene, unsigned int num_photons)
{
	//preparar escena para realizar el photon trace con embree
	RTCDevice device = rtcNewDevice("threads=0");
	RTCScene embree_scene = rtcNewScene(device);

	std::vector<Object*> objs = scene->getObjects();
	std::vector<Object*>::iterator objs_it;
	std::map<unsigned int, Object*> objs_map;

	for (objs_it = objs.begin(); objs_it != objs.end(); ++objs_it)
	{
		Object* obj = *objs_it;
		RTCGeometry* geo = obj->getGeometry();
		rtcCommitGeometry(*geo);
		unsigned int geomID = rtcAttachGeometry(embree_scene, *geo);
		rtcCommitScene(embree_scene);
		rtcReleaseGeometry(*geo);
		objs_map.insert(std::pair<unsigned int, Object*>(geomID, obj));
	}

	for (int i = 0; i < num_photons; i++) //se puede aplicar paralelismo en un futuro
	{
		Photon ph; 
		ph.color = Vec3f(255.f); ph.point = scene->getLight()->getSource(); ph.dir = scene->getLight()->randomDir();
	}


	//emision de fotones en curso
	return NULL;
}