#include "../include/Raytracer.h"

Raytracer::Raytracer() {
	rt_especular = Vec3fa(0.0f);
	rt_indirecta = Vec3fa(0.0f);
	rt_caustica = Vec3fa(0.0f);
}

Vec3fa Raytracer::raytrace(const ISPCCamera& camara, int x, int y, RTCScene escena, RTCIntersectContext& context) {
	
	Ray rayo = { Vec3fa(camara.xfm.p), Vec3fa(normalize(x * camara.xfm.l.vx + y * camara.xfm.l.vy + camara.xfm.l.vz)), 0.0f, inf };

	/* busca interseccion del rayo contra el objeto mas cercano en la escena */
	rtcIntersect1(escena, &context, RTCRayHit_(rayo));
	return traza(rayo, 1, escena, context);
}

Vec3fa Raytracer::traza(Ray rayo, int profundidad, RTCScene escena, RTCIntersectContext& context) {
	if (rayo.geomID != RTC_INVALID_GEOMETRY_ID) {
		Vec3fa interseccion_rayo = rayo.org + rayo.tfar * rayo.dir;
		if (interseccion_rayo.y>19.6 && interseccion_rayo.x > -5 && interseccion_rayo.x < 5 && interseccion_rayo.z > -5 && interseccion_rayo.z < 5)
		{
			return Vec3fa(1.f);
		}
		else
			return sombra(escena, context, rayo, profundidad, rayo.geomID);
	} else {
		/* color de fondo */
		return Vec3fa(0.1f);
	}
}

Vec3fa Raytracer::sombra(RTCScene escena, RTCIntersectContext& context, Ray rayo, int profundidad, int geomID) {
	
	Material mat = Settings::getInstance()->getObject(geomID)->getMaterial();
	Vec3fa interseccion_rayo = rayo.org + rayo.tfar * rayo.dir;
	
	std::vector<Light*> luces = Settings::getInstance()->getLights();
	float luz = luces.size();

	Vec3fa color = Vec3fa(0.0f);
	Vec3fa indirecta = Vec3fa(0.0f);
	Vec3fa causticas = Vec3fa(0.0f);
	Vec3fa difuso = Vec3fa(0.0f);
	Vec3fa especular = Vec3fa(0.0f);

	PhotonKDTree* kdtree = Settings::getInstance()->getKdTree();
	PhotonKDTree* caustree = Settings::getInstance()->getCausTree();
	
	/* obtengo los n fotones mas cercanos al punto de interseccion */
	int cant_photons = 100;
	std::vector<Photon> nearest_photons = kdtree->kNNValue(interseccion_rayo, cant_photons);
	float radius = getRadius(nearest_photons, interseccion_rayo);
	
	std::vector<Photon> nearest_photonsCaus = caustree->kNNValue(interseccion_rayo, cant_photons);
	float radiusCaus = getRadius(nearest_photonsCaus, interseccion_rayo);

	for (int i = 0; i < luces.size(); i++) {
		luz = 0;
		Vec3fa l_pos = luces.at(i)->getSource();
		Vec3fa l_dir = normalize(l_pos-interseccion_rayo);// luces.at(i)->lightDir(interseccion_rayo);

		/* initialize shadow ray */
		Ray shadow(interseccion_rayo, normalize(l_dir), 0.001f, inf);

		/* trace shadow ray */
		rtcIntersect1(escena, &context, RTCRayHit_(shadow));

		/* factor de atenuacion */
		float f_att = 1 / (0.020*(distance(shadow.org, l_pos)));

		SquareLight* squareLight = dynamic_cast<SquareLight*>(luces[i]);

		if (dot(normalize(rayo.Ng), normalize(l_dir)) > 0) {
			if (shadow.tfar == (float)inf) {
				luz = 1;
			}
			else {
				/* calcular porcentaje de luz bloqueada por cuerpos con transparencia */
				//luz = procesarOclusion(interseccion_rayo, l_dir, l_pos, escena, context);
				luz = softShadow(interseccion_rayo, escena, context);
			}
		}

		if (squareLight && dot(normalize(squareLight->getNormal()), normalize(l_dir)) > 0) {
			luz = 0;
		}

		
		difuso = difuso + (luz * f_att *  mat.coef_difuso * mat.color * dot(normalize(rayo.Ng), normalize(l_dir)))/luces.size(); 
		/* Calculo de termino especular */
		int n = 500;
		Vec3fa R = 2 * dot(normalize(rayo.Ng), normalize(l_dir)) * normalize(rayo.Ng) - normalize(l_dir);
		float RV = dot(normalize(R), -normalize(rayo.dir));
		especular = especular + (luz * f_att * mat.coef_especular * Vec3fa(1) * pow(RV, n))/luces.size(); // Color blanco o color de la luz
	}
	indirecta = estimacion_radiancia(nearest_photons, interseccion_rayo, rayo.dir, cant_photons, mat.coef_difuso / std::_Pi, radius);
	causticas = estimacion_radiancia(nearest_photonsCaus, interseccion_rayo, rayo.dir, cant_photons, mat.coef_difuso / std::_Pi, radiusCaus);
	/* color final */
	color = color  + mat.coef_difuso*difuso + mat.coef_especular*especular + (1-mat.coef_difuso-mat.coef_especular)*(indirecta+causticas);

	if (profundidad < profundidad_max) {
		if (mat.coef_reflexion > 0) {
			Ray rayo_reflejado(interseccion_rayo, normalize(rayo.dir) - 2 * dot(normalize(rayo.Ng), normalize(rayo.dir)) * normalize(rayo.Ng), 0.001f, inf);
			rtcIntersect1(escena, &context, RTCRayHit_(rayo_reflejado));
			especular = especular + mat.coef_reflexion * traza(rayo_reflejado, profundidad + 1, escena, context);
			color = color + especular;
		}


		if (mat.coef_transparencia > 0) {
			Vec3fa dir_refract = refract(rayo.dir, rayo.Ng, mat.indice_refraccion);
			if (dir_refract != Vec3fa(0)) {
				Ray rayo_refractado = { interseccion_rayo, dir_refract, 0.001f, inf };
				rtcIntersect1(escena, &context, RTCRayHit_(rayo_refractado));
				color = color + mat.coef_transparencia * traza(rayo_refractado, profundidad + 1, escena, context);
			}
		}
	}
	
	rt_especular = especular;
	rt_indirecta = indirecta;
	rt_caustica = causticas;
	rt_directo = difuso;


	return color;
}

