#include "gui_scene.h"
#include "gui_vector.h"
#include "editorform.h"
#include "propertyform.h"
#include "utils.h"
#include "GUI.h"

#include <core.h>

using namespace System;
using namespace System::Drawing;

namespace My3D {

/*
===============================================================================

  Конструктор / деструктор

===============================================================================
*/

Entity::Entity (CoreEntityHandle _handle)
{
  handle = _handle;

  local_position = gcnew Vec3f ();
  local_rotation = gcnew Vec3f ();
  local_scale    = gcnew Vec3f ();
  local_dir      = gcnew Vec3f ();
  local_up       = gcnew Vec3f ();

  global_position = gcnew Vec3f ();
  global_rotation = gcnew Vec3f ();
  global_scale    = gcnew Vec3f ();
  global_dir      = gcnew Vec3f ();
  global_up       = gcnew Vec3f ();
}

Entity::~Entity ()
{
  delete local_position;
  delete local_rotation;
  delete local_scale;
  delete local_dir;
  delete local_up;

  delete global_position;
  delete global_rotation;
  delete global_scale;
  delete global_dir;
  delete global_up;
}

/*
===============================================================================

  Свойства Entity

===============================================================================
*/

#pragma region Properties

String^ Entity::Name::get (void)
{
  return name;
}

void Entity::Name::set (String^ value)
{
  name = value;
  CoreSetEntityParams (handle, CORE_ENTITY_NAME, (AutoStr (value)).c_str ());
  GUI::PropertyControl->UpdateEntityName (this);
}

String^ Entity::Parent::get (void)
{
  String^ parent = gcnew String (CoreGetEntityParams (handle, CORE_ENTITY_PARENT_NAME));
  return parent;
}

String^ Entity::Type::get (void)
{
  return type;
}
/*
void Entity::Type::set (String^ value)
{
  type = value;
  CoreSetEntityParams (handle, CORE_ENTITY_TYPE, (AutoStr (value)).c_str ());
}
*/
Color Entity::WireColor::get ()
{
  return color;
}
void Entity::WireColor::set (Color value)
{
  color = value;
  float rgb [3] = { value.R / 255.0f, value.G / 255.0f, value.B / 255.0f};
  CoreSetEntityParam3fv (handle, CORE_ENTITY_WIRE_COLOR, rgb);
}

Vec3f^ Entity::LocalPosition::get ()
{
  return local_position;
}

void Entity::LocalPosition::set (Vec3f^ value)
{
  local_position->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_ENTITY_LOCAL_POSITION, vec3f);
}

Vec3f^ Entity::LocalRotation::get ()
{
  return local_rotation;
}

void Entity::LocalRotation::set (Vec3f^ value)
{
  local_rotation->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_ENTITY_LOCAL_ROTATION, vec3f);
}

Vec3f^ Entity::LocalScale::get ()
{
  return local_scale;
}

void Entity::LocalScale::set (Vec3f^ value)
{
  local_scale->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_ENTITY_LOCAL_SCALE, vec3f);
}

Vec3f^ Entity::LocalDir::get ()
{
  return local_dir;
}
/*
void Entity::LocalDir::set (Vec3f^ value)
{
  local_dir->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_ENTITY_LOCAL_DIR, vec3f);
}
*/
Vec3f^ Entity::LocalUp::get ()
{
  return local_up;
}
/*
void Entity::LocalUp::set (Vec3f^ value)
{
  local_up->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_ENTITY_LOCAL_UP, vec3f);
}
*/
Vec3f^ Entity::GlobalPosition::get ()
{
  return global_position;
}

void Entity::GlobalPosition::set (Vec3f^ value)
{
  global_position->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_ENTITY_WORLD_POSITION, vec3f);
}

Vec3f^ Entity::GlobalRotation::get ()
{
  return global_rotation;
}

void Entity::GlobalRotation::set (Vec3f^ value)
{
  global_rotation->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_ENTITY_WORLD_ROTATION, vec3f);
}

Vec3f^ Entity::GlobalScale::get ()
{
  return global_scale;
}

void Entity::GlobalScale::set (Vec3f^ value)
{
  global_scale->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_ENTITY_WORLD_SCALE, vec3f);
}

Vec3f^ Entity::GlobalDir::get ()
{
  return global_dir;
}
/*
void Entity::GlobalDir::set (Vec3f^ value)
{
  global_dir->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_ENTITY_WORLD_DIR, vec3f);
}
*/
Vec3f^ Entity::GlobalUp::get ()
{
  return global_up;
}
/*
void Entity::GlobalUp::set (Vec3f^ value)
{
  global_up->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_ENTITY_WORLD_UP, vec3f);
}
*/
bool Entity::Visible::get ()
{
  return visible;
}

