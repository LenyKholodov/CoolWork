#include "gui_scene.h"
#include "gui_vector.h"
#include "editorform.h"
#include "propertyform.h"

using namespace System;
using namespace System::Drawing;

namespace My3D {

/*
===============================================================================

  Конструктор / деструктор

===============================================================================
*/

Sphere::Sphere (CoreEntityHandle _handle) :  ModelEntity (_handle)
{
  OnModify ();
}

Sphere::~Sphere ()
{
}

/*
===============================================================================

  Свойства Sphere

===============================================================================
*/

#pragma region Properties

System::Single Sphere::Radius::get ()
{
  return radius;
}

void Sphere::Radius::set (System::Single value)
{
  radius = value;
  CoreSetEntityParamf (handle, CORE_MODEL_RADIUS, value);
}

System::Int32 Sphere::Parallels::get ()
{
  return parallels;
}

void Sphere::Parallels::set (System::Int32 value)
{
  parallels = value;
  CoreSetEntityParamf (handle, CORE_MODEL_PARALLELS, value);
}

System::Int32 Sphere::Meridians::get ()
{
  return meridians;
}

void Sphere::Meridians::set (System::Int32 value)
{
  meridians = value;
  CoreSetEntityParamf (handle, CORE_MODEL_MERIDIANS, value);
}

#pragma endregion

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void Sphere::OnModify ()
{
  radius    = CoreGetEntityParamf (handle, CORE_MODEL_RADIUS);
  meridians = CoreGetEntityParami (handle, CORE_MODEL_MERIDIANS);
  parallels = CoreGetEntityParami (handle, CORE_MODEL_PARALLELS);

  ModelEntity::OnModify ();
}

}
