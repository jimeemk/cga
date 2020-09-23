# CMakeList.txt: proyecto de CMake para prueba_embree, incluya el origen y defina
# la lógica específica del proyecto aquí.
#

cmake_minimum_required (VERSION 3.8)
find_package(embree 3.0 REQUIRED)
set(embree_DIR "C:/Program Files/Intel/Embree3")
add_executable (prueba_embree "prueba_embree.cpp" "prueba_embree.h")

target_link_libraries(prueba_embree embree)

# Agregue un origen al ejecutable de este proyecto.



# TODO: Agregue pruebas y destinos de instalación si es necesario.

# Agregue un origen al ejecutable de este proyecto.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(E:/Documentos/prueba_embree/prueba_embree/prueba_embree/glfw-3.3.2/glfw-3.3.2)

target_link_libraries(prueba_embree glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(prueba_embree OpenGL::GL)


