#include "GUI.h"
#include "gui_scene.h"
#include "gui_camera.h"
#include "utils.h"

#include <core.h>

using namespace System;

namespace My3D {

/*
===============================================================================

  Конструктор / деструктор

===============================================================================
*/

Scene::Scene ()
{
  entity_map = gcnew EntityMap;
  type_map   = CreateTypeMap ();
  camera_map = CreateCameraMap ();
  light_map  = CreateLightMap ();
}

Scene::~Scene ()
{
  delete type_map;
  delete camera_map;
  delete light_map;
}

/*
===============================================================================

  Создание команд

===============================================================================
*/

Scene::TypeMap^ Scene::CreateTypeMap ()
{
  Scene::TypeMap^ typeMap = gcnew TypeMap ();

  typeMap->Add ("box",      gcnew Scene::Command (this, &Scene::CreateBox));
  typeMap->Add ("sphere",   gcnew Scene::Command (this, &Scene::CreateSphere));
  typeMap->Add ("cone",     gcnew Scene::Command (this, &Scene::CreateCone));
  typeMap->Add ("cylinder", gcnew Scene::Command (this, &Scene::CreateCylinder));
  typeMap->Add ("torus",    gcnew Scene::Command (this, &Scene::CreateTorus));

  typeMap->Add ("halfsphere",   gcnew Scene::Command (this, &Scene::CreateSphere));
  typeMap->Add ("halfcone",     gcnew Scene::Command (this, &Scene::CreateCone));
  typeMap->Add ("halfcylinder", gcnew Scene::Command (this, &Scene::CreateCylinder));
  typeMap->Add ("halftorus",    gcnew Scene::Command (this, &Scene::CreateTorus));

  typeMap->Add ("tetrahedron",  gcnew Scene::Command (this, &Scene::CreatePlatonicBody));
  typeMap->Add ("hexahedron",   gcnew Scene::Command (this, &Scene::CreatePlatonicBody));
  typeMap->Add ("octahedron",   gcnew Scene::Command (this, &Scene::CreatePlatonicBody));
  typeMap->Add ("dodecahedron", gcnew Scene::Command (this, &Scene::CreatePlatonicBody));
  typeMap->Add ("icosahedron",  gcnew Scene::Command (this, &Scene::CreatePlatonicBody));

  typeMap->Add ("light",  gcnew Scene::Command (this, &Scene::CreateLight));
  typeMap->Add ("camera",  gcnew Scene::Command (this, &Scene::CreateCamera));

  return typeMap;
}

Scene::CameraMap^ Scene::CreateCameraMap ()
{
  Scene::CameraMap^ cameraMap = gcnew CameraMap ();

  cameraMap->Add ("ortho",        gcnew Scene::Command (this, &Scene::CreateBaseCam));
  cameraMap->Add ("isometry",     gcnew Scene::Command (this, &Scene::CreateIsometryCam));
  cameraMap->Add ("dimetry",      gcnew Scene::Command (this, &Scene::CreateDimetryCam));
  cameraMap->Add ("trimetry",     gcnew Scene::Command (this, &Scene::CreateTrimetryCam));
  cameraMap->Add ("cabinet",      gcnew Scene::Command (this, &Scene::CreateBaseCam));
  cameraMap->Add ("free",         gcnew Scene::Command (this, &Scene::CreateBaseCam));
  cameraMap->Add ("perspective",  gcnew Scene::Command (this, &Scene::CreatePerspective1Cam));
//  cameraMap->Add ("perspective1", gcnew Scene::Command (this, &Scene::CreatePerspective1Cam));
//  cameraMap->Add ("perspective2", gcnew Scene::Command (this, &Scene::CreatePerspective1Cam));
  cameraMap->Add ("perspective_special", gcnew Scene::Command (this, &Scene::CreatePerspective3Cam));

  return cameraMap;
}

Scene::LightMap^ Scene::CreateLightMap ()
{
  Scene::LightMap^ lightMap = gcnew LightMap ();

  lightMap->Add ("point",    gcnew Scene::Command (this, &Scene::CreatePointLight));
  lightMap->Add ("spot",     gcnew Scene::Command (this, &Scene::CreateSpotLight));
  lightMap->Add ("direct",   gcnew Scene::Command (this, &Scene::CreateDirectLight));

  return lightMap;
}

/*
===============================================================================

  Создание источников света

===============================================================================
*/

Entity^ Scene::CreateLight (CoreEntityHandle handle)
{
  String^ light_type = gcnew String ( CoreGetEntityParams (handle, CORE_LIGHT_TYPE) );

  if ( !light_map->ContainsKey (light_type) )
    return nullptr;

  return light_map [light_type] (handle);
}

Entity^ Scene::CreatePointLight (CoreEntityHandle handle)
{
  return gcnew Light (handle);
}

Entity^ Scene::CreateSpotLight (CoreEntityHandle handle)
{
  return gcnew SpotLight (handle);
}

Entity^ Scene::CreateDirectLight (CoreEntityHandle handle)
{
  return gcnew Light (handle);
}

/*
===============================================================================

  Создание примитивов

===============================================================================
*/

Entity^ Scene::Create (CoreEntityHandle handle, String^ type)
{
  if ( !type_map->ContainsKey (type) )
    return nullptr;

  return type_map [type] (handle);
}

Entity^ Scene::CreateBox (CoreEntityHandle handle)
{
  return gcnew Box (handle);
}

Entity^ Scene::CreateSphere (CoreEntityHandle handle)
{
  return gcnew Sphere (handle);
}

Entity^ Scene::CreateCone (CoreEntityHandle handle)
{
  return gcnew Cone (handle);
}

Entity^ Scene::CreateCylinder (CoreEntityHandle handle)
{
  return gcnew Cylinder (handle);
}

Entity^ Scene::CreateTorus (CoreEntityHandle handle)
{
  return gcnew Torus (handle);
}

Entity^ Scene::CreatePlatonicBody (CoreEntityHandle handle)
{
  return gcnew PlatonicBody (handle);
}

/*
===============================================================================

  Создание камеры

===============================================================================
*/

Entity^ Scene::CreateCamera (CoreEntityHandle handle)
{
  String^ proj_type = gcnew String ( CoreGetEntityParams (handle, CORE_CAMERA_PROJECTION) );

  if ( !camera_map->ContainsKey (proj_type) )
    return nullptr;

  return camera_map [proj_type] (handle);
}

Entity^ Scene::CreateBaseCam (CoreEntityHandle handle)
{
  return gcnew BaseCamera (handle);
}

Entity^ Scene::CreateIsometryCam (CoreEntityHandle handle)
{
  return gcnew IsometryCamera (handle);
}

Entity^ Scene::CreateDimetryCam (CoreEntityHandle handle)
{
  return gcnew DimetryCamera (handle);
}

Entity^ Scene::CreateTrimetryCam (CoreEntityHandle handle)
{
  return gcnew TrimetryCamera (handle);
}

Entity^ Scene::CreatePerspective1Cam (CoreEntityHandle handle)
{
  return gcnew Perspective1Camera (handle);
}

Entity^ Scene::CreatePerspective2Cam (CoreEntityHandle handle)
{
  return gcnew Perspective2Camera (handle);
}

Entity^ Scene::CreatePerspective3Cam (CoreEntityHandle handle)
{
  return gcnew Perspective3Camera (handle);
}

/*
===============================================================================

  Обработка сообщений рендера  

===============================================================================
*/

Entity^ Scene::OnCreateEntity (CoreEntityHandle handle)
{
  String^ type = gcnew String (CoreGetEntityParams (handle, CORE_ENTITY_TYPE));

  if (type == "Unknown")
    return nullptr;

  GUI::Editor->ActivateCursor ();

  Entity^ entity = Create (handle,type);

  if (entity == nullptr)
    return entity;

  entity_map->Add (handle, entity);

  entity->OnCreate ();

  modified = true;

  return entity;
}

void Scene::OnDeleteEntity (CoreEntityHandle handle)
{
  Entity^ entity;

  if ( !entity_map->TryGetValue (handle,entity))
    return;

  entity->OnDelete ();

  delete entity;

  entity_map->Remove (handle);

  modified = true;
}

void Scene::OnModifyEntity (CoreEntityHandle handle)
{
  Entity^ entity;

  if (!entity_map->TryGetValue (handle,entity))
    return;
    
  entity->OnModify ();

  modified = true;
}

void Scene::OnSelect ()
{
  int selected_entities_count = CoreGetSelectedEntitiesCount ();

  array <System::Object^>^ selected_objects = gcnew array <System::Object^> (selected_entities_count);

  for (int i=0; i<CoreGetSelectedEntitiesCount (); ++i)
  {
    CoreEntityHandle handle = CoreGetSelectedEntity (i);

    Entity^ entity;

    if ( !entity_map->TryGetValue (handle, entity))
    {
      GUI::LogMessage (String::Format ("Selection: entity {0:X} not found", handle));
      continue;
    }

    selected_objects [i] = entity;
  }

  if (selected_entities_count == 1)
    GUI::PropertyControl->ActiveEntity = selected_objects [0];
  else
    GUI::PropertyControl->SelectedEntities = selected_objects;
}

}
