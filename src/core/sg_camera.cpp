#include <scene.h>
#include <stdio.h>

const float DEFAULT_ANGLE = 30;

struct Camera::Impl { //описание данных реализации камеры
  Camera*    owner;   //владелец
  ProjParams params;  //параметры проекции
  mat4f      proj;    //матрица проекции
  bool       dynamic_projection; //динамическая проекция
  
  void reset_params (float left=0.0f,float right=0.0f,float top=0.0f,float bottom=0.0f,float znear=0.0f,float zfar=0.0f);  
  
  void OnSetProjType (Var&);
  void OnGetProjType (Var&);  
  void OnSetLeft   (Var&);
  void OnGetLeft   (Var&);
  void OnSetRight  (Var&);
  void OnGetRight  (Var&);  
  void OnSetBottom (Var&);
  void OnGetBottom (Var&);    
  void OnSetTop    (Var&);
  void OnGetTop    (Var&);
  void OnSetZNear  (Var&);
  void OnGetZNear  (Var&);  
  void OnSetZFar   (Var&);
  void OnGetZFar   (Var&);
  void OnSetAlpha  (Var&);
  void OnGetAlpha  (Var&);  
  void OnSetBeta   (Var&);
  void OnGetBeta   (Var&);
  void OnSetDistance (Var&);
  void OnGetDistance (Var&);  
  void OnSetPoint1 (Var&);
  void OnGetPoint1 (Var&);    
  void OnSetPoint2 (Var&);
  void OnGetPoint2 (Var&);      
  void OnSetPoint3 (Var&);
  void OnGetPoint3 (Var&);      

  void SetPerspectiveSpecial ();  
  void SetPerspective ();
  void SetOrtho       ();
  void SetIsometry    ();
  void SetDimetry     ();
  void SetTrimetry    ();
  void SetCabinet     ();
  void SetFree        ();
  
  void RegisterParams ();
  void UnregisterParams ();
  
  void change_projection ();
  
  static size_t camera_counter;
};

size_t Camera::Impl::camera_counter = 0;

void Camera::Impl::reset_params (float left,float right,float top,float bottom,float znear,float zfar)
{
  params.type     = CAMERA_PROJ_ORTHO;
  params.left     = left;
  params.right    = right;
  params.top      = top;
  params.bottom   = bottom;
  params.znear    = znear;
  params.zfar     = zfar;
  params.alpha    = 0.0f;
  params.beta     = 0.0f;
  params.gamma    = 0.0f;
  params.distance = 0.0f;
  
  for (int i=0;i<3;i++)
    params.point [i] = 0.0f;
}

static mat4f GetNoramlizePM (const ProjParams& p)
{
  mat4f m;

  m [0] = vec4f (2.0f / (p.right-p.left),0,0,-(p.right+p.left)/(p.right-p.left));
  m [1] = vec4f (0,2.0f /(p.bottom-p.top),0,-(p.top+p.bottom)/(p.bottom-p.top));
  m [2] = vec4f (0,0,-2.0f / (p.zfar-p.znear),-(p.zfar+p.znear)/(p.zfar-p.znear));
  m [3] = vec4f (0,0,0,1);
    
  return m;
}

template <class T> inline T sqr (T x)
{
  return x*x;
}

static mat4f PerspectiveProjX (const mat4f& viewTM,const vec3f& _v)
{
  mat4f proj = 1;

  vec3f v  = invert (viewTM) * _v;
  proj [3] = vec4f (-1.0f/v.x,0,0,1);  
  
  return translatef (0,v.y,v.z) * proj * translatef (0,-v.y,-v.z);
}

static mat4f PerspectiveProjY (const mat4f& viewTM,const vec3f& _v)
{
  mat4f proj = 1;

  vec3f v  = invert (viewTM) * _v;
  proj [3] = vec4f (0,-1.0f/v.y,0,1);
  
  return translatef (v.x,0,v.z) * proj * translatef (-v.x,0,-v.z);
}

