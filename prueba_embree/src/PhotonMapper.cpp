#include "../include/PhotonMapper.h"
#include <fstream>
#include "../common/algorithms/parallel_for.h"

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
float sustituir(float numero)
{
	if (numero<0.00001 && numero> -0.00001)
		return 0;
	else if (numero < 1.00001 && numero > 0.99999)
		return 1;
	else if (numero< -0.99999 && numero > -1.00001)
		return -1;
	else
		return numero;
}

Vec3f reglaCoseno(int* seed, Vec3f normal)
{
	normal = normalize(normal);
	normal = Vec3fa(sustituir(normal.x), sustituir(normal.y), sustituir(normal.z));
	double nums[2];
	niederreiter2(2, seed, nums);
	float rho = sustituir(std::sqrt(nums[0]*-1));
	float theta = sustituir(nums[1] * -2 * (float)embree::pi);
	float x = sustituir(rho * std::cos(theta));
	float y = sustituir(rho * std::sin(theta));
	float z = 0;
	if((1 - x * x - y * y)>=0)
		z = sustituir(std::sqrt(1 - x*x - y*y));
	if (normal == Vec3fa(1, 0, 0))
		return normalize(Vec3fa(z,x,y));
	else if (normal == Vec3fa(-1, 0, 0))
		return normalize(Vec3fa(-z,x,y));
	else if (normal == Vec3fa(0, 1, 0))
		return normalize(Vec3fa(x,z,y));
	else if (normal == Vec3fa(0, -1, 0))
		return normalize(Vec3fa(x,-z,y));
	else if (normal == Vec3fa(0, 0, 1))
		return normalize(Vec3fa(x,y,z));
	else if (normal == Vec3fa(0, 0, -1))
		return normalize(Vec3fa(x,-y,-z));
	else return normalize(Vec3fa(x, y, z));
}

PhotonKDTree* PhotonMapper::emitPhotons(Scene* scene, unsigned int num_photons)
{

	std::vector<Photon> photons;
	int seed;
	srand(time(NULL)); //se inicializa seed para en rand posicion de las luces
	int cantFotones = 0;
	for (int j = 0; j < scene->getLights().size(); j++)
	{
		for (int inc=0; inc<num_photons; inc++)
		{
			bool guardo = false;
			while (!guardo)
			{
				//Inicializacion de un foton.
				Photon ph;
				ph.point = scene->getLights().at(j)->getSource();
				ph.dir = scene->getLights().at(j)->randomDir(&seed);
				ph.color = Vec3fa(scene->getLights().at(j)->getPower() / num_photons);
				cout << ph.dir.x << '/' << ph.dir.y << '/' << ph.dir.z << '\n';
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
					if (query->hit.geomID != RTC_INVALID_GEOMETRY_ID)
					{
						Object* obj = Settings::getInstance()->getObject(query->hit.geomID);
						Material m = obj->getMaterial();
						Vec3fa coef_difuso = m.coef_difuso * m.color;
						Vec3fa coef_especular = m.coef_especular * m.color;
						float d = getMax(coef_difuso.x * m.color.x, coef_difuso.y * m.color.y, coef_difuso.z * m.color.z) / getMax(m.color.x, m.color.y, m.color.z);
						float s = d + getMax(coef_especular.x * m.color.x, coef_especular.y * m.color.y, coef_especular.z * m.color.z) / getMax(m.color.x, m.color.y, m.color.z);
						float t = s + obj->getMaterial().coef_transparencia;

						ph.point = ph.point + (ph.dir * query->ray.tfar);
						cout << ph.point.x << '/' << ph.point.y << '/' << ph.point.z << '\n';
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
							N = normalize(N);
							if (dot(N, ph.dir) > 0) N = N * (-1);
							Vec3fa dir2 = ph.dir - (dot(N, ph.dir) * 2) * N;

							ph.color.x = ph.color.x * coef_especular.x / s;
							ph.color.y = ph.color.y * coef_especular.y / s;
							ph.color.z = ph.color.z * coef_especular.z / s;
							ph.dir = normalize(dir2);
						}
						else if (random < t) //transparente
						{
							//Atenuo el color segun cuanta luz deja pasar el material

							ph.color = ph.color * m.coef_transparencia;

							bool adentro = false;
							Vec3fa N = { query->hit.Ng_x, query->hit.Ng_y, query->hit.Ng_z };
							N = normalize(N);
							if (dot(N, ph.dir) > 0) { N = N * (-1); adentro = true; }

							float ior = m.indice_refraccion;
							float eta;
							if (adentro)
								eta = ior;
							else
								eta = 1 / ior;// Ese uno es medio cualquiera. Ponele que es el del aire.. ponele. 
							float cosi = dot(N * (-1), ph.dir);
							float k = 1 - eta * eta * (1 - cosi * cosi);
							Vec3fa T = (ph.dir * eta) + (N * (eta * cosi - embree::sqrt(k)));
							T = normalize(T);

							ph.dir = T;
						}
						else
						{
							discard = true;
							if (intersections >= 1)
							{
								cantFotones++;
								photons.push_back(ph);
								guardo = true;
							}
							else
								guardo = false;
						}
						query->hit.geomID = RTC_INVALID_GEOMETRY_ID;
						if (!discard)
						{
							intersections++;

							cout << ph.dir.x << '/' << ph.dir.y << '/' << ph.dir.z << '\n';
							cout << ph.point.x << '/' << ph.point.y << '/' << ph.point.z << '\n';

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
						}
					}
					else { 
						discard = true; 
						guardo = false;
					}
				}
			}
		}
	}

	ofstream file;
	file.open("puntos.txt");
	for (int i = 0; i < photons.size(); i++)
	{
		Vec3fa p = photons.at(i).point;
		Vec3fa c = photons.at(i).color*num_photons;
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