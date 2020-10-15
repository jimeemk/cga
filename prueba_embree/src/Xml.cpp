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

void cargarEscena(const char* ruta, Scene* scene)
{
	if (scene != NULL)
	{
		std::map<std::string, Material> materiales;

		//Material que se le asigna al obj si se introduce el nombre de algun material
		//que no haya sido cargado
		Material default = { 0.1f, 0.8f , 0.1f , 0, 0, 0, Vec3fa(1.0, 1.0, 1.0) };

		TiXmlDocument doc(ruta);
		doc.LoadFile();
		TiXmlElement* root = doc.RootElement();
		if (root != NULL)
		{
			TiXmlElement* nombre_ele = root->FirstChildElement("nombre");
			TiXmlElement* mats_ele = root->FirstChildElement("materiales");
			TiXmlElement* objetos_ele = root->FirstChildElement("objetos");
			TiXmlElement* luces_ele = root->FirstChildElement("luces");

			//Cambiar el nombre de la escena
			if (nombre_ele != NULL) scene->setNombre(nombre_ele->GetText());
			
			//Cargar materiales (solo para instanciar a los objetos)
			if (mats_ele != NULL)
			{
				TiXmlElement* material_ele = mats_ele->FirstChildElement("material");

				while (material_ele != NULL)
				{
					Material material = default;
					TiXmlElement* color_ele = material_ele->FirstChildElement("color");
					if (color_ele != NULL)
					{
						float r = stof(color_ele->Attribute("r"));
						float g = stof(color_ele->Attribute("g"));
						float b = stof(color_ele->Attribute("b"));
						material.color = Vec3fa(r, g, b);
					}

					std::string nombre = material_ele->Attribute("nombre");
					material.coef_ambiente = stof(material_ele->Attribute("coef_a"));
					material.coef_difuso = stof(material_ele->Attribute("coef_d"));
					material.coef_especular = stof(material_ele->Attribute("coef_e"));
					material.coef_reflexion = stof(material_ele->Attribute("coef_r"));
					material.coef_transparencia = stof(material_ele->Attribute("coef_t"));
					material.indice_refraccion = stof(material_ele->Attribute("indice_r"));

					materiales.insert(std::pair<std::string, Material>(nombre, material));
					material_ele = material_ele->NextSiblingElement("material");
				}
			}

			//Cargar objetos y agregarlos a la escena
			if (objetos_ele != NULL)
			{
				TiXmlElement* obj_ele = objetos_ele->FirstChildElement("obj");
				TiXmlElement* plano_ele = objetos_ele->FirstChildElement("plano");

				while (obj_ele != NULL)
				{
					Vec3fa centro = Vec3fa(0.f);
					Vec3fa rotacion = centro;

					TiXmlElement* centro_ele = obj_ele->FirstChildElement("centro");
					TiXmlElement* rotacion_ele = obj_ele->FirstChildElement("rotacion");

					if (centro_ele != NULL)
					{
						float x = stof(centro_ele->Attribute("x"));
						float y = stof(centro_ele->Attribute("y"));
						float z = stof(centro_ele->Attribute("z"));
						centro = Vec3fa(x, y, z);
					}

					if (rotacion_ele != NULL)
					{
						float x = stof(rotacion_ele->Attribute("x"));
						float y = stof(rotacion_ele->Attribute("y"));
						float z = stof(rotacion_ele->Attribute("z"));
						rotacion = Vec3fa(x, y, z);
					}

					std::string path = obj_ele->Attribute("path");
					std::string material = obj_ele->Attribute("material");
					float escala = stof(obj_ele->Attribute("escala"));

					Material mat = default;
					if (materiales.find(material) != materiales.end()) mat = materiales.at(material);
					Object* obj = new Object(path, mat, centro, escala, rotacion);
					scene->addObject(obj);
					obj_ele = obj_ele->NextSiblingElement("obj");
				}

				while (plano_ele != NULL)
				{
					Vec3fa centro = Vec3fa(0.f);
					Vec3fa normal = Vec3fa(0.f, 1.f, 0.f);
					Vec3fa derecha = Vec3fa(1.f, 0.f, 0.f);

					TiXmlElement* centro_ele = plano_ele->FirstChildElement("centro");
					TiXmlElement* normal_ele = plano_ele->FirstChildElement("normal");
					TiXmlElement* derecha_ele = plano_ele->FirstChildElement("derecha");

					float x, y, z;

					if (centro_ele != NULL)
					{
						x = stof(centro_ele->Attribute("x"));
						y = stof(centro_ele->Attribute("y"));
						z = stof(centro_ele->Attribute("z"));
						centro = Vec3fa(x, y, z);
					}

					if (normal_ele != NULL)
					{
						x = stof(normal_ele->Attribute("x"));
						y = stof(normal_ele->Attribute("y"));
						z = stof(normal_ele->Attribute("z"));
						normal = Vec3fa(x, y, z);
					}

					if (derecha_ele != NULL)
					{
						x = stof(derecha_ele->Attribute("x"));
						y = stof(derecha_ele->Attribute("y"));
						z = stof(derecha_ele->Attribute("z"));
						derecha = Vec3fa(x, y, z);
					}

					std::string material = plano_ele->Attribute("material");
					float size = stof(plano_ele->Attribute("size"));

					Material mat = default;
					if (materiales.find(material) != materiales.end()) mat = materiales.at(material);
					Plano* plano = new Plano(mat, centro, size, normal, derecha);
					scene->addObject(plano);

					plano_ele = objetos_ele->NextSiblingElement("plano");
				}

			}

			//Cargar las luces y agregarlas a la escena
			if (luces_ele != NULL)
			{
				TiXmlElement* puntual_ele = luces_ele->FirstChildElement("puntual");
				TiXmlElement* cuadrada_ele = luces_ele->FirstChildElement("cuadrada");

				while (puntual_ele != NULL)
				{
					Vec3f centro = Vec3f(0.f);
					TiXmlElement* centro_ele = puntual_ele->FirstChildElement("centro");

					if (centro_ele != NULL)
					{
						centro.x = stof(centro_ele->Attribute("x"));
						centro.y = stof(centro_ele->Attribute("y"));
						centro.z = stof(centro_ele->Attribute("z"));
					}

					float potencia = stof(puntual_ele->Attribute("potencia"));

					Light* puntual = new Light(centro, potencia);
					scene->addLight(puntual);

					puntual_ele = puntual_ele->NextSiblingElement("puntual");
				}

				while (cuadrada_ele != NULL)
				{
					Vec3f centro = Vec3f(0.f);
					Vec3f normal = centro;
					Vec3f derecha = centro;

					TiXmlElement* centro_ele = cuadrada_ele->FirstChildElement("centro");
					TiXmlElement* normal_ele = cuadrada_ele->FirstChildElement("normal");
					TiXmlElement* derecha_ele = cuadrada_ele->FirstChildElement("derecha");

					if (centro_ele != NULL)
					{
						centro.x = stof(centro_ele->Attribute("x"));
						centro.y = stof(centro_ele->Attribute("y"));
						centro.z = stof(centro_ele->Attribute("z"));
					}
					
					if (normal_ele != NULL)
					{
						normal.x = stof(normal_ele->Attribute("x"));
						normal.y = stof(normal_ele->Attribute("y"));
						normal.z = stof(normal_ele->Attribute("z"));
					}

					if (derecha_ele != NULL)
					{
						derecha.x = stof(derecha_ele->Attribute("x"));
						derecha.y = stof(derecha_ele->Attribute("y"));
						derecha.z = stof(derecha_ele->Attribute("z"));
					}

					float potencia = stof(cuadrada_ele->Attribute("potencia"));
					float size = stof(cuadrada_ele->Attribute("size"));

					SquareLight* cuadrada = new SquareLight(centro, potencia, size, normal, derecha);
					scene->addLight(cuadrada);

					cuadrada_ele = cuadrada_ele->NextSiblingElement("cuadrada");
				}
			}
		}
	}
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