void Entity::Visible::set (bool value)
{
  visible = value;
  CoreSetEntityParami (handle, CORE_ENTITY_VISIBLE, visible?1:0);
}

bool Entity::Selected::get ()
{
  return selected;
}

#pragma endregion

/*
===============================================================================

  Создание / Удаление / Изменение параметров

===============================================================================
*/

void Entity::OnCreate ()
{
  GUI::PropertyControl->AddEntity (this);

  OnModify ();
}

void Entity::OnDelete ()
{
  GUI::PropertyControl->DelEntity (this);
}

void Entity::OnModify ()
{
  name = gcnew String ( CoreGetEntityParams (handle, CORE_ENTITY_NAME) );

  type = gcnew String ( CoreGetEntityParams (handle, CORE_ENTITY_TYPE) );

  visible = (bool) CoreGetEntityParami (handle, CORE_ENTITY_VISIBLE);

  selected = (bool) CoreGetEntityParami (handle,CORE_ENTITY_SELECT);

  float vec3f [3];

  CoreGetEntityParam3fv (handle, CORE_ENTITY_WIRE_COLOR, vec3f);
  color = Color::FromArgb (vec3f[0]*255, vec3f[1]*255, vec3f[2]*255);

  CoreGetEntityParam3fv (handle, CORE_ENTITY_LOCAL_POSITION, vec3f);
  local_position->assign (vec3f);

  CoreGetEntityParam3fv (handle, CORE_ENTITY_LOCAL_SCALE, vec3f);
  local_scale->assign (vec3f);

  CoreGetEntityParam3fv (handle, CORE_ENTITY_LOCAL_ROTATION, vec3f);
  local_rotation->assign (vec3f);

  CoreGetEntityParam3fv (handle, CORE_ENTITY_LOCAL_DIR, vec3f);
  local_dir->assign (vec3f);

  CoreGetEntityParam3fv (handle, CORE_ENTITY_LOCAL_UP, vec3f);
  local_up->assign (vec3f);

  CoreGetEntityParam3fv (handle, CORE_ENTITY_WORLD_POSITION, vec3f);
  global_position->assign (vec3f);

  CoreGetEntityParam3fv (handle, CORE_ENTITY_WORLD_ROTATION, vec3f);
  global_scale->assign (vec3f);

  CoreGetEntityParam3fv (handle, CORE_ENTITY_WORLD_SCALE, vec3f);
  global_rotation->assign (vec3f);

  CoreGetEntityParam3fv (handle, CORE_ENTITY_WORLD_DIR, vec3f);
  global_dir->assign (vec3f);

  CoreGetEntityParam3fv (handle, CORE_ENTITY_WORLD_UP, vec3f);
  global_up->assign (vec3f);

  if (GUI::PropertyControl->ActiveEntity == this)
    GUI::PropertyControl->EntityProperties->Refresh ();

/*
  if (selected != (bool) CoreGetEntityParami (handle,CORE_ENTITY_SELECT))
  {
    if (selected == false)
      GUI::PropertyControl->ActiveEntity = this;
    else
      GUI::PropertyControl->EntityProperties->Refresh ();

    selected = !selected;
  }

  if (selected == true)
    GUI::PropertyControl->EntityProperties->Refresh ();
*/
}

/*
===============================================================================

  Helpers

===============================================================================
*/

CoreEntityHandle Entity::GetHandle ()
{
  return handle;
}

String^ Entity::ToString ()
{
  return name;
}

/*
===============================================================================

  Model Entity

===============================================================================
*/

/*
===============================================================================

  Конструктор / деструктор

===============================================================================
*/

ModelEntity::ModelEntity (CoreEntityHandle _handle) : Entity (_handle)
{
  OnModify ();
}

ModelEntity::~ModelEntity ()
{
}

/*
===============================================================================

  Свойства ModelEntity

===============================================================================
*/

#pragma region Properties


System::Single ModelEntity::ShearXY::get ()
{
  return shear_xy;
}

void ModelEntity::ShearXY::set (System::Single value)
{
  shear_xy = value;
  CoreSetEntityParamf (handle, CORE_MODEL_SHEAR_XY, value);
}

System::Single ModelEntity::ShearZY::get ()
{
  return shear_zy;
}

void ModelEntity::ShearZY::set (System::Single value)
{
  shear_zy = value;
  CoreSetEntityParamf (handle, CORE_MODEL_SHEAR_ZY, value);
}

Color ModelEntity::Ambient::get ()
{
  return ambient_color;
}

