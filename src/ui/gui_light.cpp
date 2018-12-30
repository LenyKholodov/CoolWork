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

Light::Light (CoreEntityHandle _handle) :  Entity (_handle)
{
  OnModify ();
}

Light::~Light ()
{
}

/*
===============================================================================

  Свойства Light

===============================================================================
*/

#pragma region Properties

String^ Light::LightType::get (void)
{
  return light_type;
}

void Light::LightType::set (String^ value)
{
  light_type = value;
  CoreSetEntityParams (handle, CORE_LIGHT_TYPE, (AutoStr (value)).c_str ());
}

bool Light::Enabled::get ()
{
  return enabled;
}

void Light::Enabled::set (bool value)
{
  enabled = value;
  CoreSetEntityParami (handle, CORE_LIGHT_ENABLE, enabled);
}

Color Light::Ambient::get ()
{
  return ambient_color;
}

void Light::Ambient::set (Color value)
{
  ambient_color = value;
  float rgb [3] = { value.R / 255.0f, value.G / 255.0f, value.B / 255.0f};
  CoreSetEntityParam3fv (handle, CORE_LIGHT_AMBIENT_COLOR, rgb);
}

Color Light::Diffuse::get ()
{
  return diffuse_color;
}

void Light::Diffuse::set (Color value)
{
  diffuse_color = value;
  float rgb [3] = { value.R / 255.0f, value.G / 255.0f, value.B / 255.0f};
  CoreSetEntityParam3fv (handle, CORE_LIGHT_DIFFUSE_COLOR, rgb);
}

Color Light::Specular::get ()
{
  return specular_color;
}

void Light::Specular::set (Color value)
{
  specular_color = value;
  float rgb [3] = { value.R / 255.0f, value.G / 255.0f, value.B / 255.0f};
  CoreSetEntityParam3fv (handle, CORE_LIGHT_SPECULAR_COLOR, rgb);
}

Single Light::Radius::get ()
{
  return radius;
}

void Light::Radius::set (Single value)
{
  radius = value;
  CoreSetEntityParamf (handle, CORE_LIGHT_RADIUS, value);  
}

#pragma endregion

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void Light::OnModify ()
{
  enabled = (bool) CoreGetEntityParami (handle, CORE_LIGHT_ENABLE);

  light_type = gcnew String ( CoreGetEntityParams (handle, CORE_LIGHT_TYPE) );  

  radius = CoreGetEntityParamf (handle, CORE_LIGHT_RADIUS);

  float vec3f [3];

  CoreGetEntityParam3fv (handle, CORE_LIGHT_AMBIENT_COLOR, vec3f);
  ambient_color = Color::FromArgb (vec3f[0]*255, vec3f[1]*255, vec3f[2]*255);

  CoreGetEntityParam3fv (handle, CORE_LIGHT_DIFFUSE_COLOR, vec3f);
  diffuse_color = Color::FromArgb (vec3f[0]*255, vec3f[1]*255, vec3f[2]*255);

  CoreGetEntityParam3fv (handle, CORE_LIGHT_SPECULAR_COLOR, vec3f);
  specular_color = Color::FromArgb (vec3f[0]*255, vec3f[1]*255, vec3f[2]*255);

  Entity::OnModify ();
}

/*
===============================================================================
===============================================================================
*/

/*
===============================================================================

  Конструктор / деструктор SpotLight

===============================================================================
*/

SpotLight::SpotLight (CoreEntityHandle _handle) :  Light (_handle)
{
  OnModify ();
}

SpotLight::~SpotLight ()
{
}

/*
===============================================================================

  Свойства SpotLight

===============================================================================
*/

#pragma region Properties

Single SpotLight::Angle::get ()
{
  return angle;
}

void SpotLight::Angle::set (Single value)
{
  angle = Math::Min (Math::Max (value, 0.0f), 180.0f);
  CoreSetEntityParamf (handle, CORE_LIGHT_SPOT_ANGLE, angle);
}

Single SpotLight::Exponent::get ()
{
  return exponent;
}

void SpotLight::Exponent::set (Single value)
{
  exponent = Math::Min (Math::Max (value, 0.0f), 128.0f);

  CoreSetEntityParamf (handle, CORE_LIGHT_SPOT_EXPONENT, exponent);  
}

#pragma endregion

/*
===============================================================================

  Изменение параметров

===============================================================================
*/

void SpotLight::OnModify ()
{
  angle = CoreGetEntityParamf (handle, CORE_LIGHT_SPOT_ANGLE);

  exponent = CoreGetEntityParamf (handle, CORE_LIGHT_SPOT_EXPONENT);

  Light::OnModify ();
}

}