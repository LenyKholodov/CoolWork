#include "propertyform.h"
#include "gui_scene.h"
#include "gui_camera.h"
#include "gui_vector.h"
#include "utils.h"

#include <core.h>

using namespace System;

namespace My3D {

/*
===============================================================================

  Конструктор / деструктор BaseCamera

===============================================================================
*/

BaseCamera::BaseCamera (CoreEntityHandle _handle) : Entity (_handle)
{
  OnModify ();
}

BaseCamera::~BaseCamera ()
{
}

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void BaseCamera::OnModify ()
{
  proj_type = gcnew String ( CoreGetEntityParams (handle, CORE_CAMERA_PROJECTION) );

  left      = CoreGetEntityParamf (handle, CORE_CAMERA_LEFT);
  right     = CoreGetEntityParamf (handle, CORE_CAMERA_RIGHT);
  top       = CoreGetEntityParamf (handle, CORE_CAMERA_TOP);
  bottom    = CoreGetEntityParamf (handle, CORE_CAMERA_BOTTOM);
  near_clip = CoreGetEntityParamf (handle, CORE_CAMERA_ZNEAR);
  far_clip  = CoreGetEntityParamf (handle, CORE_CAMERA_ZFAR);

  Entity::OnModify ();
}

/*
===============================================================================

  Параметры базовой камеры

===============================================================================
*/

String^ BaseCamera::ProjectionType::get (void)
{
  return proj_type;
}

void BaseCamera::ProjectionType::set (String^ value)
{
  proj_type = value;
  CoreSetEntityParams (handle, CORE_CAMERA_PROJECTION, (AutoStr (value)).c_str ());
}

System::Single BaseCamera::Left::get ()
{
  return left;
}

void BaseCamera::Left::set (System::Single value)
{
  left = value;
  CoreSetEntityParamf (handle, CORE_CAMERA_LEFT, value);
}

System::Single BaseCamera::Right::get ()
{
  return right;
}

void BaseCamera::Right::set (System::Single value)
{
  right = value;
  CoreSetEntityParamf (handle, CORE_CAMERA_RIGHT, value);
}

System::Single BaseCamera::Top::get ()
{
  return top;
}

void BaseCamera::Top::set (System::Single value)
{
  top = value;
  CoreSetEntityParamf (handle, CORE_CAMERA_TOP, value);
}

System::Single BaseCamera::Bottom::get ()
{
  return bottom;
}

void BaseCamera::Bottom::set (System::Single value)
{
  bottom = value;
  CoreSetEntityParamf (handle, CORE_CAMERA_BOTTOM, value);
}

System::Single BaseCamera::NearClip::get ()
{
  return near_clip;
}

void BaseCamera::NearClip::set (System::Single value)
{
  near_clip = value;
  CoreSetEntityParamf (handle, CORE_CAMERA_ZNEAR, value);
}

System::Single BaseCamera::FarClip::get ()
{
  return far_clip;
}

void BaseCamera::FarClip::set (System::Single value)
{
  far_clip = value;
  CoreSetEntityParamf (handle, CORE_CAMERA_ZFAR, value);
}

/*
===============================================================================
===============================================================================
*/

/*
===============================================================================

  Конструктор / деструктор IsometryCamera

===============================================================================
*/

IsometryCamera::IsometryCamera (CoreEntityHandle _handle) : BaseCamera (_handle)
{
  OnModify ();
}

IsometryCamera::~IsometryCamera ()
{
}

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void IsometryCamera::OnModify ()
{
  alpha = CoreGetEntityParamf (handle, CORE_CAMERA_ALPHA);

  BaseCamera::OnModify ();
}

/*
===============================================================================

  Параметры изометрии

===============================================================================
*/

System::Single IsometryCamera::Alpha::get ()
{
  return alpha;
}

void IsometryCamera::Alpha::set (System::Single value)
{
  alpha = value;
  CoreSetEntityParamf (handle, CORE_CAMERA_ALPHA, value);
}

/*
===============================================================================
===============================================================================
*/

/*
===============================================================================

  Конструктор / деструктор DimetryCamera

===============================================================================
*/

DimetryCamera::DimetryCamera (CoreEntityHandle _handle) : IsometryCamera (_handle)
{
  OnModify ();
}

DimetryCamera::~DimetryCamera ()
{
}

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void DimetryCamera::OnModify ()
{
  beta = CoreGetEntityParamf (handle, CORE_CAMERA_BETA);

  IsometryCamera::OnModify ();
}

/*
===============================================================================

  Параметры диметрии

===============================================================================
*/

System::Single DimetryCamera::Beta::get ()
{
  return beta;
}

void DimetryCamera::Beta::set (System::Single value)
{
  beta = value;
  CoreSetEntityParamf (handle, CORE_CAMERA_BETA, value);
}

/*
===============================================================================
===============================================================================
*/

/*
===============================================================================

  Конструктор / деструктор TrimetryCamera

===============================================================================
*/

TrimetryCamera::TrimetryCamera (CoreEntityHandle _handle) : DimetryCamera (_handle)
{
  OnModify ();
}

TrimetryCamera::~TrimetryCamera ()
{
}

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void TrimetryCamera::OnModify ()
{
  gamma = 360.0f - alpha - beta;

  DimetryCamera::OnModify ();
}

/*
===============================================================================

  Параметры триметрии

===============================================================================
*/

System::Single TrimetryCamera::Gamma::get ()
{
  return gamma;
}

void TrimetryCamera::Gamma::set (System::Single value)
{
}

/*
===============================================================================
===============================================================================
*/

/*
===============================================================================

  Конструктор / деструктор Perspective1Camera

===============================================================================
*/

Perspective1Camera::Perspective1Camera (CoreEntityHandle _handle)
 : first (gcnew Vec3f ()), BaseCamera (_handle)
{
  OnModify ();
}

Perspective1Camera::~Perspective1Camera ()
{
  delete first;
}

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void Perspective1Camera::OnModify ()
{
  float vec3f [3];

  CoreGetEntityParam3fv (handle, CORE_CAMERA_POINT1, vec3f);
  first->assign (vec3f);

  BaseCamera::OnModify ();
}

/*
===============================================================================

  Параметры одноточечной перспективной проекции

===============================================================================
*/

Vec3f^ Perspective1Camera::First::get ()
{
  return first;
}

void Perspective1Camera::First::set (Vec3f^ value)
{
  first->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_CAMERA_POINT1, vec3f);
}

