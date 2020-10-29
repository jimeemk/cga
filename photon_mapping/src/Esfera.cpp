#include "../include/Esfera.h"

Esfera::Esfera()
{
}

Esfera::Esfera(Material m, Vec3fa c, float r) : Object(m, c)
{
	radio = r;
}

//Funciones usadas para agregar la esfera a embree
void sphereBoundsFunc(const struct RTCBoundsFunctionArguments* args) 
{
	Esfera* esfera = (Esfera*) args->geometryUserPtr;
	RTCBounds* bounds_o = args->bounds_o;
	//Esfera* esfera = esferas[args->primID];
	float radio = esfera->getRadio();
	Vec3fa centro = esfera->getCentro();
	bounds_o->lower_x = centro.x - radio;
	bounds_o->lower_y = centro.y - radio;
	bounds_o->lower_z = centro.z - radio;
	bounds_o->upper_x = centro.x + radio;
	bounds_o->upper_y = centro.y + radio;
	bounds_o->upper_z = centro.z + radio;
}

inline void copyInstanceIdStack(const RTCIntersectContext* ctx, unsigned* tgt)
{
	tgt[0] = ctx->instID[0];
#if (RTC_MAX_INSTANCE_LEVEL_COUNT > 1)
	for (unsigned l = 1; l < RTC_MAX_INSTANCE_LEVEL_COUNT && l < ctx->instStackSize; ++l)
		tgt[l] = ctx->instID[l];
#endif
}


void sphereIntersectFunc(const RTCIntersectFunctionNArguments* args)
{
  int* valid = args->valid;
  void* ptr  = args->geometryUserPtr;
  Ray *ray = (Ray*)args->rayhit;
  RTCHit* hit = (RTCHit *)&ray->Ng.x;
  unsigned int primID = args->primID;
  
  assert(args->N == 1);
  Esfera*esfera = (Esfera*)ptr;
  
  if (!valid[0]) return;

  const Vec3fa v = ray->org-esfera->getCentro();
  const float A = dot(ray->dir,ray->dir);
  const float B = 2.0f*dot(v,ray->dir);
  const float C = dot(v,v) - sqr(esfera->getRadio());
  const float D = B*B - 4.0f*A*C;
  if (D < 0.0f) return;
  const float Q = sqrtf(D);
  const float rcpA = rcp(A);
  const float t0 = 0.5f*rcpA*(-B-Q);
  const float t1 = 0.5f*rcpA*(-B+Q);

  RTCHit potentialHit;
  potentialHit.u = 0.0f;
  potentialHit.v = 0.0f;
  copyInstanceIdStack(args->context, potentialHit.instID);
  potentialHit.geomID = args->geomID;
  potentialHit.primID = primID;
  if ((ray->tnear() < t0) & (t0 < ray->tfar))
  {
    int imask;
    bool mask = 1;
    {
      imask = mask ? -1 : 0;
    }
    
    const Vec3fa Ng = ray->org+t0*ray->dir-esfera->getCentro();
    potentialHit.Ng_x = Ng.x;
    potentialHit.Ng_y = Ng.y;
    potentialHit.Ng_z = Ng.z;

    RTCFilterFunctionNArguments fargs;
    fargs.valid = (int*)&imask;
    fargs.geometryUserPtr = ptr;
    fargs.context = args->context;
    fargs.ray = (RTCRayN *)args->rayhit;
    fargs.hit = (RTCHitN*)&potentialHit;
    fargs.N = 1;

    const float old_t = ray->tfar;
    ray->tfar = t0;
    rtcFilterIntersection(args,&fargs);

	if (imask == -1)
	{
		*hit = potentialHit;
	}
    else
      ray->tfar = old_t;
  }

  if ((ray->tnear() < t1) & (t1 < ray->tfar))
  {
    int imask;
    bool mask = 1;
    {
      imask = mask ? -1 : 0;
    }
    
    const Vec3fa Ng = ray->org+t1*ray->dir-esfera->getCentro();
    potentialHit.Ng_x = Ng.x;
    potentialHit.Ng_y = Ng.y;
    potentialHit.Ng_z = Ng.z;

    RTCFilterFunctionNArguments fargs;
    fargs.valid = (int*)&imask;
    fargs.geometryUserPtr = ptr;
    fargs.context = args->context;
    fargs.ray = (RTCRayN *)args->rayhit;
    fargs.hit = (RTCHitN*)&potentialHit;
    fargs.N = 1;

    const float old_t = ray->tfar;
    ray->tfar = t1;
    rtcFilterIntersection(args,&fargs);

	if (imask == -1)
	{
		*hit = potentialHit;
	}
    else
      ray->tfar = old_t;
  }
}

