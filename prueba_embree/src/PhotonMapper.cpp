#include "../include/PhotonMapper.h"

PhotonMapper::PhotonMapper()
{

}

PhotonMapper::~PhotonMapper()
{

}

Vec3f PhotonMapper::randomDir(int *seed, Vec3f normal)
{
	double nums[3];
	Vec3f dir = Vec3f(0.f);
	do
	{
		niederreiter2(3, seed, nums);
		dir.x = (nums[0] * 2) + 1;
		dir.y = (nums[1] * 2) + 1;
		dir.z = (nums[2] * 2) + 1;
	} while (distance(dir, Vec3f(0.f)) > 1.0 || dot(dir, normal) <= 0.f);
	return normalize(dir);
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

	std::vector<Photon> photons;
	int seed;
	srand(time(NULL)); //se inicializa seed para en rand posicion de las luces

	for (int i = 0; i < num_photons; i++) //se podria aplicar paralelismo
	{
		//Inicializacion de un foton.
		Photon ph;
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

		//Inicializacion de variables que controlan el loop del trazo de cada foton
		bool discard = false;
		int intersections = 0;

		//Testeo de la interseccion del foton
		while (!discard && intersections <= MAX_INTERSECTIONS)
		{
			if (query->hit.geomID != RTC_INVALID_GEOMETRY_ID)
			{
				ph.point = ph.point + (ph.dir * query->ray.tfar);
				float random = (rand() % 1000) / 1000.f; //aca iria la ruleta rusa
				if (random < 0.5) //refleja
				{
					//se calcula el foton "reflejado" y se modifica el color del foton
					ph.dir = randomDir(&seed, Vec3f(query->hit.Ng_x, query->hit.Ng_y, query->hit.Ng_z));
				}
				else if (random < 0.8) //transparente
				{
					//se calcula el foton "transparente" y se modifica el color del foton
					ph.dir = randomDir(&seed, Vec3f(query->hit.Ng_x, query->hit.Ng_y, query->hit.Ng_z));
				}
				else discard = true;
				query->hit.geomID = RTC_INVALID_GEOMETRY_ID;
				if (!discard)
				{
					//modificar el rayo para hacer la siguiente interseccion
					query->ray.dir_x = ph.dir.x;
					query->ray.dir_y = ph.dir.y;
					query->ray.dir_z = ph.dir.z;
					query->ray.org_x = ph.point.x;
					query->ray.org_y = ph.point.y;
					query->ray.org_z = ph.point.z;
					query->ray.tnear = 0.0001f;
					query->ray.tfar = inf;
					query->hit.primID = RTC_INVALID_GEOMETRY_ID;
					rtcIntersect1(scene_emb, &context, query);
					intersections++; //limitar cantidad de intersecciones
				}
			}
			else discard = true;
			photons.push_back(ph);
		}
	}

	return new PhotonKDTree(photons);
}