/*
===============================================================================
===============================================================================
*/

/*
===============================================================================

  Конструктор / деструктор Perspective2Camera

===============================================================================
*/

Perspective2Camera::Perspective2Camera (CoreEntityHandle _handle)
 : second (gcnew Vec3f ()), Perspective1Camera (_handle)
{
  OnModify ();
}

Perspective2Camera::~Perspective2Camera ()
{
  delete second;
}

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void Perspective2Camera::OnModify ()
{
  float vec3f [3];

  CoreGetEntityParam3fv (handle, CORE_CAMERA_POINT2, vec3f);
  second->assign (vec3f);

  Perspective1Camera::OnModify ();
}

/*
===============================================================================

  Параметры двухточечной перспективной проекции

===============================================================================
*/

Vec3f^ Perspective2Camera::Second::get ()
{
  return second;
}

void Perspective2Camera::Second::set (Vec3f^ value)
{
  second->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_CAMERA_POINT2, vec3f);
}

/*
===============================================================================
===============================================================================
*/

/*
===============================================================================

  Конструктор / деструктор Perspective3Camera

===============================================================================
*/

Perspective3Camera::Perspective3Camera (CoreEntityHandle _handle)
 : third (gcnew Vec3f ()), Perspective2Camera (_handle)
{
  OnModify ();
}

Perspective3Camera::~Perspective3Camera ()
{
  delete third;
}

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void Perspective3Camera::OnModify ()
{
  float vec3f [3];

  CoreGetEntityParam3fv (handle, CORE_CAMERA_POINT3, vec3f);
  third->assign (vec3f);

  Perspective2Camera::OnModify ();
}

/*
===============================================================================

  Параметры двухточечной перспективной проекции

===============================================================================
*/

Vec3f^ Perspective3Camera::Third::get ()
{
  return third;
}

void Perspective3Camera::Third::set (Vec3f^ value)
{
  third->assign ( value );
  float vec3f [3] = { value->X, value->Y, value->Z };
  CoreSetEntityParam3fv (handle, CORE_CAMERA_POINT3, vec3f);
}

}
