#include <scene.h>
#include <material.h>
#include <mesh.h>
#include <hash_map>
#include <string>

using namespace std;
using namespace stdext;

const float DEFAULT_MODEL_LIN_SIZE = 1.0f;
const float MIN_MODEL_LIN_SIZE     = 0.01f;
const float MAX_MODEL_LIN_SIZE     = 1000.0f;
const float DEFAULT_SLICES         = 30;
const float MIN_SLICES             = 3;
const float MAX_SLICES             = 300;
const float DEFAULT_SHEAR_ANGLE    = 0.0f;
const float MIN_SHEAR_ANGLE        = -89.999999f;
const float MAX_SHEAR_ANGLE        = 89.999999f;

struct Param
{
  float value,min,max;
  
  Param () : value (0), min (0), max (0) {}
  Param (float _value,float _min,float _max) : value (_value), min (_min), max (_max) {}
};

typedef hash_map<string,Param> ParamMap;

struct ParamModel::Impl { //описание реализации класса параметрической модели
  ParamModel*    owner;   //владелец
  ParamModelType type;    //тип параметрической модели
  ParamMap       map;     //карта параметров
  Surface*       surface; //поверхность
  bool           dirty;   //флаг "грязности"
  Material       mtl;     //материал  
  
  void OnSetWidth  (Var&);
  void OnGetWidth  (Var&);
  void OnSetHeight (Var&);
  void OnGetHeight (Var&);  
  void OnSetDepth  (Var&);
  void OnGetDepth  (Var&);
  void OnSetRadius (Var&);
  void OnGetRadius (Var&);
  void OnSetSlices (Var&);
  void OnGetSlices (Var&);
  void OnSetParallels (Var&);
  void OnGetParallels (Var&);  
  void OnSetMeridians (Var&);
  void OnGetMeridians (Var&);
  void OnSetRadius2 (Var&);
  void OnGetRadius2 (Var&);
  void OnSetShearX (Var&);
  void OnGetShearX (Var&);
  void OnSetShearZ (Var&);
  void OnGetShearZ (Var&);
};

ParamModel::ParamModel ()
{
  impl = new Impl;
  
  impl->surface = NULL;
  impl->dirty   = true;
  impl->owner   = this;
  impl->type    = (ParamModelType)-1;  
}

ParamModel::~ParamModel ()
{
  if (impl->surface)
    impl->surface->release ();

  delete impl;
}

void ParamModel::SetModelType (ParamModelType type)
{
  static Param default_lin_param = Param (DEFAULT_MODEL_LIN_SIZE,MIN_MODEL_LIN_SIZE,MAX_MODEL_LIN_SIZE);
  static Param default_slice_param = Param (DEFAULT_SLICES,MIN_SLICES,MAX_SLICES);  
  static Param default_shear_param = Param (DEFAULT_SHEAR_ANGLE,MIN_SHEAR_ANGLE,MAX_SHEAR_ANGLE);  

  impl->map.clear ();
  impl->type = MODEL_UNKNOWN;    
  
  if (impl->surface)
  {
    impl->surface->release ();  
    impl->surface = NULL;
  }

  switch (type)
  {
    case MODEL_BOX:
      impl->map ["width"]  = default_lin_param;
      impl->map ["height"] = default_lin_param;
      impl->map ["depth"]  = default_lin_param;      
      break;
    case MODEL_HALF_SPHERE:
    case MODEL_SPHERE:
      impl->map ["radius"]    = default_lin_param;
      impl->map ["parallels"] = default_slice_param;
      impl->map ["meridians"] = default_slice_param;
      break;
    case MODEL_HALF_CONE:          
    case MODEL_CONE:
      impl->map ["radius"]  = default_lin_param; 
      impl->map ["radius2"] = Param (MIN_MODEL_LIN_SIZE,0.0f,MAX_MODEL_LIN_SIZE);
      impl->map ["height"]  = default_lin_param;
      impl->map ["slices"]  = default_slice_param;
      break;
    case MODEL_HALF_CYLINDER:
    case MODEL_CYLINDER:    
      impl->map ["radius"] = default_lin_param;
      impl->map ["height"] = default_lin_param;
      impl->map ["slices"] = default_slice_param;
      break;
    case MODEL_TETRAHEDRON:
    case MODEL_HEXAHEDRON:
    case MODEL_OCTAHEDRON:
    case MODEL_DODECAHEDRON:
    case MODEL_ICOSAHEDRON:
      impl->map ["radius"] = default_lin_param;
      break;
    case MODEL_HALF_TORUS:
    case MODEL_TORUS:
      impl->map ["radius"]  = default_lin_param; 
      impl->map ["radius2"] = default_lin_param;
      impl->map ["parallels"]  = default_slice_param;
      impl->map ["meridians"]  = default_slice_param;
      break;
    default:
      return;
  }
  
  impl->map ["shear_x"] = default_shear_param;
  impl->map ["shear_z"] = default_shear_param;  

  impl->type  = type;  
  impl->dirty = true;
  
  Entity* p = parent ();
  
  unbind         ();  
  UnregisterVars ();      
  RegisterVars   ();    
  bind           (p);
  
//  OnModify ();
}

