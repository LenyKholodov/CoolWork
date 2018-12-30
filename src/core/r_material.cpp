#include <material.h>
#include <algorithm>
#include <string>

using namespace std;

struct Material::Impl {                   //описание реализации материала
  Material* owner;                        //владелец
  string    name;                         //префикс имени  
  vec3f     color [MATERIAL_COLORS_NUM];  //цвета материала
  float     shininess;                    //"металичность"
  float     transparency;                 //прозрачность
  BlendMode blend_mode;                   //режим блендинга
  size_t    flags;                        //флаги
  bool      reg_vars;                     //флаг, сигнализирующий о зарегистрированности переменных
  
  static int mtl_counter;
  
  const char* property (const char*);  

  void OnSetTransparency (Var&);
  void OnGetTransparency (Var&);
  void OnSetShininess (Var&);
  void OnGetShininess (Var&);  
  void OnSetTwoSided  (Var&);
  void OnGetTwoSided  (Var&);
  void OnSetWireframe (Var&);
  void OnGetWireframe (Var&);  
  void OnSetBlendMode (Var&);
  void OnGetBlendMode (Var&);    
};

int Material::Impl::mtl_counter = 0;

Material::Material ()
{
  impl = new Impl;
   
  impl->owner      = this;
  impl->reg_vars   = false;  
  impl->shininess  = 0.0f;
  impl->flags      = 0;
  impl->blend_mode = MATERIAL_BLEND_SOLID;
  
  impl->color [MATERIAL_EMISSION_COLOR] = 0.0f;
  impl->color [MATERIAL_AMBIENT_COLOR]  = 0.0f;  
  impl->color [MATERIAL_DIFFUSE_COLOR]  = 0.8f;
  impl->color [MATERIAL_SPECULAR_COLOR] = 0.0f;
  
  rename (NULL); //auto name  
}

Material::~Material ()
{
  UnregisterVars ();
  delete impl;
}

void Material::enable (size_t flags)
{
  if (!(flags ^ impl->flags))
    return;  
    
  impl->flags |= flags;  
}

void Material::disable (size_t flags)
{
  if (!(~flags ^ impl->flags))
    return;  
    
  impl->flags &= ~flags;
}

bool Material::IsEnabled (size_t flags)
{
  return (impl->flags & flags) != 0;
}

void Material::SetColor (MaterialColor type,const vec3f& color)
{
  if (type < 0 || type >= MATERIAL_COLORS_NUM)
    return;
    
  impl->color [type] = color;  
}

const vec3f& Material::GetColor (MaterialColor type)
{
  static vec3f err_color = 0.0f;
  return type < 0 || type >= MATERIAL_COLORS_NUM ? err_color : impl->color [type];
}

void Material::SetShininess (float shininess)
{
  impl->shininess = fabs (shininess);
}

float Material::GetShininess ()
{
  return impl->shininess;
}

void Material::SetBlend (BlendMode mode,float transparency)
{
  SetBlendMode    (mode);
  SetTransparency (transparency);
}

void Material::SetBlendMode (BlendMode mode)
{
  switch (mode)
  {
    case MATERIAL_BLEND_ADD:
    case MATERIAL_BLEND_FILTER:
    case MATERIAL_BLEND_SOLID:
      break;
    default:
      return;
  }
  
  impl->blend_mode = mode;
}

BlendMode Material::GetBlendMode ()
{
  return impl->blend_mode;
}

void Material::SetTransparency (float transparency)
{
  impl->transparency = max (min (transparency,1.0f),0.0f);
}

float Material::GetTransparency ()
{
  return impl->transparency;
}

const char* Material::Impl::property (const char* property_name)
{
  static char buf [256];  
  
  _snprintf (buf,sizeof (buf),"%s.%s",name.c_str (),property_name);
  
  return buf;
}

void Material::Impl::OnSetTransparency (Var& var)
{
  owner->SetTransparency (var.getf ());
}

void Material::Impl::OnGetTransparency (Var& var)
{
  var.set (owner->GetTransparency ());
}

void Material::Impl::OnSetShininess (Var& var)
{
  shininess = fabs (var.getf ());
}

