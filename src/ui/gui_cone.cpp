#include "gui_scene.h"
#include "gui_vector.h"
#include "propertyform.h"

using namespace System;
using namespace System::Drawing;

namespace My3D {

/*
===============================================================================

  Конструктор / деструктор

===============================================================================
*/

Cone::Cone (CoreEntityHandle _handle) : ModelEntity (_handle)
{
  OnModify ();
}

Cone::~Cone ()
{
}

/*
===============================================================================

  Свойства Cone

===============================================================================
*/

#pragma region Properties

System::Single Cone::Height::get ()
{
  return height;
}

void Cone::Height::set (System::Single value)
{
  height = value;
  CoreSetEntityParamf (handle, CORE_MODEL_HEIGHT, value);
}

System::Single Cone::Radius::get ()
{
  return radius;
}

void Cone::Radius::set (System::Single value)
{
  radius = value;
  CoreSetEntityParamf (handle, CORE_MODEL_RADIUS, value);
}

System::Single Cone::SecondRadius::get ()
{
  return second_radius;
}

void Cone::SecondRadius::set (System::Single value)
{
  second_radius = value;
  CoreSetEntityParamf (handle, CORE_MODEL_SECOND_RADIUS, value);
}

System::Int32 Cone::Slices::get ()
{
  return slices;
}

void Cone::Slices::set (System::Int32 value)
{
  slices = value;
  CoreSetEntityParami (handle, CORE_MODEL_SLICES, value);
}

#pragma endregion

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void Cone::OnModify ()
{
  height = CoreGetEntityParamf (handle, CORE_MODEL_HEIGHT);
  radius = CoreGetEntityParamf (handle, CORE_MODEL_RADIUS);
  second_radius = CoreGetEntityParamf (handle, CORE_MODEL_SECOND_RADIUS);
  slices       = CoreGetEntityParami (handle, CORE_MODEL_SLICES);

  ModelEntity::OnModify ();
}

}