#include "gui_scene.h"

using namespace System;
using namespace System::Drawing;

namespace My3D {

/*
===============================================================================

  Конструктор / деструктор

===============================================================================
*/

PlatonicBody::PlatonicBody (CoreEntityHandle _handle) : ModelEntity (_handle)
{
  OnModify ();
}

PlatonicBody::~PlatonicBody ()
{
}

/*
===============================================================================

  Свойства PlatonicBody

===============================================================================
*/

#pragma region Properties

System::Single PlatonicBody::Radius::get ()
{
  return radius;
}

void PlatonicBody::Radius::set (System::Single value)
{
  radius = value;
  CoreSetEntityParamf (handle, CORE_MODEL_RADIUS, value);
}

#pragma endregion

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void PlatonicBody::OnModify ()
{
  radius = CoreGetEntityParamf (handle, CORE_MODEL_RADIUS);

  ModelEntity::OnModify ();
}

}