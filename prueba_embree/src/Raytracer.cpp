#include "../include/Raytracer.h"

Raytracer::Raytracer() {}

Vec3fa Raytracer::raytrace(const ISPCCamera& camara, int x, int y, RTCScene escena, RTCIntersectContext& context) {
	Ray rayo = { Vec3fa(camara.xfm.p), Vec3fa(normalize(x * camara.xfm.l.vx + y * camara.xfm.l.vy + camara.xfm.l.vz)), 0.0f, inf };

	/* busca interseccion del rayo contra el objeto mas cercano en la escena */
	rtcIntersect1(escena, &context, RTCRayHit_(rayo));
	return traza(rayo, 1, escena, context);
}

Vec3fa Raytracer::traza(Ray rayo, int profundidad, RTCScene escena, RTCIntersectContext& context) {
	if (rayo.geomID != RTC_INVALID_GEOMETRY_ID) {
		return sombra(escena, context, rayo, profundidad, rayo.geomID);
	} else {
		/* color de fondo */
		return Vec3fa(0.1f);
	}
}

Vec3fa Raytracer::sombra(RTCScene escena, RTCIntersectContext& context, Ray rayo, int profundidad, int geomID) {
	//Material mat = getMaterial(rayo.geomID);
	Material mat = Settings::getInstance()->getObject(geomID)->getMaterial();
	Vec3fa interseccion_rayo = rayo.org + rayo.tfar * rayo.dir;

	//Luz luces[3];
	
	std::vector<Light*> luces = Settings::getInstance()->getLights();
	float luz = luces.size();
	//luces[0] = Luz{ Vec3fa(10,10,10) };
	//luces[1] = Luz{ Vec3fa(13,0,0) };
	//luces[2] = Luz{ Vec3fa(10,0,0) };

	Vec3fa color = Vec3fa(0.0f);
	Vec3fa ambiente = Vec3fa(0.0f);
	Vec3fa difuso = Vec3fa(0.0f);
	Vec3fa especular = Vec3fa(0.0f);

	ambiente = mat.coef_ambiente * mat.color;
	color = color + ambiente;

	for (int i = 0; i < 1; i++) {
		luz = 0;
		Vec3fa posLuc = Vec3fa(luces.at(i)->getSource().x, luces.at(i)->getSource().y, luces.at(i)->getSource().z);
		Vec3fa l_dir = posLuc - interseccion_rayo;

		/* initialize shadow ray */
		Ray shadow(interseccion_rayo, normalize(l_dir), 0.001f, inf);
		shadow.flags = 0;

		/* trace shadow ray */
		//rtcIntersect1(escena, &context, RTCRayHit_(shadow));
		rtcOccluded1(escena, &context, RTCRay_(shadow));

		/* factor de atenuacion */
		
		float f_att = 1 / ((0.025 * distance(shadow.org, posLuc)) + (0));

		if (dot(normalize(rayo.Ng), normalize(l_dir)) > 0) {
			if (shadow.tfar >= 0) {
				luz = 1;
			} else {
				/* calcular porcentaje de luz bloqueada por cuerpos con transparencia */
				luz = procesarOclusion(interseccion_rayo, l_dir, escena, context);
			}
		}

		difuso = difuso + luz * f_att * mat.coef_difuso * mat.color * dot(normalize(rayo.Ng), normalize(l_dir));

		/* Calculo de termino especular */
		int n = 300;
		Vec3fa R = 2 * dot(normalize(rayo.Ng), normalize(l_dir)) * normalize(rayo.Ng) - normalize(l_dir);
		float RV = dot(normalize(R), -normalize(rayo.dir));
		especular = especular + luz * f_att * mat.coef_especular * mat.color * pow(RV, n);
	}

	/* color final */
	color = color + difuso + especular;

	if (profundidad < profundidad_max) {
		if (mat.coef_reflexion > 0) {
			Ray rayo_reflejado(interseccion_rayo, normalize(rayo.dir) - 2 * dot(normalize(rayo.Ng), normalize(rayo.dir)) * normalize(rayo.Ng), 0.001f, inf);
			rtcIntersect1(escena, &context, RTCRayHit_(rayo_reflejado));
			color = color + mat.coef_reflexion * traza(rayo_reflejado, profundidad + 1, escena, context);
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
	return color;
}


float Raytracer::procesarOclusion(Vec3fa origen, Vec3fa direccion_luz, RTCScene escena, RTCIntersectContext context) {
	float res = 1;
	Ray rayo_oclusion = { origen, direccion_luz, 0.001f, inf };
	bool hit = true;
	int last_geomID = -1;
	Vec3fa nuevo_origen;
	while (hit) {
		rtcIntersect1(escena, &context, RTCRayHit_(rayo_oclusion));
		hit = rayo_oclusion.tfar != (float) inf;
		if (hit && rayo_oclusion.geomID != last_geomID) {
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
		// we are outside the surface, we want cos(theta) to be positive
		cos_i = -cos_i;
	} else {
		// we are inside the surface, cos(theta) is already positive but reverse normal direction
		normal = -N;
		// swap the refraction indices
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