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

Cylinder::Cylinder (CoreEntityHandle _handle) :  ModelEntity (_handle)
{
  OnModify ();
}

Cylinder::~Cylinder ()
{
}

/*
===============================================================================

  Свойства Cylinder

===============================================================================
*/

#pragma region Properties

System::Single Cylinder::Height::get ()
{
  return height;
}

void Cylinder::Height::set (System::Single value)
{
  height = value;
  CoreSetEntityParamf (handle, CORE_MODEL_HEIGHT, value);
}

System::Single Cylinder::Radius::get ()
{
  return radius;
}

void Cylinder::Radius::set (System::Single value)
{
  radius = value;
  CoreSetEntityParamf (handle, CORE_MODEL_RADIUS, value);
}

System::Int32 Cylinder::Slices::get ()
{
  return slices;
}

void Cylinder::Slices::set (System::Int32 value)
{
  slices = value;
  CoreSetEntityParami (handle, CORE_MODEL_SLICES, value);
}

#pragma endregion

/*
===============================================================================

  Создание / Удаление / Изменение параметров

===============================================================================
*/

void Cylinder::OnModify ()
{
  height = CoreGetEntityParamf (handle, CORE_MODEL_HEIGHT);
  radius = CoreGetEntityParamf (handle, CORE_MODEL_RADIUS);
  slices = CoreGetEntityParami (handle, CORE_MODEL_SLICES);

  ModelEntity::OnModify ();
}

}