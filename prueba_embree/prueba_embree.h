// prueba_embree.h: archivo de inclusión para archivos de inclusión estándar del sistema,
// o archivos de inclusión específicos de un proyecto.

#pragma once
#define TINYOBJLOADER_IMPLEMENTATION
#include <iostream>

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include "common/math/vec2.h"
#include "common/math/vec3.h"
#include "common/camera.h"
#include <GLFW/glfw3.h>
#include <tbb/tbb.h>
#include "common/algorithms/parallel_for.h"
#include "common/tasking/taskscheduler.h"
#include <FreeImage.h>
#include "tiny_obj_loader.h"

#include "include/PhotonMapper.h"
#include "include/Object.h"

// TODO: Haga referencia aquí a los encabezados adicionales que el programa requiere.
