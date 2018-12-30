#include <scene.h>


struct RenderData { //параметры хелпера, необходимые для отрисовки
  float angle;                    //угол отрисовки сектора
};

struct HelperEntity::Impl { //описание реализации хелпера
  HelperId id;              //идентификатор хелпера
  RenderData render_data;
};

HelperEntity::HelperEntity ()
{
  impl = new Impl;
  
  impl->id = HELPER_POINT;
  impl->render_data.angle = 0;
}

HelperEntity::~HelperEntity ()
{
  delete impl;
}

void HelperEntity::SetPoint (const vec3f& pos)
{
  impl->id = HELPER_POINT;

  lookat (pos,pos+vec3f (0.0f,0.0f,1.0f));
}

void HelperEntity::SetArrow (const vec3f& from,const vec3f& to)
{
  impl->id = HELPER_ARROW;
  
  float len = length (to-from);
  
  lookat (from,to);
  scale  (vec3f (len*0.1f,1.0f,-len));
}

static mat4f getrotate (const vec3f& from,const vec3f& to)
{
  vec3f n    = normalize (cross (from,to));
  float _cos = normalize (from)&normalize (to);
  
  return rotate (acos (_cos),n);  
}

void HelperEntity::SetRect (const vec3f& pos,const vec3f& dir,const vec3f& size)
{
  impl->id = HELPER_RECT;
  
  lookat (pos,pos+dir);
  scale  (size);
}

void HelperEntity::SetAxes (const vec3f& pos)
{
  impl->id = HELPER_AXES;
  
  lookat (pos,pos+vec3f (0,0,1));
}

void HelperEntity::SetArc (const vec3f& pos,const vec3f dir, const vec3f up)
{
  impl->id = HELPER_ARC;
  
  lookat (pos, dir, up);
}

void HelperEntity::SetSector (const vec3f& pos, const vec3f dir, const vec3f up, const float angle)
{
  impl->id = HELPER_SECTOR;
  
  lookat (pos, dir, up);
}

void HelperEntity::SetCircle (const vec3f& pos,const vec3f dir, const vec3f up)
{
  impl->id = HELPER_CIRCLE;
  
  lookat (pos, dir, up);
}

void HelperEntity::SetLine (const vec3f& from,const vec3f& to)
{
  impl->id = HELPER_LINE;
  
  lookat (from,to);
  scale  (vec3f (1.0f,1.0f,-length (to-from)));
}
    
HelperId HelperEntity::GetHelperId ()
{
  return impl->id;
}

float HelperEntity::GetHelperAngle ()
{
  return impl->render_data.angle;
}
    
void HelperEntity::SetHelperAngle (const float& angle)
{
  impl->render_data.angle = angle;
}

void HelperEntity::visit (EntityVisitor* visitor)
{
  visitor->visit (this);
}