static mat4f PerspectiveProjZ (const mat4f& viewTM,const vec3f& _v)
{
  mat4f proj = 1;

  vec3f v  = invert (viewTM) * _v;
  proj [3] = vec4f (0,0,-1.0f/v.z,1);
  
  return translatef (v.x,v.y,0) * proj * translatef (-v.x,-v.y,0);
}

void Camera::Impl::SetPerspectiveSpecial ()
{  
  proj = GetNoramlizePM (params);
  
  const float EPS = 0.01f;
  
  if (fabs (params.point [0].x) > EPS) proj *= PerspectiveProjX (owner->GetWorldTM (),params.point [0]);
  if (fabs (params.point [1].y) > EPS) proj *= PerspectiveProjY (owner->GetWorldTM (),params.point [1]);
  if (fabs (params.point [2].z) > EPS) proj *= PerspectiveProjZ (owner->GetWorldTM (),params.point [2]);
  
  dynamic_projection = true;
}

void Camera::Impl::SetPerspective ()
{                            
  proj [0] = vec4f (2.0f * params.znear/(params.right-params.left),0,(params.right+params.left)/(params.right-params.left),0);
  proj [1] = vec4f (0,2.0f * params.znear/(params.bottom-params.top),(params.top+params.bottom)/(params.bottom-params.top),0);
  proj [2] = vec4f (0,0,-(params.zfar+params.znear)/(params.zfar-params.znear),-2.0f *params.znear*params.zfar/(params.zfar-params.znear));
  proj [3] = vec4f (0,0,-1,0);  
}

void Camera::Impl::SetOrtho ()
{
  proj = GetNoramlizePM (params);
}

void Camera::Impl::SetIsometry ()
{  
  mat4f m;
  
  m [0] = vec4f (0.707107f,0.408248f,-0.577353f,0);
  m [1] = vec4f (0,0.816597f,0.577345f,0);
  m [2] = vec4f (-0.707107f,0.408248f,-0.577353f);
  m [3] = vec4f (0,0,0,1);
  
  mat4f m1;
  
  m1 [0] = vec4f (0,0,1,0);
  m1 [1] = vec4f (0,1,0,0);  
  m1 [2] = vec4f (1,0,0,0);  
  m1 [3] = vec4f (0,0,0,1);  

  proj = GetNoramlizePM (params) * transpose (m) * m1;
}

void Camera::Impl::SetDimetry ()
{
  params.alpha= 120;

  float a1 = deg2rad (params.alpha),
        a2 = deg2rad ((360-params.alpha)/2.0f);
        
  mat4f p;        
 
//  By Shicin and Boreskov
//  p [0] = vec4f (cos (a1), sin (a1) * sin (a2), 0, 0);
//  p [1] = vec4f (0,    cos (a1), 0, 0);
//  p [2] = vec4f (sin (a1), -sin (a1)*cos (a1), 1, 0);
//  p [3] = vec4f (0,0,0,1);     
  p [0] = vec4f (cos (a1), sin (a1) * sin (a2), 0, 0);
  p [1] = vec4f (0,    cos (a1), 0, 0);
  p [2] = vec4f (sin (a1), -sin (a1)*cos (a1), 1, 0);
  p [3] = vec4f (0,0,0,1);     
  
  mat4f m;
  
  m [0] = vec4f (-1,0,0,0);
  m [1] = vec4f (0,-1,0,0);
  m [2] = vec4f (0,0,-1,0);  
  m [3] = vec4f (0,0,0,1);    
  
  proj = GetNoramlizePM (params) * transpose (p) * m;
          
/*  proj [0] = vec4f (cos (a1), sin (a1) * sin (a2), -sin (a1)*cos (a2), 0);
  proj [1] = vec4f (cos (a2), sin (a2), 0, 0);
  proj [2] = vec4f (sin (a1), -sin (a2)*cos (a1), cos (a1)*cos (a2), 0);
  proj [3] = vec4f (0,0,0,1);
  proj     = GetNoramlizePM (params) * proj;*/
}