void ModelEntity::Ambient::set (Color value)
{
  ambient_color = value;
  float rgb [3] = { value.R / 255.0f, value.G / 255.0f, value.B / 255.0f};
  CoreSetEntityParam3fv (handle, CORE_MATERIAL_AMBIENT_COLOR, rgb);
}

Color ModelEntity::Diffuse::get ()
{
  return diffuse_color;
}

void ModelEntity::Diffuse::set (Color value)
{
  diffuse_color = value;
  float rgb [3] = { value.R / 255.0f, value.G / 255.0f, value.B / 255.0f};
  CoreSetEntityParam3fv (handle, CORE_MATERIAL_DIFFUSE_COLOR, rgb);
}

Color ModelEntity::Specular::get ()
{
  return specular_color;
}

void ModelEntity::Specular::set (Color value)
{
  specular_color = value;
  float rgb [3] = { value.R / 255.0f, value.G / 255.0f, value.B / 255.0f};
  CoreSetEntityParam3fv (handle, CORE_MATERIAL_SPECULAR_COLOR, rgb);
}

Color ModelEntity::Emission::get ()
{
  return emission_color;
}

void ModelEntity::Emission::set (Color value)
{
  emission_color = value;
  float rgb [3] = { value.R / 255.0f, value.G / 255.0f, value.B / 255.0f};
  CoreSetEntityParam3fv (handle, CORE_MATERIAL_EMISSION_COLOR, rgb);
}

System::Single ModelEntity::Shininess::get ()
{
  return shininess;
}

void ModelEntity::Shininess::set (System::Single value)
{
  shininess = value;
  CoreSetEntityParamf (handle, CORE_MATERIAL_SHININESS, value);
}

System::Single ModelEntity::Transparency::get ()
{
  return transparency;
}

void ModelEntity::Transparency::set (System::Single value)
{
  transparency = value;
  CoreSetEntityParamf (handle, CORE_MATERIAL_TRANSPARENCY, value);
}

String^ ModelEntity::BlendMode::get (void)
{
  return blend_mode;
}

void ModelEntity::BlendMode::set (String^ value)
{
  blend_mode = value;
  CoreSetEntityParams (handle, CORE_MATERIAL_BLEND_MODE, (AutoStr (value)).c_str ());
}

bool ModelEntity::Wireframe::get ()
{
  return wireframe;
}

void ModelEntity::Wireframe::set (bool value)
{
  wireframe = value;
  CoreSetEntityParami (handle, CORE_MATERIAL_WIREFRAME, wireframe?1:0);
}

bool ModelEntity::TwoSided::get ()
{
  return two_sided;
}

void ModelEntity::TwoSided::set (bool value)
{
  two_sided = value;
  CoreSetEntityParami (handle, CORE_MATERIAL_TWO_SIDED, two_sided?1:0);
}

#pragma endregion

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void ModelEntity::OnModify ()
{
  blend_mode = gcnew String ( CoreGetEntityParams (handle, CORE_MATERIAL_BLEND_MODE) );

  shininess = CoreGetEntityParamf (handle, CORE_MATERIAL_SHININESS);
  transparency = CoreGetEntityParamf (handle, CORE_MATERIAL_TRANSPARENCY);

  wireframe = (bool) CoreGetEntityParami (handle, CORE_MATERIAL_WIREFRAME);
  two_sided = (bool) CoreGetEntityParami (handle, CORE_MATERIAL_TWO_SIDED);

  shear_xy = CoreGetEntityParamf (handle, CORE_MODEL_SHEAR_XY);
  shear_zy = CoreGetEntityParamf (handle, CORE_MODEL_SHEAR_ZY);

  float vec3f [3];

  CoreGetEntityParam3fv (handle, CORE_MATERIAL_AMBIENT_COLOR, vec3f);
  ambient_color = Color::FromArgb (vec3f[0]*255, vec3f[1]*255, vec3f[2]*255);

  CoreGetEntityParam3fv (handle, CORE_MATERIAL_DIFFUSE_COLOR, vec3f);
  diffuse_color = Color::FromArgb (vec3f[0]*255, vec3f[1]*255, vec3f[2]*255);

  CoreGetEntityParam3fv (handle, CORE_MATERIAL_SPECULAR_COLOR, vec3f);
  specular_color = Color::FromArgb (vec3f[0]*255, vec3f[1]*255, vec3f[2]*255);

  CoreGetEntityParam3fv (handle, CORE_MATERIAL_EMISSION_COLOR, vec3f);
  emission_color = Color::FromArgb (vec3f[0]*255, vec3f[1]*255, vec3f[2]*255);

  Entity::OnModify ();
}


}
