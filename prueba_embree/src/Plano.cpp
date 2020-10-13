#include "../include/Plano.h"

Plano::Plano()
{}
Plano::Plano(string path, Material m, Vec3fa centro, float escala, Vec3fa r) :Object(path, m,centro,escala,r)
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

	vertices[0].x = -20; vertices[0].y = -3; vertices[0].z = -20;
	vertices[1].x = -20; vertices[1].y = -3; vertices[1].z = +20;
	vertices[2].x = 20; vertices[2].y = -3; vertices[2].z = -20;
	vertices[3].x = +20; vertices[3].y = -3; vertices[3].z = +20;

	colores_vertices[0] = Vec3fa(0, 0, 0);
	colores_vertices[1] = Vec3fa(0, 0, 1);
	colores_vertices[2] = Vec3fa(0, 1, 0);
	colores_vertices[3] = Vec3fa(1, 0, 0);

	/* set triangles */
	Triangulo* triangles = (Triangulo*)rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangulo), 2);
	triangles[0].v0 = 0; triangles[0].v1 = 1; triangles[0].v2 = 2;
	triangles[1].v0 = 1; triangles[1].v1 = 3; triangles[1].v2 = 2;
	colores_caras[0] = Vec3fa(0, 0, 0.5);
	colores_caras[1] = Vec3fa(0.5, 0, 0);

	rtcSetGeometryVertexAttributeCount(mesh, 1);
	rtcSetSharedGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, colores_vertices, 0, sizeof(Vec3fa), 4);

	rtcCommitGeometry(mesh);
	unsigned int geomID = rtcAttachGeometry(scene, mesh);
	rtcCommitScene(scene);
	rtcReleaseGeometry(mesh);

	return geomID;
}