float Raytracer::procesarOclusion(Vec3fa origen, Vec3fa direccion_luz, Vec3fa pos_luz, RTCScene escena, RTCIntersectContext context) {
	float res = 1;
	Ray rayo_oclusion = { origen, direccion_luz, 0.001f, inf };
	bool hit = true;
	int last_geomID = -1;
	Vec3fa nuevo_origen;
	while (hit && res != 0) {
		rtcIntersect1(escena, &context, RTCRayHit_(rayo_oclusion));
		hit = rayo_oclusion.tfar != (float) inf;
		if (hit && rayo_oclusion.geomID != last_geomID && distance(rayo_oclusion.org, rayo_oclusion.org+direccion_luz * rayo_oclusion.tfar) < distance(rayo_oclusion.org, pos_luz)) { //agrego lo del tfar porque sino siempre da con el techo, es a modo temporal
			last_geomID = rayo_oclusion.geomID;
			res *= Settings::getInstance()->getObject(last_geomID)->getMaterial().coef_transparencia;
		}
		nuevo_origen = rayo_oclusion.org + rayo_oclusion.tfar * rayo_oclusion.dir;
		rayo_oclusion = { nuevo_origen, direccion_luz, 0.001f, inf };
	}

	return res;
}

Vec3f Raytracer::refract(Vec3fa I, Vec3fa N, float indice_refraccion) {
	Vec3fa normal = N;
	float cos_i = dot(normalize(normal), normalize(I));
	float n_actual = 1, n_material = indice_refraccion;
	if (cos_i < 0) {
		cos_i = -cos_i;
	} else {
		normal = -N;
		std::swap(n_actual, n_material);
	}
	float eta = n_actual / n_material;
	float k = 1 - eta * eta * (1 - cos_i * cos_i);

	if (k < 0) {
		return Vec3fa(0);
	} else {
		return eta * I + (eta * cos_i - sqrtf(k)) * normal;
	}
}

Vec3fa Raytracer::estimacion_radiancia(std::vector<Photon> nearest_photons, Vec3fa pos, Vec3fa dir, int p, float lambert_brdf, float radius) {
	float pi = std::_Pi;
	Vec3fa result = Vec3fa(0.0f);

	for (int i = 0; i < p; i++) {
		result = result + lambert_brdf * nearest_photons[i].color;
		//std::cout << nearest_photons[i].color << std::endl;
	}
	//std::cout << result << std::endl;
	return result * (1 / (pi * radius * radius));
}

float Raytracer::getRadius(std::vector<Photon> photons, Vec3f pos) {
	float max = 0;
	for (int i = 0; i < photons.size(); i++) {
		if (distance(photons[i].point, pos) > max) {
			max = distance(photons[i].point, pos);
		}
	}
	//std::cout << max << std::endl;
	return max;
}

float Raytracer::softShadow(Vec3fa origen, RTCScene escena, RTCIntersectContext context) {
	int num_samples = 256;
	std::vector<Light*> luces = Settings::getInstance()->getLights();
	float res = 0;
	for (int i = 0; i < luces.size(); i++) {
		SquareLight* squareLight = dynamic_cast<SquareLight*>(luces[i]);
		if (squareLight) {
			std::vector<Vec3f> positions = squareLight->samplePositions();
			for (int j = 0; j < num_samples ; j++) {
				res += procesarOclusion(origen, normalize(positions[j] - origen) ,positions[j], escena, context);
			}
			res = res / num_samples;
		}
		else
			res += procesarOclusion(origen, normalize(luces.at(i)->getSource() - origen), luces.at(i)->getSource(), escena, context);
	}
	res = res / luces.size();
	return res;
}
