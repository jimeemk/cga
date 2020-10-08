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
	RTCScene scene_emb = rtcNewScene(device);

	Object* obj = new Object();
	Object* obj2 = new Object();
	unsigned int objetoID = obj->agregarObjeto(device, scene_emb, "Modelos/12221_Cat_v1_l3");
	unsigned int objetoID2 = obj2->agregarObjeto(device, scene_emb, "Modelos/face");

	//agregar geometrias a la escena de embree y asociarlas con el ID

	std::vector<Photon> photons;
	int seed;
	srand(time(NULL)); //se inicializa seed para en rand de las luces

	for (int i = 0; i < num_photons; i++)
	{
		//Inicializacion de un foton.
		Photon ph;
		ph.color = Vec3f(255.f); 
		ph.point = scene->getLights().at(0)->getSource();
		ph.dir = scene->getLights().at(0)->randomDir(&seed);

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
		rtcIntersect1(scene_emb, &context, query);

		if (query->hit.geomID != RTC_INVALID_GEOMETRY_ID)
		{
			ph.point = ph.point + (ph.dir * query->ray.tfar);
		}
		photons.push_back(ph);
	}

	return new PhotonKDTree(photons);
}