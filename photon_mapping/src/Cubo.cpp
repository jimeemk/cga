#include "../include/Cubo.h"


Cubo::Cubo(){

}
Cubo::Cubo(string path, Material m, Vec3fa c, float e, Vec3fa r) :Object(path, m,c,e,r)
{

}

unsigned int Cubo::agregarObjeto(RTCDevice device, RTCScene escena)
{
	colores_caras = (Vec3fa*)alignedMalloc2(12 * sizeof(Vec3fa), 16);
	colores_vertices = (Vec3fa*)alignedMalloc2(8 * sizeof(Vec3fa), 16);
	RTCGeometry cubo = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

	Vec3fa* vertices = (Vec3fa*)rtcSetNewGeometryBuffer(cubo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vec3fa), 8);
	int x_offset = 0;
	int y_offset = 3;
	int z_offset = 0;
	colores_vertices[0] = Vec3fa(0, 0, 0); vertices[0].x = -1 + x_offset; vertices[0].y = -1 + y_offset; vertices[0].z = -1 + z_offset;
	colores_vertices[1] = Vec3fa(0, 0, 1); vertices[1].x = -1 + x_offset; vertices[1].y = -1 + y_offset; vertices[1].z = +1 + z_offset;
	colores_vertices[2] = Vec3fa(0, 1, 0); vertices[2].x = -1 + x_offset; vertices[2].y = +1 + y_offset; vertices[2].z = -1 + z_offset;
	colores_vertices[3] = Vec3fa(0, 1, 1); vertices[3].x = -1 + x_offset; vertices[3].y = +1 + y_offset; vertices[3].z = +1 + z_offset;
	colores_vertices[4] = Vec3fa(1, 0, 0); vertices[4].x = +1 + x_offset; vertices[4].y = -1 + y_offset; vertices[4].z = -1 + z_offset;
	colores_vertices[5] = Vec3fa(1, 0, 1); vertices[5].x = +1 + x_offset; vertices[5].y = -1 + y_offset; vertices[5].z = +1 + z_offset;
	colores_vertices[6] = Vec3fa(1, 1, 0); vertices[6].x = +1 + x_offset; vertices[6].y = +1 + y_offset; vertices[6].z = -1 + z_offset;
	colores_vertices[7] = Vec3fa(1, 1, 1); vertices[7].x = +1 + x_offset; vertices[7].y = +1 + y_offset; vertices[7].z = +1 + z_offset;



	int tri = 0;
	Triangulo* triangulos = (Triangulo*)rtcSetNewGeometryBuffer(cubo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangulo), 12);

	// left side
	colores_caras[tri] = Vec3fa(1, 0, 0); triangulos[tri].v0 = 0; triangulos[tri].v1 = 1; triangulos[tri].v2 = 2; tri++;
	colores_caras[tri] = Vec3fa(1, 0, 0); triangulos[tri].v0 = 1; triangulos[tri].v1 = 3; triangulos[tri].v2 = 2; tri++;

	// right side
	colores_caras[tri] = Vec3fa(0, 1, 0); triangulos[tri].v0 = 4; triangulos[tri].v1 = 6; triangulos[tri].v2 = 5; tri++;
	colores_caras[tri] = Vec3fa(0, 1, 0); triangulos[tri].v0 = 5; triangulos[tri].v1 = 6; triangulos[tri].v2 = 7; tri++;

	// bottom side
	colores_caras[tri] = Vec3fa(0.5f);  triangulos[tri].v0 = 0; triangulos[tri].v1 = 4; triangulos[tri].v2 = 1; tri++;
	colores_caras[tri] = Vec3fa(0.5f);  triangulos[tri].v0 = 1; triangulos[tri].v1 = 4; triangulos[tri].v2 = 5; tri++;

	// top side
	colores_caras[tri] = Vec3fa(1.0f);  triangulos[tri].v0 = 2; triangulos[tri].v1 = 3; triangulos[tri].v2 = 6; tri++;
	colores_caras[tri] = Vec3fa(1.0f);  triangulos[tri].v0 = 3; triangulos[tri].v1 = 7; triangulos[tri].v2 = 6; tri++;

	// front side
	colores_caras[tri] = Vec3fa(0, 0, 1); triangulos[tri].v0 = 0; triangulos[tri].v1 = 2; triangulos[tri].v2 = 4; tri++;
	colores_caras[tri] = Vec3fa(0, 0, 1); triangulos[tri].v0 = 2; triangulos[tri].v1 = 6; triangulos[tri].v2 = 4; tri++;

	// back side
	colores_caras[tri] = Vec3fa(1, 1, 0); triangulos[tri].v0 = 1; triangulos[tri].v1 = 5; triangulos[tri].v2 = 3; tri++;
	colores_caras[tri] = Vec3fa(1, 1, 0); triangulos[tri].v0 = 3; triangulos[tri].v1 = 5; triangulos[tri].v2 = 7; tri++;


	rtcSetGeometryVertexAttributeCount(cubo, 1);
	Vec3fa* colVertices = &colores_vertices[0];
	rtcSetSharedGeometryBuffer(cubo, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, colVertices, 0, sizeof(Vec3fa), 8);

	rtcCommitGeometry(cubo);
	unsigned int geomID = rtcAttachGeometry(escena, cubo);
	rtcCommitScene(escena);
	rtcReleaseGeometry(cubo);
	return geomID;
}
