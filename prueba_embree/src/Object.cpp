#include "..\include\Object.h"
#include "..\include\Object.h"

Object::Object()
{
}

Object::~Object()
{
}

Object::Object(RTCGeometry geo, Vec3f c)
{
	geometry = geo;
	diffuse_color = c;
}

RTCGeometry Object::getGeometry()
{
	return geometry;
}

Vec3f Object::getDiffuseColor() 
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

void* alignedMalloc2(size_t size, size_t align)
{
	if (size == 0)
		return nullptr;

	assert((align & (align - 1)) == 0);
	void* ptr = _mm_malloc(size, align);

	if (size != 0 && ptr == nullptr)
		throw std::bad_alloc();

	return ptr;
}

unsigned int Object::agregarObjeto(RTCDevice device, RTCScene escena, string path) {
	std::string warn, err;
	string pathObj = path + ".obj";
	string mtlObj = "Modelos";
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, pathObj.c_str(), mtlObj.c_str())) {
		throw std::runtime_error(warn + err);
	}

	RTCGeometry objeto = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

	colores_caras = (Vec3fa*)alignedMalloc2(shapes[0].mesh.num_face_vertices.size() * sizeof(Vec3fa), 16);
	colores_vertices = (Vec3fa*)alignedMalloc2(attrib.vertices.size() * sizeof(Vec3fa), 16);

	Vec3fa* vertices = (Vec3fa*)rtcSetNewGeometryBuffer(objeto, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vec3fa), attrib.vertices.size());
	for (int i = 0, ver = 0; i < attrib.vertices.size(); ver++)
	{
		float colores[3];
		vertices[ver].x = attrib.vertices.at(i); colores[0] = attrib.colors.at(i); i++;
		vertices[ver].y = attrib.vertices.at(i); colores[1] = attrib.colors.at(i); i++;
		vertices[ver].z = attrib.vertices.at(i); colores[2] = attrib.colors.at(i); i++;
		Vec3fa color = { 0.5, 0.5,0.5 };
		colores_vertices[ver] = color;
		//colores_vertices.push_back(color);
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