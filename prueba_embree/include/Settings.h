#pragma once
#include <iostream>

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

#include <GLFW/glfw3.h>
#include "Object.h"
#include <FreeImage.h>
#include <map>
#include "Light.h"
#include "SquareLight.h"
#include "Xml.h"
#include "PhotonMapper.h"
#include "Plano.h"
#include "Cubo.h"
#include "Esfera.h"

#include "../common/camera.h"


using namespace std;

class Settings
{
private:
    /* Here will be the instance stored. */
    static Settings* instance;
    RTCScene escena;
    RTCDevice device;
    Camera camara;
    PhotonKDTree* kdtree;
    PhotonKDTree* caustree;
    Scene* scene;
    int width;
    int height;
    /* Private constructor to prevent instancing. */
    Settings();

public:
    /* Static access method. */

    static Settings* getInstance();
    RTCScene getEscena();
    RTCDevice getDevice();
    Camera getCamara();
    PhotonKDTree* getKdTree();
    PhotonKDTree* getCausTree();
    std::vector<Light*> getLights();
    std::vector<Object*> getObjetos();
    Scene* getScene();
    void addObject(Object* obj);
    void addLight(Light* lig);
    int getWidth();
    int getHeight();
    Object* getObject(int id);
    void cargarConfiguraciones();
};