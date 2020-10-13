#include "../include/xml.h"

void guardarMapaFotones(const char* ruta, PhotonKDTree* kdtree)
{
	TiXmlDocument doc;
	TiXmlDeclaration* declaracion = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(declaracion);
	TiXmlElement* root = new TiXmlElement("file");
	doc.LinkEndChild(root);

	if (kdtree != NULL)
	{
		std::vector<Photon> fotones = kdtree->kNNValue(Vec3f(0.f), kdtree->size());
		std::vector<Photon>::iterator it;
		for (it = fotones.begin(); it != fotones.end(); ++it)
		{
			TiXmlElement* photon_ele = new TiXmlElement("photon");
			TiXmlElement* point_ele = new TiXmlElement("point");
			TiXmlElement* dir_ele = new TiXmlElement("dir");
			TiXmlElement* color_ele = new TiXmlElement("color");

			Photon ph = *it;

			point_ele->SetDoubleAttribute("x", ph.point.x);
			point_ele->SetDoubleAttribute("y", ph.point.y);
			point_ele->SetDoubleAttribute("z", ph.point.z);

			dir_ele->SetDoubleAttribute("x", ph.dir.x);
			dir_ele->SetDoubleAttribute("y", ph.dir.y);
			dir_ele->SetDoubleAttribute("z", ph.dir.z);

			color_ele->SetDoubleAttribute("x", ph.color.x);
			color_ele->SetDoubleAttribute("y", ph.color.y);
			color_ele->SetDoubleAttribute("z", ph.color.z);

			photon_ele->LinkEndChild(point_ele);
			photon_ele->LinkEndChild(dir_ele);
			photon_ele->LinkEndChild(color_ele);
			root->LinkEndChild(photon_ele);
		}
	}

	doc.SaveFile(ruta);
}

PhotonKDTree* cargarMapaFotones(const char* ruta)
{
	std::vector<Photon> fotones;
	
	TiXmlDocument doc(ruta);
	doc.LoadFile();

	TiXmlElement* root = doc.RootElement();

	if (root = doc.RootElement())
	{
		TiXmlElement* photon_ele = root->FirstChildElement("photon");
		
		while (photon_ele != NULL)
		{
			Photon photon;
			photon.color = Vec3f(0.f);
			photon.dir = Vec3f(0.f);
			photon.point = Vec3f(0.f);

			TiXmlElement* point_ele = photon_ele->FirstChildElement("point");
			TiXmlElement* dir_ele = photon_ele->FirstChildElement("dir");
			TiXmlElement* color_ele = photon_ele->FirstChildElement("color");

			float x, y, z;

			if (point_ele != NULL)
			{
				x = stof(point_ele->Attribute("x"));
				y = stof(point_ele->Attribute("y"));
				z = stof(point_ele->Attribute("z"));
				photon.point = Vec3f(x, y, z);
			}

			if (dir_ele != NULL)
			{
				x = stof(dir_ele->Attribute("x"));
				y = stof(dir_ele->Attribute("y"));
				z = stof(dir_ele->Attribute("z"));
				photon.dir = Vec3f(x, y, z);
			}

			if (color_ele != NULL)
			{
				x = stof(color_ele->Attribute("x"));
				y = stof(color_ele->Attribute("y"));
				z = stof(color_ele->Attribute("z"));
				photon.color = Vec3f(x, y, z);
			}
			fotones.push_back(photon);
			photon_ele = photon_ele->NextSiblingElement("photon");
		}
	}

	if (fotones.empty()) return NULL;
	else return new PhotonKDTree(fotones);
}

Scene* cargarEscena(const char* ruta)
{
	return NULL;
}

config cargarConfiguracion(const char* ruta)
{
	//Inicializacion de variable de retorno
	config conf; 
	conf.camara_from = Vec3f(15.f);
	conf.camara_to = Vec3f(0.f);
	conf.cant_fotones = 10000;
	conf.fov = 90;
	conf.height = conf.height = 800;

	//Cargar el archivo xml
	TiXmlDocument doc(ruta);
	doc.LoadFile();

	TiXmlElement* root = doc.RootElement();

	if (root != NULL)
	{
		TiXmlElement* camara = root->FirstChildElement("camara");
		TiXmlElement* imagen = root->FirstChildElement("imagen");
		TiXmlElement* cant_fotones = root->FirstChildElement("cant_fotones");

		if (cant_fotones != NULL) conf.cant_fotones = stoi(cant_fotones->GetText());
		
		if (camara != NULL)
		{
			TiXmlElement* from = camara->FirstChildElement("from");
			TiXmlElement* to = camara->FirstChildElement("to");

			float x, y, z;

			if (from != NULL)
			{
				x = stof(from->Attribute("x"));
				y = stof(from->Attribute("y"));
				z = stof(from->Attribute("z"));
				conf.camara_from = Vec3f(x, y, z);
			}

			if (to != NULL)
			{
				x = stof(to->Attribute("x"));
				y = stof(to->Attribute("y"));
				z = stof(to->Attribute("z"));
				conf.camara_to = Vec3f(x, y, z);
			}
		}

		if (imagen != NULL)
		{
			TiXmlElement* width = imagen->FirstChildElement("width");
			TiXmlElement* height = imagen->FirstChildElement("height");
			TiXmlElement* fov = imagen->FirstChildElement("fov");

			if (width != NULL) conf.width = stoi(width->GetText());
			if (height != NULL) conf.height = stoi(height->GetText());
			if (fov != NULL) conf.fov = stof(fov->GetText());
		}
	}
	
	return conf;
}