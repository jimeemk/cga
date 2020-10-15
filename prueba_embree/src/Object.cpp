#include "..\include\Object.h"

Object::Object()
{
}

Object::Object(string p, Material m, Vec3fa c, float escala, Vec3fa rot)
{
	path = p;
	material = m;
	centro = c;
	escalamiento = escala;
	rotacion = rot;
}

Object::Object(Material m, Vec3fa c, float e, Vec3fa r)
{
	material = m;
	centro = c;
	escalamiento = e;
	rotacion = r;
}

Object::~Object()
{
}

Object::Object(RTCGeometry geo, Vec3fa c)
{
	geometry = geo;
	diffuse_color = c;
}

RTCGeometry Object::getGeometry()
{
	return geometry;
}

Vec3fa Object::getDiffuseColor() 
{
	return diffuse_color;
}

Vec3fa* Object::getColoresCaras()
{
	return colores_caras;
}

Vec3fa* Object::getColoresVertices()
{
	return colores_vertices;
}

std::vector<float> Object::getVertices()
{
	return attrib.vertices;
}

std::vector<float> Object::getVertexWeights()
{
	return attrib.vertex_weights;
}

std::vector<float> Object::getNormales()
{
	return attrib.normals;
}

std::vector<float> Object::getTexCoords()
{
	return attrib.texcoords;
}

std::vector<float> Object::getTexCoordsws()
{
	return attrib.texcoord_ws;
}

std::vector<tinyobj::skin_weight_t> Object::getSkinWeights()
{
	return attrib.skin_weights;
}

Material Object::getMaterial()
{
	return material;
}

void* Object::alignedMalloc2(size_t size, size_t align)
{
	if (size == 0)
		return nullptr;

	assert((align & (align - 1)) == 0);
	void* ptr = _mm_malloc(size, align);

	if (size != 0 && ptr == nullptr)
		throw std::bad_alloc();

	return ptr;
}