void Camera::Impl::SetTrimetry ()
{
  float a1 = deg2rad (params.alpha),
        a2 = deg2rad (params.beta),  
        a3 = acos (sqrt (1.0f-sqr (cos (a1))-sqr (cos(a2))));
 
//  p [0] = vec4f (0, sin(a3) * cos (a2) + cos(a3) * sin (a1) * sin (a2), sin(a3) * sin (a2) - cos(a3) * sin (a1) * cos (a2), 0);
//  p [1] = vec4f (0, cos(a3) * cos (a2) - sin(a3) * sin (a1) * sin (a2), cos(a3) * sin (a2) + sin(a3) * sin (a1) * cos (a2), 0);
//  p [2] = vec4f (0, -cos (a1) * sin (a2), cos (a1) * cos (a2), 0);
//  p [3] = vec4f (0,0,0,1);  
  
  proj [0] = vec4f (cos (a3)*cos (a1),-sin (a3)*cos (a1),sin (a1),0);
  proj [1] = vec4f (sin (a3)*cos (a2)+cos (a3)*sin (a1)*sin (a2),cos (a3)*cos (a2)-sin (a3)*sin (a1)*sin (a2),-sin (a2)*cos (a1),0);
  proj [2] = vec4f (0,0,0,0);
  proj [3] = vec4f (0,0,0,1);
  proj     = GetNoramlizePM (params) * proj;
}

void Camera::Impl::SetCabinet ()
{  
  proj [0] = vec4f (-1,0,sqrt (2.0f)*0.25f,0);
  proj [1] = vec4f (0,1,sqrt (2.0f)*0.25f,0);
  proj [2] = vec4f (0,0,-1,0);
  proj [3] = vec4f (0,0,0,1);  
  
  proj     = GetNoramlizePM (params) * proj;
}

void Camera::Impl::SetFree ()
{ 
  proj [0] = vec4f (-1,0,sqrt (2.0f)*0.5f,0);
  proj [1] = vec4f (0,1,sqrt (2.0f)*0.5f,0);
  proj [2] = vec4f (0,0,1,0);
  proj [3] = vec4f (0,0,0,1);
  proj     = GetNoramlizePM (params) * proj;
}

void Camera::Impl::change_projection ()
{
  dynamic_projection = false;

  switch (params.type)
  {
    case CAMERA_PROJ_PERSPECTIVE_SPECIAL: SetPerspectiveSpecial (); break;    
    case CAMERA_PROJ_PERSPECTIVE: SetPerspective (); break;
    case CAMERA_PROJ_ORTHO:       SetOrtho (); break;
    case CAMERA_PROJ_DIMETRY:     SetDimetry (); break;  
    case CAMERA_PROJ_ISOMETRY:    SetIsometry (); break;
    case CAMERA_PROJ_TRIMETRY:    SetTrimetry (); break;
    case CAMERA_PROJ_CABINET:     SetCabinet ();  break;
    case CAMERA_PROJ_FREE:        SetFree (); break;
  }
}

Camera::Camera ()
{
  char buf [128];
  
  _snprintf (buf,sizeof (buf),"Camera%d",Impl::camera_counter++);
  
  rename (buf);

  impl        = new Impl;
  impl->owner = this;
  impl->proj  = 1;
  impl->dynamic_projection = false;
  
  impl->reset_params ();
}

Camera::~Camera ()
{
  delete impl;
}

ProjType Camera::GetProjType ()
{
  return impl->params.type;
}

const ProjParams& Camera::GetProjParams ()
{
  return impl->params;
}

void Camera::SetPerspective (float fov,float znear,float zfar)
{   
  float width2  = tan (deg2rad (fov/2.0f)) * znear, 
        height2 = width2;

  impl->reset_params (-width2,width2,-height2,height2,znear,zfar);

  impl->params.type  = CAMERA_PROJ_PERSPECTIVE;
  
  impl->change_projection ();

  UnregisterVars ();
  RegisterVars ();
  OnModify ();
} 

