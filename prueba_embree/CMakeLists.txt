# CMakeList.txt: proyecto de CMake para prueba_embree, incluya el origen y defina
# la lógica específica del proyecto aquí.
#

cmake_minimum_required (VERSION 3.8)
find_package(embree 3.0 REQUIRED)
set(embree_DIR "C:/Program Files/Intel/Embree3")
add_executable (prueba_embree "prueba_embree.cpp" "prueba_embree.h" )

target_link_libraries(prueba_embree PUBLIC embree)

# Agregue un origen al ejecutable de este proyecto.

set(FREEIMAGE_DIR "FreeImage/Dist/x64")
link_directories(${FREEIMAGE_DIR}/lib)
include_directories(${FREEIMAGE_DIR}/include)
target_link_libraries(prueba_embree PUBLIC FreeImage)


#link_libraries(FreeImage/Dist/x64/FreeImage)
#link_directories(FreeImage/Dist/x64/)

# TODO: Agregue pruebas y destinos de instalación si es necesario.

# Agregue un origen al ejecutable de este proyecto.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw-3.3.2/glfw-3.3.2)

target_link_libraries(prueba_embree PUBLIC glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(prueba_embree PUBLIC OpenGL::GL)

set(TBBDIR "tbb")
link_directories(${TBBDIR}/lib)
include_directories(${TBBDIR}/include)

##############################################################################
# Configurations (add configurations also to common/cmake/embree-config.cmake)

OPTION(EMBREE_ZIP_MODE "Create Embree ZIP package" OFF)
MARK_AS_ADVANCED(EMBREE_ZIP_MODE)

OPTION(EMBREE_INSTALL_DEPENDENCIES "Install Embree dependencies in binary packages and install" OFF)
MARK_AS_ADVANCED(EMBREE_INSTALL_DEPENDENCIES)

OPTION(EMBREE_STAT_COUNTERS "Enables statistic counters.")
OPTION(EMBREE_STACK_PROTECTOR "When enabled Embree compiles with stack protection against return address overrides." OFF)

OPTION(EMBREE_RAY_MASK "Enables ray mask support.")
OPTION(EMBREE_BACKFACE_CULLING "Enables backface culling.")
OPTION(EMBREE_FILTER_FUNCTION "Enables filter functions." ON)
OPTION(EMBREE_IGNORE_INVALID_RAYS "Ignores invalid rays." OFF) # FIXME: enable by default?
OPTION(EMBREE_COMPACT_POLYS "Enables double indexed poly layout." OFF)

OPTION(EMBREE_GEOMETRY_TRIANGLE "Enables support for triangle geometries." ON)
OPTION(EMBREE_GEOMETRY_QUAD "Enables support for quad geometries." ON)
OPTION(EMBREE_GEOMETRY_CURVE "Enables support for curve geometries." ON)
OPTION(EMBREE_GEOMETRY_SUBDIVISION "Enables support for subdiv geometries." ON)
OPTION(EMBREE_GEOMETRY_USER "Enables support for user geometries." ON)
OPTION(EMBREE_GEOMETRY_INSTANCE "Enables support for instances." ON)
OPTION(EMBREE_GEOMETRY_GRID "Enables support for grid geometries." ON)
OPTION(EMBREE_GEOMETRY_POINT "Enables support for point geometries." ON)
OPTION(EMBREE_RAY_PACKETS "Enabled support for ray packets." ON)

SET(EMBREE_MAX_INSTANCE_LEVEL_COUNT 1 CACHE STRING "Maximum number of instance levels.")
SET(EMBREE_CURVE_SELF_INTERSECTION_AVOIDANCE_FACTOR 2.0 CACHE STRING "Self intersection avoidance factor for flat curves. Specify floating point value in range 0 to inf.")
OPTION(EMBREE_MIN_WIDTH "Enables min-width feature to enlarge curve and point thickness to pixel width." OFF)

SET(EMBREE_TASKING_SYSTEM "TBB" CACHE STRING "Selects tasking system")
IF (WIN32)
  SET_PROPERTY(CACHE EMBREE_TASKING_SYSTEM PROPERTY STRINGS TBB INTERNAL PPL)
ELSE()
  SET_PROPERTY(CACHE EMBREE_TASKING_SYSTEM PROPERTY STRINGS TBB INTERNAL)
ENDIF()

IF (EMBREE_TASKING_SYSTEM STREQUAL "TBB")
  SET(TASKING_TBB      ON )
  SET(TASKING_INTERNAL OFF)
  SET(TASKING_PPL      OFF )
  ADD_DEFINITIONS(-DTASKING_TBB)
  LIST(APPEND ISPC_DEFINITIONS -DTASKING_TBB)
ELSEIF (EMBREE_TASKING_SYSTEM STREQUAL "PPL")
  SET(TASKING_PPL      ON )
  SET(TASKING_TBB      OFF )
  SET(TASKING_INTERNAL OFF)
  ADD_DEFINITIONS(-DTASKING_PPL)
  LIST(APPEND ISPC_DEFINITIONS -DTASKING_PPL)
ELSE()
  SET(TASKING_INTERNAL ON )
  SET(TASKING_TBB      OFF)
  SET(TASKING_PPL      OFF )
  ADD_DEFINITIONS(-DTASKING_INTERNAL)
  LIST(APPEND ISPC_DEFINITIONS -DTASKING_INTERNAL)
ENDIF()


