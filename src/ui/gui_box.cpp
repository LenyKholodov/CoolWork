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

Box::Box (CoreEntityHandle _handle) : ModelEntity (_handle)
{
  OnModify ();
}

Box::~Box ()
{
}

/*
===============================================================================

  Свойства Box

===============================================================================
*/

#pragma region Properties

System::Single Box::Width::get ()
{
  return width;
}

void Box::Width::set (System::Single value)
{
  width = value;
  CoreSetEntityParamf (handle, CORE_MODEL_WIDTH, value);
}

System::Single Box::Height::get ()
{
  return height;
}

void Box::Height::set (System::Single value)
{
  height = value;
  CoreSetEntityParamf (handle, CORE_MODEL_HEIGHT, value);
}

System::Single Box::Depth::get ()
{
  return depth;
}

void Box::Depth::set (System::Single value)
{
  depth = value;
  CoreSetEntityParamf (handle, CORE_MODEL_DEPTH, value);
}

#pragma endregion

/*
===============================================================================

  Создание / Удаление / Изменение параметров

===============================================================================
*/

void Box::OnModify ()
{
  height = CoreGetEntityParamf (handle, CORE_MODEL_HEIGHT);
  width  = CoreGetEntityParamf (handle, CORE_MODEL_WIDTH);
  depth  = CoreGetEntityParamf (handle, CORE_MODEL_DEPTH);

  ModelEntity::OnModify ();
}

}