void Camera::SetPerspective (float fov,float znear,float zfar,const vec3f& a,const vec3f& b,const vec3f& c)
{   
  float width2  = tan (deg2rad (fov/2.0f)) * znear, 
        height2 = width2;

  impl->reset_params (-width2,width2,-height2,height2,znear,zfar);

  impl->params.type      = CAMERA_PROJ_PERSPECTIVE_SPECIAL;
  impl->params.point [0] = a;
  impl->params.point [1] = b;
  impl->params.point [2] = c;
  
  impl->change_projection ();

  UnregisterVars ();
  RegisterVars ();
  OnModify ();
} 

void Camera::SetOrtho (float left,float right,float top,float bottom,float znear,float zfar)
{
  impl->reset_params (left,right,top,bottom,znear,zfar);  
  
  impl->params.type = CAMERA_PROJ_ORTHO;

  impl->change_projection ();
  
  UnregisterVars ();
  RegisterVars ();  
  OnModify ();  
}

void Camera::SetIsometry (float left,float right,float bottom,float top,float znear,float zfar)
{
  impl->reset_params (left,right,top,bottom,znear,zfar);  

  impl->params.type = CAMERA_PROJ_ISOMETRY;
  
  impl->change_projection ();    

  UnregisterVars ();
  RegisterVars ();    
  OnModify();
}

void Camera::SetDimetry (float left,float right,float bottom,float top,float znear,float zfar,float a1,float a2)
{
  impl->reset_params (left,right,top,bottom,znear,zfar);  

  impl->params.type  = CAMERA_PROJ_DIMETRY;
  impl->params.alpha = a1;
  impl->params.beta  = a2;
  
  impl->change_projection ();

  UnregisterVars ();
  RegisterVars ();  
  OnModify ();
}

void Camera::SetTrimetry (float left,float right,float bottom,float top,float znear,float zfar,float a1,float a2)
{
  impl->reset_params (left,right,top,bottom,znear,zfar);  

  impl->params.type  = CAMERA_PROJ_TRIMETRY;
  impl->params.alpha = a1;
  impl->params.beta  = a2;
  
  impl->change_projection ();

  UnregisterVars ();
  RegisterVars ();  
  OnModify ();
}

void Camera::SetCabinet (float left,float right,float bottom,float top,float znear,float zfar)
{
  impl->reset_params (left,right,top,bottom,znear,zfar);  

  impl->params.type = CAMERA_PROJ_CABINET;
  
  impl->change_projection ();

  UnregisterVars ();
  RegisterVars ();  
  OnModify ();
}

void Camera::SetFree (float left,float right,float bottom,float top,float znear,float zfar)
{
  impl->reset_params (left,right,top,bottom,znear,zfar);  

  impl->params.type = CAMERA_PROJ_FREE;
  
  impl->change_projection ();

  UnregisterVars ();
  RegisterVars ();  
  OnModify ();
}

const mat4f& Camera::GetProjTM ()
{
  if (impl->dynamic_projection)
    impl->change_projection ();

  return impl->proj;
}

vec3f Camera::ProjectWorldPoint (const vec3f& pos)
{
  vec4f t = GetProjTM() * invert(GetWorldTM()) * vec4f (pos,1);
  return t / t.w;
}

void Camera::forward (float step)
{
  translate (vec3f (0,0,step));
}

void Camera::back (float step)
{
  forward (-step);
}

void Camera::left (float step)
{
  translate (vec3f (-step,0,0));  
}

void Camera::right (float step)
{
  left (-step);
}

void Camera::pitch (float angle)
{
  rotate (fromEulerAnglef (angle,0,0));
}

void Camera::yaw (float angle)
{
  rotate (fromEulerAnglef (0,angle,0));
}

void Camera::roll (float angle)
{
  rotate (fromEulerAnglef (0,0,angle));
}

