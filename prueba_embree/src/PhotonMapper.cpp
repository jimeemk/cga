#include "../include/PhotonMapper.h"
#include <fstream>

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
	//RTCDevice device = rtcNewDevice("threads=0");
	//RTCScene scene_emb = rtcNewScene(device);
	//Material m1={0.2,0.6,0.2,0.5,0.2,0.3,Vec3fa(1.0,1.0,0)};//kd,ka,ks,kr,kt,kR,color
	//Object* obj = new Object("Modelos/12221_Cat_v1_l3",m1);
	//Object* obj2 = new Object("Modelos/face",m1);
	//unsigned int objetoID = obj->agregarObjeto(device, scene_emb);
	//unsigned int objetoID2 = obj2->agregarObjeto(device, scene_emb);

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
		rtcIntersect1(Settings::getInstance()->getEscena(), &context, query);

		//Inicializacion de variables que controlan el loop del trazo de cada foton
		bool discard = false;
		int intersections = 0;

		//Testeo de la interseccion del foton
		while (!discard && intersections <= MAX_INTERSECTIONS)
		{
			photons.push_back(ph);
			if (query->hit.geomID != RTC_INVALID_GEOMETRY_ID)
			{
				Object* obj = Settings::getInstance()->getObject(query->hit.geomID);
				Material m = obj->getMaterial();
				Vec3fa coef_difuso = m.coef_difuso * m.color;
				Vec3fa coef_especular = m.coef_especular * m.color;
				float d = getMax(coef_difuso.x*m.color.x, coef_difuso.y * m.color.y, coef_difuso.z * m.color.z) / getMax(m.color.x,m.color.y,m.color.z);
				float s = d + getMax(coef_especular.x * m.color.x, coef_especular.y * m.color.y, coef_especular.z * m.color.z) / getMax(m.color.x, m.color.y, m.color.z);
				float t = d + s + obj->getMaterial().coef_transparencia;

				ph.point = ph.point + (ph.dir * query->ray.tfar);
				
				float random = (rand() % 1000) / 1000.f; //aca iria la ruleta rusa
				
				if (random < d) //refleja difuso
				{
					//Aca falta la magia que quiere hacer el edu del coseno y la semiesfera. Creo que se que es lo que quiere, pero son 4am bye bye
					//se calcula el foton "reflejado" y se modifica el color del foton
					ph.color.x = ph.color.x * coef_difuso.x / d;
					ph.color.y = ph.color.y * coef_difuso.y / d;
					ph.color.z = ph.color.z * coef_difuso.z / d;
					ph.dir = randomDir(&seed, Vec3f(query->hit.Ng_x, query->hit.Ng_y, query->hit.Ng_z));
				}
				else if (random < s) //refleja especular
				{
					//se calcula el foton "reflejado" y se modifica el color del foton
					//bool adentro = false;
					//if (fun->productoEscalar(N, dir) > 0) N = fun->multiplicarK(N, -1), adentro = true;
					//punto R = fun->resta(dir, fun->multiplicarK(N, fun->productoEscalar(N, dir) * 2));
					Vec3fa N = { query->hit.Ng_x, query->hit.Ng_y, query->hit.Ng_z };
					if (dot(N, ph.dir) > 0) N = N * (-1);
					Vec3fa dir2 = ph.dir - (dot(N, ph.dir) * 2) * N;

					ph.color.x = ph.color.x * coef_especular.x / s;
					ph.color.y = ph.color.y * coef_especular.y / s;
					ph.color.z = ph.color.z * coef_especular.z / s;
					ph.dir = dir2;
				}
				else if (random < t) //transparente
				{

					bool adentro = false;
					Vec3fa N = { query->hit.Ng_x, query->hit.Ng_y, query->hit.Ng_z };
					if (dot(N, ph.dir) > 0) { N = N * (-1); adentro = true; }
					//Si el producto vectorial da positivo, no deberia ser adentro=false??? o sea, viene de afuera. Esto lo copie de la tarea pasada je. Como mucho es cambiar el > por <

					float ior = m.indice_refraccion;
					float eta;
					if (adentro)
						eta = ior;
					else
						eta = 1 / ior;// Ese uno es medio cualquiera. Ponele que es el del aire.. ponele. 
					float cosi = dot(N*(-1), ph.dir);
					float k = 1 - eta * eta * (1 - cosi * cosi);
					Vec3fa T = (ph.dir* eta) + (N* (eta * cosi - embree::sqrt(k)));
					T = normalize(T);

					ph.dir = T;
				}
				else discard = true;
				query->hit.geomID = RTC_INVALID_GEOMETRY_ID;
				if (!discard)
				{
					query->ray.dir_x = ph.dir.x;
					query->ray.dir_y = ph.dir.y;
					query->ray.dir_z = ph.dir.z;
					query->ray.org_x = ph.point.x;
					query->ray.org_y = ph.point.y;
					query->ray.org_z = ph.point.z;
					query->ray.tnear = 0.0001f;
					query->ray.tfar = inf;
					query->hit.primID = RTC_INVALID_GEOMETRY_ID;
					rtcIntersect1(Settings::getInstance()->getEscena(), &context, query);
					intersections++; //limitar cantidad de intersecciones
				}
			}
			else discard = true;
		}
		
	}

	ofstream file;
	file.open("puntos.txt");
	for (int i = 0; i < photons.size(); i++)
	{
		Vec3fa p = photons.at(i).point;
		file << p.x << "/" << p.y << "/" << p.z << "/\n";
	}
	file.close();

	return new PhotonKDTree(photons);
}

float PhotonMapper::getMax(float a, float b, float c)
{
	if (a > b)
	{
		if (a > c)
			return a;
		else
			return c;
	}
	else {
		if (b > c)
			return b;
		else
			return c;
	}
}