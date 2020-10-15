#include "../include/Plano.h"

Plano::Plano()
{
}

Plano::Plano(Material m, Vec3fa centro, float s, Vec3fa n, Vec3fa r) : Object(m, centro, s, r)
{
	normal = normalize(n);
}

Plano::~Plano()
{

}

unsigned int Plano::agregarObjeto(RTCDevice device, RTCScene scene)
{
	/* create a triangulated plane with 2 triangles and 4 vertices */
	colores_caras = (Vec3fa*)alignedMalloc2(2 * sizeof(Vec3fa), 16);
	colores_vertices = (Vec3fa*)alignedMalloc2(4 * sizeof(Vec3fa), 16);
	RTCGeometry mesh = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
	/* set vertices */
	Vec3fa* vertices = (Vec3fa*)rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vec3fa), 4);

	Vec3fa up = cross(normal, normalize(rotacion));
	Vec3fa r = cross(up, normal);

	//abajo a la izquierda
	vertices[0] = centro + (escalamiento * 0.5) * up + (escalamiento * 0.5) * r;
	vertices[1] = centro + (escalamiento * 0.5) * up - (escalamiento * 0.5) * r;
	vertices[2] = centro - (escalamiento * 0.5) * up + (escalamiento * 0.5) * r;
	vertices[3] = centro - (escalamiento * 0.5) * up - (escalamiento * 0.5) * r;

	colores_vertices[0] = material.color;
	colores_vertices[1] = material.color;
	colores_vertices[2] = material.color;
	colores_vertices[3] = material.color;

	/* set triangles */
	Triangulo* triangles = (Triangulo*)rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangulo), 2);
	triangles[0].v0 = 0; triangles[0].v1 = 1; triangles[0].v2 = 2;
	triangles[1].v0 = 1; triangles[1].v1 = 3; triangles[1].v2 = 2;
	colores_caras[0] = material.color;
	colores_caras[1] = material.color;

	rtcSetGeometryVertexAttributeCount(mesh, 1);
	rtcSetSharedGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, colores_vertices, 0, sizeof(Vec3fa), 4);

	rtcCommitGeometry(mesh);
	unsigned int geomID = rtcAttachGeometry(scene, mesh);
	rtcCommitScene(scene);
	rtcReleaseGeometry(mesh);

	return geomID;
}