void sphereOccludedFunc(const RTCOccludedFunctionNArguments* args)
{
	int* valid = args->valid;
	void* ptr = args->geometryUserPtr;
	Ray *ray = (Ray*)args->ray;
	unsigned int primID = args->primID;

	assert(args->N == 1);
	Esfera* esfera = (Esfera*)ptr;

	if (!valid[0])
		return;

	const Vec3fa v = ray->org - esfera->getCentro();
	const float A = dot(ray->dir, ray->dir);
	const float B = 2.0f*dot(v, ray->dir);
	const float C = dot(v, v) - sqr(esfera->getRadio());
	const float D = B * B - 4.0f*A*C;
	if (D < 0.0f) return;
	const float Q = sqrtf(D);
	const float rcpA = rcp(A);
	const float t0 = 0.5f*rcpA*(-B - Q);
	const float t1 = 0.5f*rcpA*(-B + Q);

	RTCHit potentialHit;
	potentialHit.u = 0.0f;
	potentialHit.v = 0.0f;
	copyInstanceIdStack(args->context, potentialHit.instID);
	potentialHit.geomID = args->geomID;
	potentialHit.primID = primID;
	if ((ray->tnear() < t0) & (t0 < ray->tfar))
	{
		int imask;
		bool mask = 1;
		{
			imask = mask ? -1 : 0;
		}

		const Vec3fa Ng = ray->org + t0 * ray->dir - esfera->getCentro();
		potentialHit.Ng_x = Ng.x;
		potentialHit.Ng_y = Ng.y;
		potentialHit.Ng_z = Ng.z;

		RTCFilterFunctionNArguments fargs;
		fargs.valid = (int*)&imask;
		fargs.geometryUserPtr = ptr;
		fargs.context = args->context;
		fargs.ray = args->ray;
		fargs.hit = (RTCHitN*)&potentialHit;
		fargs.N = 1;

		const float old_t = ray->tfar;
		ray->tfar = t0;
		rtcFilterOcclusion(args, &fargs);

		if (imask == -1)
			ray->tfar = neg_inf;
		else
			ray->tfar = old_t;
	}

	if ((ray->tnear() < t1) & (t1 < ray->tfar))
	{
		int imask;
		bool mask = 1;
		{
			imask = mask ? -1 : 0;
		}

		const Vec3fa Ng = ray->org + t1 * ray->dir - esfera->getCentro();
		potentialHit.Ng_x = Ng.x;
		potentialHit.Ng_y = Ng.y;
		potentialHit.Ng_z = Ng.z;

		RTCFilterFunctionNArguments fargs;
		fargs.valid = (int*)&imask;
		fargs.geometryUserPtr = ptr;
		fargs.context = args->context;
		fargs.ray = args->ray;
		fargs.hit = (RTCHitN*)&potentialHit;
		fargs.N = 1;

		const float old_t = ray->tfar;
		ray->tfar = t1;
		rtcFilterOcclusion(args, &fargs);

		if (imask == -1)
			ray->tfar = neg_inf;
		else
			ray->tfar = old_t;
	}
}

unsigned int Esfera::agregarObjeto(RTCDevice device, RTCScene scene)
{
	RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_USER);
	unsigned int geomID = rtcAttachGeometry(scene, geom);
	//rtcCommitScene(scene);
	rtcSetGeometryUserPrimitiveCount(geom, 1);
	rtcSetGeometryUserData(geom, this);
	rtcSetGeometryBoundsFunction(geom, sphereBoundsFunc, nullptr);
	rtcSetGeometryIntersectFunction(geom, sphereIntersectFunc);
	rtcSetGeometryOccludedFunction(geom, sphereOccludedFunc);
	rtcCommitGeometry(geom);
	rtcReleaseGeometry(geom);
	return geomID;
}

float Esfera::getRadio()
{
	return radio;
}

Vec3fa Esfera::getCentro()
{
	return centro;
}

Esfera::~Esfera()
{
}

RTCBounds Esfera::getBounds()
{
	RTCBounds bounds;
	bounds.lower_x = centro.x - radio;
	bounds.lower_y = centro.y - radio;
	bounds.lower_z = centro.z - radio;
	bounds.upper_x = centro.x + radio;
	bounds.upper_y = centro.y + radio;
	bounds.upper_z = centro.z + radio;
	return bounds;
}