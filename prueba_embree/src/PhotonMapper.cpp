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

Vec3f reglaCoseno(int* seed, Vec3f normal)
{
	double nums[2];
	niederreiter2(2, seed, nums);
	float rho = embree::sqrt(nums[0]);
	float theta = nums[1] * 2 * (float)embree::pi;
	float x = theta * embree::cos(rho);
	float y = theta * embree::sin(rho);
	float z = embree::sqrt(1 - x*x - y*y);

	//Eso es para una normal en la direccion de z, asi que voy a rotarlo para que la normal coincida
	float titay = embree::atan(-1*(normal.x/normal.z)); // Angulo que rota en y para que x de la normal=0
	float titax = embree::atan(normal.y / normal.z);//Angulo que rota en x para que y de la normal =0
	float xroty = embree::cos(titay) * x + embree::sin(titay) * z;
	float zroty = embree::sin(titay) * x * (-1) + embree::cos(titay) * z;
	float yrotx = embree::sin(titax) * zroty * (-1) + embree::cos(titax) * y;
	float zrotx = embree::sin(titax) * y + embree::cos(titax) * zroty;

	return normalize(Vec3fa(xroty,yrotx,zrotx));

}

PhotonKDTree* PhotonMapper::emitPhotons(Scene* scene, unsigned int num_photons)
{

	std::vector<Photon> photons;
	int seed;
	srand(time(NULL)); //se inicializa seed para en rand posicion de las luces

	for (int i = 0; i < num_photons; i++) //se podria aplicar paralelismo
	{
		//Inicializacion de un foton.
		Photon ph;
		ph.point = scene->getLights().at(0)->getSource();
		ph.dir = scene->getLights().at(0)->randomDir(&seed);
		ph.color = Vec3fa(scene->getLights().at(0)->getPower()/num_photons);

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
					//se calcula el foton "reflejado" y se modifica el color del foton
					ph.color.x = ph.color.x * coef_difuso.x / d;
					ph.color.y = ph.color.y * coef_difuso.y / d;
					ph.color.z = ph.color.z * coef_difuso.z / d;
					ph.dir = reglaCoseno(&seed, Vec3f(query->hit.Ng_x, query->hit.Ng_y, query->hit.Ng_z));
				}
				else if (random < s) //refleja especular
				{
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
					//Atenuo el color segun cuanta luz deja pasar el material
					
					ph.color = ph.color * m.coef_transparencia;

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
		Vec3fa c = photons.at(i).color;
		file << p.x << "/" << p.y << "/" << p.z <<"/"<< c.x << "/" << c.y << "/" << c.z << "/\n";
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