void Material::Impl::OnGetShininess (Var& var)
{
  var.set (shininess);
}

void Material::Impl::OnSetWireframe (Var& var)
{
  if (var.geti ()) owner->enable  (MATERIAL_WIREFRAME);
  else             owner->disable (MATERIAL_WIREFRAME);
}

void Material::Impl::OnGetWireframe (Var& var)
{
  var.set (owner->IsEnabled (MATERIAL_WIREFRAME)!=0);  
}

void Material::Impl::OnSetTwoSided (Var& var)
{
  if (var.geti ()) owner->enable  (MATERIAL_TWO_SIDED);
  else             owner->disable (MATERIAL_TWO_SIDED);
}

void Material::Impl::OnGetTwoSided (Var& var)
{
  var.set (owner->IsEnabled (MATERIAL_TWO_SIDED)!=0);  
}

void Material::Impl::OnSetBlendMode (Var& var)
{
  const char* mode = var.gets ();
  
  if      (!strcmp (mode,"filter")) blend_mode = MATERIAL_BLEND_FILTER;
  else if (!strcmp (mode,"add"))    blend_mode = MATERIAL_BLEND_ADD;
  else if (!strcmp (mode,"solid"))  blend_mode = MATERIAL_BLEND_SOLID;
}

void Material::Impl::OnGetBlendMode (Var& var)
{
  switch (blend_mode)
  {
    case MATERIAL_BLEND_ADD:     var.set ("add"); break;
    case MATERIAL_BLEND_FILTER:  var.set ("filter"); break;
    case MATERIAL_BLEND_SOLID:   var.set ("solid"); break;
  }
}

void Material::rename (const char* name)
{
  char tmp [128];
  
  if (!name)  _snprintf (tmp,sizeof (tmp),"Material%d",Impl::mtl_counter++);
  else        strncpy (tmp,name,sizeof (tmp));
  
  UnregisterVars ();
  impl->name = tmp;      
  RegisterVars ();
}

const char* Material::name ()
{
  return impl->name.c_str ();
}
                          
void Material::RegisterVars ()
{
  if (impl->reg_vars || !*name ())
    return;
      
  Etc& etc = Command::instance ().etc ();
  
  etc.bind (impl->property ("ambient_color"),impl->color [MATERIAL_AMBIENT_COLOR]);
  etc.bind (impl->property ("diffuse_color"),impl->color [MATERIAL_DIFFUSE_COLOR]);
  etc.bind (impl->property ("specular_color"),impl->color [MATERIAL_SPECULAR_COLOR]);
  etc.bind (impl->property ("emission_color"),impl->color [MATERIAL_EMISSION_COLOR]);
  
  etc.bind_handler (impl->property ("transparency"),*impl,&Impl::OnSetTransparency,&Impl::OnGetTransparency);
  etc.bind_handler (impl->property ("shininess"),*impl,&Impl::OnSetShininess,&Impl::OnGetShininess);
  etc.bind_handler (impl->property ("wireframe"),*impl,&Impl::OnSetWireframe,&Impl::OnGetWireframe);
  etc.bind_handler (impl->property ("two_sided"),*impl,&Impl::OnSetTwoSided,&Impl::OnGetTwoSided);  
  etc.bind_handler (impl->property ("blend"),*impl,&Impl::OnSetBlendMode,&Impl::OnGetBlendMode);

  impl->reg_vars = true;
}

void Material::UnregisterVars ()
{
  if (!impl->reg_vars)
    return;
    
  Etc& etc = Command::instance ().etc ();
    
  etc.remove (impl->property ("ambient_color"));
  etc.remove (impl->property ("diffuse_color"));
  etc.remove (impl->property ("specular_color"));
  etc.remove (impl->property ("emission_color"));
  etc.remove (impl->property ("shininess"));
  etc.remove (impl->property ("transparency"));
  etc.remove (impl->property ("wireframe"));
  etc.remove (impl->property ("two_sided"));
  etc.remove (impl->property ("blend_mode"));  
    
  impl->reg_vars = false;  
}
