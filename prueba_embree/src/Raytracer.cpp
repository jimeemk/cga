#include "../include/Raytracer.h"

Raytracer::Raytracer() {}

Vec3fa Raytracer::Raytrace(const ISPCCamera& camara, int x, int y, RTCScene escena, RTCIntersectContext& context) {
	Ray rayo = { Vec3fa(camara.xfm.p), Vec3fa(normalize(x * camara.xfm.l.vx + y * camara.xfm.l.vy + camara.xfm.l.vz)), 0.0f, inf };

	/* busca interseccion del rayo contra el objeto mas cercano en la escena */
	rtcIntersect1(escena, &context, RTCRayHit_(rayo));
	return Traza(rayo, 1, escena, context);
}

Vec3fa Raytracer::Traza(Ray rayo, int profundidad, RTCScene escena, RTCIntersectContext& context) {
	if (rayo.geomID != RTC_INVALID_GEOMETRY_ID) {
		return Sombra(escena, context, rayo, profundidad);
	} else {
		/* color de fondo */
		return Vec3fa(0.1f);
	}
}

Vec3fa Raytracer::Sombra(RTCScene escena, RTCIntersectContext& context, Ray rayo, int profundidad) {
	Material mat = { 0.3f, 0.4f, 0.3f, 0, 0, 0, Vec3fa(0.3f, 0, 0.3f) };
	Material mat1 = { 0.3f, 0.4f, 0.3f, 0, 0, 0, Vec3fa(0, 0.3f, 0.3f) };

	//Material mat = { 0.1f, 0.9f, 0.0f, 0.0f, 0, 0, Vec3fa(0.3f, 0, 0.3f) };

	Luz luces[3];
	float luz = 1;
	luces[0] = Luz{ Vec3fa(0,5,0) };
	luces[1] = Luz{ Vec3fa(13,0,0) };
	//luces[2] = Luz{ Vec3fa(10,0,0) };

	Vec3fa color = Vec3fa(0.0f);
	Vec3fa ambiente = Vec3fa(0.0f);
	Vec3fa difuso = Vec3fa(0.0f);
	Vec3fa especular = Vec3fa(0.0f);
	
	ambiente = mat.coefAmbiente * mat.color;
	color = color + ambiente;

	for (int i = 0; i < 2; i++) {
		luz = 0;
		Vec3fa l_dir = luces[i].pos - (rayo.org + rayo.tfar * rayo.dir);

		/* initialize shadow ray */
		Ray shadow(rayo.org + rayo.tfar * rayo.dir, normalize(l_dir), 0.001f, inf);
		shadow.flags = 0;

		/* trace shadow ray */
		//rtcIntersect1(escena, &context, RTCRayHit_(shadow));
		rtcOccluded1(escena, &context, RTCRay_(shadow));

		/* factor de atenuacion */
		float f_att = 1 / ((0.035 * distance(shadow.org, luces[i].pos)) + (0));
		
		if (dot(normalize(rayo.Ng), normalize(l_dir)) > 0) {
			if (shadow.tfar >= 0 ) {
				luz = 1;
				//std::cout << luz << std::endl;
			} else {
				luz = 0;
				/* calcular porcentaje de luz bloqueada por cuerpos con transparencia */
			}	
		} 

		difuso = difuso + luz * f_att * mat.coefDifuso * mat.color * dot(normalize(rayo.Ng), normalize(l_dir));
		
		/* Calculo de termino especular */
		int n = 300;
		Vec3fa R = 2 * dot(normalize(rayo.Ng), normalize(l_dir)) * normalize(rayo.Ng) - normalize(l_dir);
		float RV = dot(normalize(R), -normalize(rayo.dir));
		especular = especular + luz * f_att * mat.coefEspecular * mat.color * pow(RV, n);
		
	}

	/* color final */
	color = color + difuso + especular;

	if (profundidad < profundidad_max) {
		if (mat.coefReflexion > 0) {
			Ray rayo_reflejado(rayo.org + rayo.tfar * rayo.dir, normalize(rayo.dir) - 2 * dot(normalize(rayo.Ng), normalize(rayo.dir)) * normalize(rayo.Ng) , 0.001f, inf);
			rtcIntersect1(escena, &context, RTCRayHit_(rayo_reflejado));
			color = color + mat.coefReflexion * Traza(rayo_reflejado, profundidad + 1, escena, context);
		}

	//	if (transparente) {

	//	}
	}
	return color;
}
