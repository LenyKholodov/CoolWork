#include "gui_scene.h"
#include "propertyform.h"

using namespace System;
using namespace System::Drawing;

namespace My3D {

/*
===============================================================================

  Конструктор / деструктор

===============================================================================
*/

Torus::Torus (CoreEntityHandle _handle) : ModelEntity (_handle)
{
  OnModify ();
}

Torus::~Torus ()
{
}

/*
===============================================================================

  Свойства Torus

===============================================================================
*/

#pragma region Properties

System::Single Torus::InnerRadius::get ()
{
  return inner_radius;
}

void Torus::InnerRadius::set (System::Single value)
{
  inner_radius = value;
  CoreSetEntityParamf (handle, CORE_MODEL_RADIUS, value);
}

System::Single Torus::OuterRadius::get ()
{
  return outer_radius;
}

void Torus::OuterRadius::set (System::Single value)
{
  outer_radius = value;
  CoreSetEntityParamf (handle, CORE_MODEL_SECOND_RADIUS, value);
}

System::Int32 Torus::Parallels::get ()
{
  return parallels;
}

void Torus::Parallels::set (System::Int32 value)
{
  parallels = value;
  CoreSetEntityParamf (handle, CORE_MODEL_PARALLELS, value);
}

System::Int32 Torus::Meridians::get ()
{
  return meridians;
}

void Torus::Meridians::set (System::Int32 value)
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

void Torus::OnModify ()
{
  inner_radius = CoreGetEntityParamf (handle, CORE_MODEL_RADIUS);
  outer_radius = CoreGetEntityParamf (handle, CORE_MODEL_SECOND_RADIUS);
  meridians    = CoreGetEntityParami (handle, CORE_MODEL_MERIDIANS);
  parallels    = CoreGetEntityParami (handle, CORE_MODEL_PARALLELS);

  ModelEntity::OnModify ();
}

}