#include "../include/Settings.h"

Settings* Settings::instance = 0;

Settings* Settings::getInstance()
{
    if (instance == 0)
    {
        instance = new Settings();
    }

    return instance;
}

Settings::Settings()
{
    device = rtcNewDevice("threads=0");
    escena = rtcNewScene(device);
    rtcRetainDevice(device);
	scene = new Scene("escena1");
	kdtree = NULL;
}

RTCScene Settings::getEscena() {
    return escena;
}
RTCDevice Settings::getDevice() {
    return device;
}
Camera Settings::getCamara() {
	return camara;
}
std::vector<Object*> Settings::getObjetos()
{
    return scene->getObjects();
}
void Settings::addObject(Object* obj)
{
    scene->addObject(obj);
}
void Settings::addLight(Light* lig)
{
	scene->addLight(lig);
}
Object* Settings::getObject(int id)
{
    return scene->getObject(id);
}

PhotonKDTree* Settings::getKdTree() {
	return kdtree;
}
std::vector<Light*> Settings::getLights() {
	return scene->getLights();
}

Scene* Settings::getScene() {
	return scene;
}

void Settings::cargarConfiguraciones() {
	config configuracion = cargarConfiguracion("xml/config.xml");

	camara.from = configuracion.camara_from;
	camara.to = configuracion.camara_to;
	camara.fov = configuracion.fov;
	int width = configuracion.width;
	int height = configuracion.height;

	cargarEscena("xml/escenas/escena1.xml", scene);
	
	for (int i = 0; i < scene->getObjects().size(); i++)
	{
		scene->getObject(i)->agregarObjeto(device,escena);
	}

	rtcCommitScene(escena);

	std::string ruta_mf = "xml/mapa_fotones/" + scene->getNombre() + "-" + to_string(configuracion.cant_fotones) + ".txt";
	PhotonMapper* photon_mapper = new PhotonMapper();
	kdtree = cargarMapaFotones(ruta_mf.c_str());

	//Si no pudo cargar con el nombre segun el formato, emito los fotones y genero el xml
	if (kdtree == NULL)
	{
		std::cout << "No existia\n";
		kdtree = photon_mapper->emitPhotons(scene, configuracion.cant_fotones);
		guardarMapaFotones(ruta_mf.c_str(), kdtree);
	}
	else std::cout << "Ya existia\n";
}