unsigned int Object::agregarObjeto(RTCDevice device, RTCScene escena) {
	std::string warn, err;
	string pathObj = path + ".obj";
	string mtlObj = "Modelos";
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, pathObj.c_str(), mtlObj.c_str())) {
		throw std::runtime_error(warn + err);
	}

	RTCGeometry objeto = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

	colores_caras = (Vec3fa*)alignedMalloc2(shapes[0].mesh.num_face_vertices.size() * sizeof(Vec3fa), 16);
	colores_vertices = (Vec3fa*)alignedMalloc2(attrib.vertices.size() * sizeof(Vec3fa), 16);
	maxX, maxY, maxZ, minX, minY, minZ = 0;
	bool primeraVez = true;
	Vec3fa* vertices = (Vec3fa*)rtcSetNewGeometryBuffer(objeto, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vec3fa), attrib.vertices.size());
	for (int i = 0, ver = 0; i < attrib.vertices.size(); ver++)
	{
		float colores[3];
		vertices[ver].x = attrib.vertices.at(i); colores[0] = attrib.colors.at(i); i++;
		vertices[ver].y = attrib.vertices.at(i); colores[1] = attrib.colors.at(i); i++;
		vertices[ver].z = attrib.vertices.at(i); colores[2] = attrib.colors.at(i); i++;
		Vec3fa color = { 0.5, 0.5,0.5 };
		colores_vertices[ver] = color;
		if (!primeraVez)
		{
			if (vertices[ver].x > maxX)maxX = vertices[ver].x;
			if (vertices[ver].y > maxY)maxY = vertices[ver].y;
			if (vertices[ver].z > maxZ)maxZ = vertices[ver].z;
			if (vertices[ver].x < minX)minX = vertices[ver].x;
			if (vertices[ver].y < minY)minY = vertices[ver].y;
			if (vertices[ver].z < minZ)minZ = vertices[ver].z;
		}
		else
		{
			maxX = vertices[ver].x;
			maxY = vertices[ver].y;
			maxZ = vertices[ver].z;
			minX = vertices[ver].x;
			minY = vertices[ver].y;
			minZ = vertices[ver].z;
			primeraVez = false;
		}
		//colores_vertices.push_back(color);
	}
	float xCentro = ((maxX + minX) / 2);
	float yCentro = ((maxY + minY) / 2);
	float zCentro = ((maxZ + minZ) / 2);

	float ladoMax = getLadoMax(maxX - minX, maxY - minY, maxZ - minZ);

	float nuevoEscalamiento = escalamiento / ladoMax;

	for (int i = 0, ver = 0; i < attrib.vertices.size(); ver++)
	{
		vertices[ver] = trasladarVertice(vertices[ver], Vec3fa(-xCentro, -yCentro, -zCentro));
		vertices[ver] = escalarVertice(vertices[ver], nuevoEscalamiento);
		vertices[ver] = rotarVertice(vertices[ver], rotacion);
		vertices[ver] = trasladarVertice(vertices[ver], Vec3fa(centro.x,centro.y,centro.z));
		i = i + 3;
	}

	int tri = 0;
	Triangulo* triangulos = (Triangulo*)rtcSetNewGeometryBuffer(objeto, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangulo), shapes[0].mesh.num_face_vertices.size());

	for (int i = 0; tri < shapes[0].mesh.num_face_vertices.size(); tri++)
	{
		triangulos[tri].v0 = shapes[0].mesh.indices.at(i).vertex_index; i++;
		triangulos[tri].v1 = shapes[0].mesh.indices.at(i).vertex_index; i++;
		triangulos[tri].v2 = shapes[0].mesh.indices.at(i).vertex_index; i++;
		//colores_caras.push_back({ (float)shapes[0].mesh.material_ids.at(tri), (float)shapes[0].mesh.material_ids.at(tri), (float)shapes[0].mesh.material_ids.at(tri) });
		//colores_caras.push_back({0.2,0.7,0.9});
		//Vec3fa color = { (float)shapes[0].mesh.material_ids.at(tri),(float)shapes[0].mesh.material_ids.at(tri),(float)shapes[0].mesh.material_ids.at(tri) };
		Vec3fa color = { 0.2,0.7,0.9 };
		colores_caras[tri] = color;
	}
	rtcSetGeometryVertexAttributeCount(objeto, 1);
	Vec3fa* colVertices = &colores_vertices[0];
	rtcSetSharedGeometryBuffer(objeto, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, colVertices, 0, sizeof(Vec3fa), attrib.vertices.size());
	rtcCommitGeometry(objeto);
	unsigned int geomID = rtcAttachGeometry(escena, objeto);
	rtcCommitScene(escena);
	rtcReleaseGeometry(objeto);
	return geomID;
}

Vec3fa Object::trasladarVertice(Vec3fa inicial, Vec3fa t) {
	return Vec3fa(inicial.x + t.x, inicial.y + t.y, inicial.z + t.z);

}
Vec3fa Object::rotarVertice(Vec3fa inicial, Vec3fa r) {
	r = r * ((float)embree::pi / 180);
	Vec3fa rotX = Vec3fa(inicial.x, inicial.y*embree::cos(r.x)-inicial.z*embree::sin(r.x),inicial.y*embree::sin(r.x)+inicial.z*embree::cos(r.x));
	Vec3fa rotY = Vec3fa(rotX.x * embree::cos(r.y) + rotX.z * embree::sin(r.y), rotX.y, rotX.z * embree::cos(r.y) - rotX.x * embree::sin(r.y));
	Vec3fa rotZ = Vec3fa(rotY.x * embree::cos(r.z) - rotY.y * embree::sin(r.z), rotY.x * embree::sin(r.z) + rotY.y * embree::cos(r.z), rotY.z);
	return rotZ;
}
Vec3fa Object::escalarVertice(Vec3fa inicial, float e) {
	return inicial*e;

}

float Object::getLadoMax(float a, float b, float c)
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