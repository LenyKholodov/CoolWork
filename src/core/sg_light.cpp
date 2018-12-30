#include <scene.h>
#include <stdio.h>
#include <algorithm>

using namespace std;

const float MIN_INNER_RADIUS     = 0.01f;
const float DEFAULT_INNER_RADIUS = 100;

struct Light::Impl {                 //описание реализации камеры
  Light*    owner;                   //владелец
  LightType type;                    //тип источника освещения
  vec3f     color [LIGHT_COLOR_NUM]; //цвета источника  
  int       state;                   //состояние источника
  float     spot_angle;              //угол для spot light
  float     spot_exponent;           //экспонента освещения
  float     inner_radius;            //внутренний радиус освещения
  
  static size_t light_counter;
  
  void OnSetLightType    (Var&);
  void OnGetLightType    (Var&);  
  void OnSetSpotAngle    (Var&);
  void OnGetSpotAngle    (Var&);
  void OnSetSpotExponent (Var&);
  void OnGetSpotExponent (Var&);
  void OnSetInnerRadius  (Var&);
  void OnGetInnerRadius  (Var&);
};

size_t Light::Impl::light_counter = 0;

Light::Light ()
{  
  impl = new Impl;

  impl->owner         = this;  
  impl->type          = LIGHT_POINT;
  impl->state         = true;    
  impl->spot_angle    = 180;
  impl->spot_exponent = 0;
  impl->inner_radius  = DEFAULT_INNER_RADIUS;
  
  impl->color [LIGHT_AMBIENT_COLOR]  = 0.0f;
  impl->color [LIGHT_DIFFUSE_COLOR]  = 0.8f;
  impl->color [LIGHT_SPECULAR_COLOR] = 0.0f;  
  
  char buf [128];
  
  _snprintf (buf,sizeof (buf),"Light%d",Impl::light_counter++);
  
  rename (buf);  
}

Light::~Light ()
{
  delete impl;
}

void Light::on ()
{
  impl->state = true;
  OnModify ();  
}

void Light::off ()
{
  impl->state = false;
  OnModify ();  
}

bool Light::state ()
{
  return impl->state != 0;
}
  
void Light::SetLightType (LightType type)
{
  impl->type = type;
  OnModify ();  
}

LightType Light::GetLightType ()
{
  return impl->type;
}

void Light::SetPointLight (const vec3f& pos)
{
  SetLightType (LIGHT_POINT);  
  lookat       (pos,0.0f);
  SetSpotAngle (180);
}

void Light::SetSpotLight (const vec3f& pos,const vec3f& dir,float angle)
{
  SetLightType (LIGHT_SPOT);
  lookat       (pos,pos+dir);  
  SetSpotAngle (angle);
}

void Light::SetDirectLight (const vec3f& dir)
{
  SetLightType (LIGHT_DIRECT);
  lookat       (0,dir);
  SetSpotAngle (180);
}

void Light::SetLightColor (LightColor type,const vec3f& color)
{
  if (type < LIGHT_AMBIENT_COLOR || type > LIGHT_SPECULAR_COLOR)
    return;
    
  impl->color [type] = color;  
  
  OnModify ();  
}

const vec3f& Light::GetLightColor (LightColor type)
{
  static vec3f error_color = 0.0f;
  
  return type >= LIGHT_AMBIENT_COLOR && type <= LIGHT_SPECULAR_COLOR ? impl->color [type] : error_color;
}

void Light::SetSpotAngle (float angle)
{
  impl->spot_angle = max (min (angle,90.0f),0.0f);
}

float Light::GetSpotAngle ()
{
  return impl->spot_angle;
}

void Light::SetSpotExponent (float exponent)
{
  impl->spot_exponent = max (min (exponent,128.0f),0.0f);
}

float Light::GetSpotExponent ()
{
  return impl->spot_exponent;
}

void Light::SetInnerRadius (float radius)
{
  impl->inner_radius = max (radius,MIN_INNER_RADIUS);
}

float Light::GetInnerRadius ()
{
  return impl->inner_radius;
}

void Light::visit (EntityVisitor* visitor)
{
  visitor->visit (this);
}

void Light::Impl::OnSetLightType (Var& var)
{
  const char* stype = var.gets ();
  
  if      (!strcmp (stype,"point"))  type = LIGHT_POINT;
  else if (!strcmp (stype,"spot"))   type = LIGHT_SPOT;
  else if (!strcmp (stype,"direct")) type = LIGHT_DIRECT;
  else                               return;  
}

void Light::Impl::OnGetLightType (Var& var)
{
  switch (type)
  {
    default:    
    case LIGHT_POINT:  var.set ("point");  break;
    case LIGHT_SPOT:   var.set ("spot");   break;
    case LIGHT_DIRECT: var.set ("direct"); break;
  }
}

void Light::Impl::OnSetSpotAngle (Var& var)
{
  owner->SetSpotAngle (var.getf ());
}

void Light::Impl::OnGetSpotAngle (Var& var)
{
  var.set (owner->GetSpotAngle ());
}

void Light::Impl::OnSetSpotExponent (Var& var)
{
  owner->SetSpotExponent (var.getf ());
}

void Light::Impl::OnGetSpotExponent (Var& var)
{
  var.set (owner->GetSpotExponent ());
}

void Light::Impl::OnSetInnerRadius (Var& var)
{
  owner->SetInnerRadius (var.getf ());
}

void Light::Impl::OnGetInnerRadius (Var& var)
{
  var.set (owner->GetInnerRadius ());
}

void Light::RegisterVars ()
{
  Entity::RegisterVars ();

  Etc& etc = Command::instance ().etc ();  
  
  etc.set (property ("type"),"light");

  etc.bind_handler (property ("light_type"),*impl,&Light::Impl::OnSetLightType,&Light::Impl::OnGetLightType);  
  etc.bind_handler (property ("spot_angle"),*impl,&Light::Impl::OnSetSpotAngle,&Light::Impl::OnGetSpotAngle);
  etc.bind_handler (property ("spot_exponent"),*impl,&Light::Impl::OnSetSpotExponent,&Light::Impl::OnGetSpotExponent);
  etc.bind_handler (property ("radius"),*impl,&Light::Impl::OnSetInnerRadius,&Light::Impl::OnGetInnerRadius);
  
  etc.bind (property ("ambient_color"),impl->color [LIGHT_AMBIENT_COLOR]);
  etc.bind (property ("diffuse_color"),impl->color [LIGHT_DIFFUSE_COLOR]);  
  etc.bind (property ("specular_color"),impl->color [LIGHT_SPECULAR_COLOR]);
  etc.bind (property ("enable"),impl->state);
}

void Light::UnregisterVars ()
{  
  Etc& etc = Command::instance ().etc ();  
    
  etc.remove (property ("ambient_color"));
  etc.remove (property ("diffuse_color"));
  etc.remove (property ("specular_color"));  
  etc.remove (property ("light_type"));
  etc.remove (property ("spot_angle"));
  etc.remove (property ("spot_exponent"));
  etc.remove (property ("radius"));  
  etc.remove (property ("enable"));
  
  Entity::UnregisterVars ();  
}