void Camera::visit (EntityVisitor* visitor)
{
  visitor->visit (this);
}

void Camera::Impl::OnSetProjType (Var& var)
{
  params.alpha    = params.beta = params.gamma = 0.0f;
  params.distance = 0.0f;
  params.point [0] = params.point [1] = params.point [2] = 0.0f;  

  if      (!strcmp (var.gets (),"ortho"))       params.type = CAMERA_PROJ_ORTHO;
  else if (!strcmp (var.gets (),"isometry"))    params.type = CAMERA_PROJ_ISOMETRY;
  else if (!strcmp (var.gets (),"dimetry"))     
  {
    params.type  = CAMERA_PROJ_DIMETRY;
    params.alpha = params.beta = DEFAULT_ANGLE;
  }
  else if (!strcmp (var.gets (),"trimetry")) 
  {
    params.type  = CAMERA_PROJ_TRIMETRY;    
    params.alpha = params.beta = DEFAULT_ANGLE;
  }
  else if (!strcmp (var.gets (),"cabinet"))     params.type = CAMERA_PROJ_CABINET;
  else if (!strcmp (var.gets (),"free"))        params.type = CAMERA_PROJ_FREE; 
  else if (!strcmp (var.gets (),"perspective")) params.type = CAMERA_PROJ_PERSPECTIVE;
  else if (!strcmp (var.gets (),"perspective_special")) params.type = CAMERA_PROJ_PERSPECTIVE_SPECIAL;
  
  UnregisterParams ();
  RegisterParams ();
  
  change_projection ();
}

void Camera::Impl::OnGetProjType (Var& var)
{
  const char* type = "user";  

  switch (params.type)
  {
    case CAMERA_PROJ_ORTHO:               type = "ortho"; break;
    case CAMERA_PROJ_ISOMETRY:            type = "isometry"; break;
    case CAMERA_PROJ_DIMETRY:             type = "dimetry"; break;
    case CAMERA_PROJ_TRIMETRY:            type = "trimetry"; break;
    case CAMERA_PROJ_CABINET:             type = "cabinet"; break;
    case CAMERA_PROJ_FREE:                type = "free"; break;
    case CAMERA_PROJ_PERSPECTIVE:         type = "perspective"; break;
    case CAMERA_PROJ_PERSPECTIVE_SPECIAL: type = "perspective_special"; break;
  }
  
  var.set (type);
}

void Camera::Impl::OnSetLeft (Var& var)     { params.left = var.getf ();  change_projection (); }
void Camera::Impl::OnGetLeft (Var& var)     { var.set (params.left); }
void Camera::Impl::OnSetRight (Var& var)    { params.right = var.getf ();  change_projection (); }
void Camera::Impl::OnGetRight (Var& var)    { var.set (params.right); }
void Camera::Impl::OnSetTop (Var& var)      { params.top = var.getf ();  change_projection (); }
void Camera::Impl::OnGetTop (Var& var)      { var.set (params.top); }
void Camera::Impl::OnSetBottom (Var& var)   { params.bottom = var.getf ();  change_projection (); }
void Camera::Impl::OnGetBottom (Var& var)   { var.set (params.bottom); }
void Camera::Impl::OnSetZNear (Var& var)    { params.znear = var.getf ();  change_projection ();}
void Camera::Impl::OnGetZNear (Var& var)    { var.set (params.znear); }
void Camera::Impl::OnSetZFar (Var& var)     { params.zfar = var.getf ();  change_projection ();}
void Camera::Impl::OnGetZFar (Var& var)     { var.set (params.zfar);}
void Camera::Impl::OnSetAlpha (Var& var)    { params.alpha = var.getf ();  change_projection ();}
void Camera::Impl::OnGetAlpha (Var& var)    { var.set (params.alpha);}
void Camera::Impl::OnSetBeta (Var& var)     { params.beta = var.getf ();  change_projection ();}
void Camera::Impl::OnGetBeta (Var& var)     { var.set (params.beta);}
void Camera::Impl::OnSetDistance (Var& var) { params.distance = var.get3f ();  change_projection ();}
void Camera::Impl::OnGetDistance (Var& var) { var.set (params.distance);}
void Camera::Impl::OnSetPoint1 (Var& var)   { params.point [0] = var.get3f ();  change_projection ();}
void Camera::Impl::OnGetPoint1 (Var& var)   { var.set (params.point [0]);}
void Camera::Impl::OnSetPoint2 (Var& var)   { params.point [1] = var.get3f ();  change_projection ();}
void Camera::Impl::OnGetPoint2 (Var& var)   { var.set (params.point [1]);}
void Camera::Impl::OnSetPoint3 (Var& var)   { params.point [2] = var.get3f ();  change_projection ();}
void Camera::Impl::OnGetPoint3 (Var& var)   { var.set (params.point [2]);}

