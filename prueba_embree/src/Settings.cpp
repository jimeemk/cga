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

	//Esto ahora va a mano pero despues se carga del XML 

	SquareLight* light = new SquareLight(Vec3f(0.f, 15.0, 0.f), 10, 6, Vec3f(0.f, -1.f, 0.f), Vec3f(1.f, 0.f, 0.f));
	scene->addLight(light);
	
	Material rojo = { 0.1f, 0.4f , 0.4f , 0, 0, 0, Vec3fa(0.7f, 0, 0.1f) };
	Material azul = { 0.1f, 0.8f , 0.1f , 0, 0, 0, Vec3fa(0.1f, 0, 0.7f) };
	Material verde = { 0, 0.1f, 0.3f, 0, 0.6f, 1.0f, Vec3fa(0.1f, 0.6f, 0.1f) };
	Material naranja = { 0.2f, 0.6f, 0.2f , 0, 0, 1.4f, Vec3fa(0.9f, 0.6f, 0.1f) };

	Object* obj = new Object("Modelos/091_W_Aya_10K", rojo, Vec3fa(0,0,0), 0.025, Vec3fa(0, 0, 0));
	scene->addObject(obj);

	Object* obj2 = new Object("Modelos/face", azul, Vec3fa(-10, 0, 10), 1, Vec3fa(90, 0, 0));
	scene->addObject(obj2);

	Object* cubo = new Cubo("Modelos/12221_Cat_v1_l3", naranja, Vec3fa(10, 0, -4), 1, Vec3fa(90, 0, 0));
	scene->addObject(cubo);

	Object* plano = new Plano("Modelos/face", verde, Vec3fa(-5, 0, 5), 4, Vec3fa(0, 0, 0));
	scene->addObject(plano);

	//Fin de carga a mano
	
	for (int i = 0; i < scene->getObjects().size(); i++)
	{
		scene->getObject(i)->agregarObjeto(device,escena);
	}

	std::string ruta_mf = "xml/mapa_fotones/" + scene->getNombre() + "-" + to_string(configuracion.cant_fotones) + ".xml";
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