ParamModelType ParamModel::GetModelType ()
{
  return impl->type;
}

void ParamModel::SetParam (const char* name,float value)
{
  if (!name)
    return;
    
  ParamMap::iterator i = impl->map.find (name);
  
  if (i == impl->map.end ())
    return;
    
  Param& param = i->second;
  
  param.value = min (max (value,param.min),param.max);
  
  impl->dirty = true;
  
  OnModify ();  
}

float ParamModel::GetParam (const char* name)
{
  if (!name)
    return 0;
    
  ParamMap::iterator i = impl->map.find (name);
  
  if (i == impl->map.end ())
    return 0;  
    
  return i->second.value;
}

static mat4f GetShearTM(float angle_x,float angle_z) {
  angle_x=max(-89.999999f,min(89.999999f,angle_x));
  angle_z=max(-89.999999f,min(89.999999f,angle_z));
  
  mat4f sh=1;
  sh[0][1]=tan(deg2rad(angle_x));
  sh[2][1]=tan(deg2rad(angle_z));
  return sh;
}

Surface* ParamModel::GetSurface ()
{
  if (!impl->dirty)
    return impl->surface;

  impl->dirty = false;

  if (impl->surface)
  {
    impl->surface->release ();
    impl->surface = NULL;
  }
    
  switch (impl->type)
  {
    case MODEL_BOX:
      impl->surface = Primitives::CreateBox (GetParam ("width"),GetParam ("height"),GetParam ("depth"));      
      break;
    case MODEL_SPHERE:
      impl->surface = Primitives::CreateSphere (GetParam ("radius"),(size_t)GetParam ("parallels"),(size_t)GetParam ("meridians"));
      break;
    case MODEL_HALF_SPHERE:
      impl->surface = Primitives::CreateHalfSphere (GetParam ("radius"),(size_t)GetParam ("parallels"),(size_t)GetParam ("meridians"));
      break;      
    case MODEL_CONE:           
      impl->surface = Primitives::CreateClipCone (GetParam ("height"),GetParam ("radius"),GetParam ("radius2"),(size_t)GetParam ("slices"));
      break;
    case MODEL_HALF_CONE:           
      impl->surface = Primitives::CreateHalfClipCone (GetParam ("height"),GetParam ("radius"),GetParam ("radius2"),(size_t)GetParam ("slices"));
      break;
    case MODEL_CYLINDER:
      impl->surface = Primitives::CreateCylinder (GetParam ("radius"),GetParam ("height"),(size_t)GetParam ("slices"));
      break;
    case MODEL_HALF_CYLINDER:
      impl->surface = Primitives::CreateHalfCylinder (GetParam ("radius"),GetParam ("height"),(size_t)GetParam ("slices"));
      break;      
    case MODEL_TETRAHEDRON:
      impl->surface = Primitives::CreateTetrahedron (GetParam ("radius"));
      break;      
    case MODEL_HEXAHEDRON:
      impl->surface = Primitives::CreateHexahedron (GetParam ("radius"));
      break;
    case MODEL_OCTAHEDRON:
      impl->surface = Primitives::CreateOctahedron (GetParam ("radius"));
      break;
    case MODEL_DODECAHEDRON:
      impl->surface = Primitives::CreateDodecahedron (GetParam ("radius"));
      break;
    case MODEL_ICOSAHEDRON:
      impl->surface = Primitives::CreateIcosahedron (GetParam ("radius"));
      break;
    case MODEL_HALF_TORUS:
      impl->surface = Primitives::CreateHalfTorus (GetParam ("radius"),GetParam ("radius2"),(size_t)GetParam ("parallels"),(size_t)GetParam ("meridians"));
      break;
    case MODEL_TORUS:
      impl->surface = Primitives::CreateTorus (GetParam ("radius"),GetParam ("radius2"),(size_t)GetParam ("parallels"),(size_t)GetParam ("meridians"));
      break;
  }
  
  if (impl->surface)
    impl->surface->transform(GetShearTM(GetParam ("shear_x"),GetParam ("shear_z")));
  
  return impl->surface;
}

