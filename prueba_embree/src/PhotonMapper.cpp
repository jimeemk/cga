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

	//agregar geometrias a la escena de embree y asociarlas con el ID
	for (int i = 0; i < objs.size(); i++)
	{
		Object* obj = objs.at(i);
		RTCGeometry geo = obj->getGeometry();
		std::cout << geo;
		rtcCommitGeometry(geo);
		rtcAttachGeometryByID(embree_scene, geo, i);
		rtcCommitScene(embree_scene);
	}

	std::vector<Photon> photons;
	int seed;

	for (int i = 0; i < num_photons; i++)
	{
		//Inicializacion de un foton.
		Photon ph;
		ph.color = Vec3f(255.f); 
		ph.point = scene->getLight()->getSource();
		ph.dir = scene->getLight()->randomDir(&seed);

		//Context y RayHit para embree
		RTCIntersectContext context;
		rtcInitIntersectContext(&context);

		RTCRayHit* query = new RTCRayHit();
		query->ray.dir_x = ph.dir.x;
		query->ray.dir_y = ph.dir.y;
		query->ray.dir_z = ph.dir.z;
		query->ray.org_x = ph.point.x;
		query->ray.org_y = ph.point.y;
		query->ray.org_z = ph.point.z;
		query->ray.tnear = 0.0001f;
		query->ray.tfar = inf;
		query->hit.geomID = RTC_INVALID_GEOMETRY_ID;
		query->hit.primID = RTC_INVALID_GEOMETRY_ID;

		//Interseccion con la escena
		rtcIntersect1(embree_scene, &context, query);

		if (query->hit.geomID != RTC_INVALID_GEOMETRY_ID)
		{
			std::cout << "choca con algo";
			ph.point = ph.point + (ph.dir * query->ray.tfar);
		}
		photons.push_back(ph);
	}

	return new PhotonKDTree(photons);
}