void Camera::Impl::RegisterParams ()
{
  Etc& etc = Command::instance ().etc ();

  switch (params.type)
  {
    case CAMERA_PROJ_PERSPECTIVE:
      etc.bind_handler (owner->property ("distance"),*this,&Impl::OnSetDistance,&Impl::OnGetDistance);
      break;
    case CAMERA_PROJ_PERSPECTIVE_SPECIAL:
      etc.bind_handler (owner->property ("point1"),*this,&Impl::OnSetPoint1,&Impl::OnGetPoint1);
      etc.bind_handler (owner->property ("point2"),*this,&Impl::OnSetPoint2,&Impl::OnGetPoint2);
      etc.bind_handler (owner->property ("point3"),*this,&Impl::OnSetPoint3,&Impl::OnGetPoint3);
      break;
    case CAMERA_PROJ_DIMETRY:
    case CAMERA_PROJ_TRIMETRY:
      etc.bind_handler (owner->property ("alpha"),*this,&Impl::OnSetAlpha,&Impl::OnGetAlpha);
      etc.bind_handler (owner->property ("beta"),*this,&Impl::OnSetBeta,&Impl::OnGetBeta);
      break;
  }
}

void Camera::Impl::UnregisterParams ()
{
  Etc& etc = Command::instance ().etc ();

  etc.remove (owner->property ("alpha"));
  etc.remove (owner->property ("beta"));
  etc.remove (owner->property ("distance"));
  etc.remove (owner->property ("point1"));
  etc.remove (owner->property ("point2"));
  etc.remove (owner->property ("point3"));
}

void Camera::RegisterVars ()
{
  Entity::RegisterVars ();

  Etc& etc = Command::instance ().etc ();
  
  etc.set (property ("type"),"camera");
  
  etc.bind_handler (property ("projection"),*impl,&Impl::OnSetProjType,&Impl::OnGetProjType);  
  etc.bind_handler (property ("left"),*impl,&Impl::OnSetLeft,&Impl::OnGetLeft);
  etc.bind_handler (property ("right"),*impl,&Impl::OnSetRight,&Impl::OnGetRight);
  etc.bind_handler (property ("top"),*impl,&Impl::OnSetTop,&Impl::OnGetTop);
  etc.bind_handler (property ("bottom"),*impl,&Impl::OnSetBottom,&Impl::OnGetBottom);
  etc.bind_handler (property ("znear"),*impl,&Impl::OnSetZNear,&Impl::OnGetZNear);
  etc.bind_handler (property ("zfar"),*impl,&Impl::OnSetZFar,&Impl::OnGetZFar);
  
  impl->RegisterParams ();
}

void Camera::UnregisterVars ()
{
  Etc& etc = Command::instance ().etc ();

  etc.remove (property ("projection"));  
  etc.remove (property ("left"));
  etc.remove (property ("right"));
  etc.remove (property ("top"));
  etc.remove (property ("bottom"));
  etc.remove (property ("znear"));
  etc.remove (property ("zfar"));
  
  impl->UnregisterParams ();
  
  Entity::UnregisterVars ();
}