Material& ParamModel::GetMaterial ()
{
  return impl->mtl;
}

AABB ParamModel::GetLocalAABB ()
{
  static AABB err_aabb = {0.0f,0.0f};
  
  if (!impl->surface)
    return err_aabb;
  
  AABB bbox = impl->surface->GetAABB ();
  
  bbox.min = GetLocalTM () * bbox.min;
  bbox.max = GetLocalTM () * bbox.max;
  
  return bbox;
}

AABB ParamModel::GetWorldAABB ()
{
  static AABB err_aabb = {0.0f,0.0f};  
  
  if (!impl->surface)
    return err_aabb;  
  
  AABB bbox = impl->surface->GetAABB ();
    
  bbox.min = GetWorldTM () * bbox.min;
  bbox.max = GetWorldTM () * bbox.max;  
  
  return bbox;
}

void ParamModel::visit (EntityVisitor* visitor)
{
  visitor->visit (this);
}

void ParamModel::Impl::OnSetWidth       (Var& var) { owner->SetParam ("width",var.getf ()); }
void ParamModel::Impl::OnGetWidth       (Var& var) { var.set (owner->GetParam ("width")); }
void ParamModel::Impl::OnSetHeight      (Var& var) { owner->SetParam ("height",var.getf ()); }
void ParamModel::Impl::OnGetHeight      (Var& var) { var.set (owner->GetParam ("height")); }
void ParamModel::Impl::OnSetDepth       (Var& var) { owner->SetParam ("depth",var.getf ()); }
void ParamModel::Impl::OnGetDepth       (Var& var) { var.set (owner->GetParam ("depth")); }
void ParamModel::Impl::OnSetRadius2     (Var& var) { owner->SetParam ("radius2",var.getf ()); }
void ParamModel::Impl::OnGetRadius2     (Var& var) { var.set (owner->GetParam ("radius2")); }
void ParamModel::Impl::OnSetRadius      (Var& var) { owner->SetParam ("radius",var.getf ()); }
void ParamModel::Impl::OnGetRadius      (Var& var) { var.set (owner->GetParam ("radius")); }
void ParamModel::Impl::OnSetSlices      (Var& var) { owner->SetParam ("slices",var.getf ()); }
void ParamModel::Impl::OnGetSlices      (Var& var) { var.set (owner->GetParam ("slices")); }
void ParamModel::Impl::OnSetParallels   (Var& var) { owner->SetParam ("parallels",var.getf ()); }
void ParamModel::Impl::OnGetParallels   (Var& var) { var.set (owner->GetParam ("parallels")); }
void ParamModel::Impl::OnSetMeridians   (Var& var) { owner->SetParam ("meridians",var.getf ()); }
void ParamModel::Impl::OnGetMeridians   (Var& var) { var.set (owner->GetParam ("meridians")); }
void ParamModel::Impl::OnSetShearX      (Var& var) { owner->SetParam ("shear_x",var.getf ()); }
void ParamModel::Impl::OnGetShearX      (Var& var) { var.set (owner->GetParam ("shear_x")); }
//void ParamModel::Impl::OnSetShearY      (Var& var) { owner->SetParam ("shear_y",var.getf ()); }
//void ParamModel::Impl::OnGetShearY      (Var& var) { var.set (owner->GetParam ("shear_y")); }
void ParamModel::Impl::OnSetShearZ      (Var& var) { owner->SetParam ("shear_z",var.getf ()); }
void ParamModel::Impl::OnGetShearZ      (Var& var) { var.set (owner->GetParam ("shear_z")); }

void ParamModel::RegisterVars ()
{
  Entity::RegisterVars ();
  
  Etc& etc = Command::instance ().etc ();

  switch (impl->type)
  {
    case MODEL_BOX:           etc.set (property ("type"),"box");          break;
    case MODEL_SPHERE:        etc.set (property ("type"),"sphere");       break;
    case MODEL_HALF_SPHERE:   etc.set (property ("type"),"halfsphere");   break;
    case MODEL_CYLINDER:      etc.set (property ("type"),"cylinder");     break;
    case MODEL_HALF_CYLINDER: etc.set (property ("type"),"halfcylinder"); break;
    case MODEL_CONE:          etc.set (property ("type"),"cone");         break;
    case MODEL_HALF_CONE:     etc.set (property ("type"),"halfcone");     break;            
    case MODEL_TETRAHEDRON:   etc.set (property ("type"),"tetrahedron");  break;
    case MODEL_HEXAHEDRON:    etc.set (property ("type"),"hexahedron");   break;
    case MODEL_OCTAHEDRON:    etc.set (property ("type"),"octahedron");   break;
    case MODEL_DODECAHEDRON:  etc.set (property ("type"),"dodecahedron"); break;
    case MODEL_ICOSAHEDRON:   etc.set (property ("type"),"icosahedron");  break;
    case MODEL_TORUS:         etc.set (property ("type"),"torus");        break;
    default:                  etc.set (property ("type"),"unknown");      break;
  }
  
  etc.bind_handler (property ("width"),*impl,&Impl::OnSetWidth,&Impl::OnGetWidth);
  etc.bind_handler (property ("height"),*impl,&Impl::OnSetHeight,&Impl::OnGetHeight);
  etc.bind_handler (property ("depth"),*impl,&Impl::OnSetDepth,&Impl::OnGetDepth);
  etc.bind_handler (property ("radius"),*impl,&Impl::OnSetRadius,&Impl::OnGetRadius);
  etc.bind_handler (property ("radius2"),*impl,&Impl::OnSetRadius2,&Impl::OnGetRadius2);
  etc.bind_handler (property ("slices"),*impl,&Impl::OnSetSlices,&Impl::OnGetSlices);
  etc.bind_handler (property ("parallels"),*impl,&Impl::OnSetParallels,&Impl::OnGetParallels);
  etc.bind_handler (property ("meridians"),*impl,&Impl::OnSetMeridians,&Impl::OnGetMeridians);
  etc.bind_handler (property ("shear_x"),*impl,&Impl::OnSetShearX,&Impl::OnGetShearX);
//  etc.bind_handler (property ("shear_y"),*impl,&Impl::OnSetShearY,&Impl::OnGetShearY);
  etc.bind_handler (property ("shear_z"),*impl,&Impl::OnSetShearZ,&Impl::OnGetShearZ);
  
  char buf [128];
  
  _snprintf (buf,sizeof (buf),"%s.mtl",name ());
  
  impl->mtl.rename (buf);
}

void ParamModel::UnregisterVars ()
{
  Etc& etc = Command::instance ().etc ();

  etc.remove (property ("width"));
  etc.remove (property ("height"));
  etc.remove (property ("depth"));
  etc.remove (property ("radius"));
  etc.remove (property ("radius2"));
  etc.remove (property ("slices"));
  etc.remove (property ("parallels"));
  etc.remove (property ("meridians"));
  etc.remove (property ("shear_x"));
//  etc.remove (property ("shear_y"));
  etc.remove (property ("shear_z"));
  
  impl->mtl.rename ("");
  
  Entity::UnregisterVars ();
}
