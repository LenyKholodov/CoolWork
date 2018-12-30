#include <string>
#include <vector>
#include <stdio.h>
#include "draw_stuff.h"

using namespace std;

const size_t MAX_GRID_SLICES     = 1000;
const size_t MIN_GRID_SLICES     = 2;
const size_t DEFAULT_GRID_SLICES = 18;
const float  DEFAULT_GRID_SIZE   = 18;
const float  EPS                 = 0.01f;

const size_t MAX_SELECTED_ENTITIES = 500;

struct Viewport::Impl {           //описание реализации области вывода
  string   name;                  //имя  
  Rect     bounds;                //границы области
  Camera*  camera;                //присоединённая камера
  size_t   flags;                 //флаги отрисовки
  Render*  render;                //ссылка на рендер, которому принадлежит область вывода
  mat4f    tm;                    //матрица предварительного преобразования
  vec2f    grid_size;             //размеры сетки
  vec3f    grid_pos;              //положение сетки в пространстве
  vec3f    grid_dir;              //направление сетки
  size_t   grid_slices;           //количество разбиений сетки
  Rect     select_rect;           //область выделения
  Scene    helper_scene;          //вспомогательная сцена

  int reverseY (int y) { return render->GetWindowRect ().height ()-y; }
  
  static size_t vp_counter;
};

size_t Viewport::Impl::vp_counter = 0;

Viewport::Viewport ()
{
  impl = new Impl;

  impl->name.resize (_scprintf ("Viewport%d",Impl::vp_counter));
  sprintf ((char*)impl->name.c_str (),"Viewport%d",Impl::vp_counter++);
  
  impl->camera = NULL;
  impl->flags  = VIEWPORT_VISIBLE;
  impl->render = NULL;
  impl->tm     = 1.0f;
  
  impl->grid_size   = DEFAULT_GRID_SIZE;
  impl->grid_slices = DEFAULT_GRID_SLICES;
  impl->grid_dir    = vec3f (0,1,0);
  impl->grid_pos    = 0;      
}

Viewport::~Viewport ()
{
  impl->camera->release ();
  
  delete impl;
}

void Viewport::SetRender (Render* render)
{
  impl->render = render;
}

Render* Viewport::GetRender ()
{
  return impl->render;
}

void Viewport::rename (const char* name)
{
  impl->name = name;
}

const char* Viewport::name ()
{
  return impl->name.c_str ();
}

void Viewport::activate ()
{
  impl->render->SetActiveViewport (this);
}

bool Viewport::IsActive ()
{
  return impl->render->GetActiveViewport () == this;
}

void Viewport::resize (int left,int top,int right,int bottom)
{
  float center_x = float ((right+left)/2),
        center_y = float ((bottom+top)/2);

  impl->bounds.set (left,top,right-left,bottom-top);
}

int Viewport::left ()
{
  return impl->bounds.left ();
}

int Viewport::top ()
{
  return impl->bounds.top ();
}

int Viewport::right ()
{
  return impl->bounds.right ();
}

int Viewport::bottom ()
{
  return impl->bounds.bottom ();
}

int Viewport::width ()
{
  return impl->bounds.width ();
}

int Viewport::height ()
{
  return impl->bounds.height ();
}

bool Viewport::IsInViewport (int x,int y)
{
  y = impl->reverseY (y);

  return impl->bounds.test (x,y);
}

void Viewport::SetView (Camera* camera)
{
  if (impl->camera)
    impl->camera->release ();
    
  impl->camera = camera;
    
  if (!camera)
  {
    rename ("Empty");
    return;
  }

  camera->addref ();
   
  rename (camera->name ());
}

void Viewport::SetView (Scene* scene,View view)
{
  Camera* camera = scene->CreateCamera ();
  
  camera->SetOrtho (-10,10,-10,10,-100,100); //???
  
  switch (view)
  {
    default:
    case VIEW_FRONT:  
      camera->lookat (vec3f (0,0,10),0.0f);
      camera->rename ("Front");  
      break;
    case VIEW_BACK:   
      camera->lookat (vec3f (0,0,-10),0.0f);  
      camera->rename ("Back");
      break;
    case VIEW_LEFT:
      camera->lookat (vec3f (-10,0,0),0.0f);
      camera->rename ("Left");
      break;
    case VIEW_RIGHT:
      camera->lookat (vec3f (10,0,0),0.0f);
      camera->rename ("Right");
      break;
    case VIEW_TOP:
      camera->lookat (vec3f (0,10,0),0.0f);
      camera->rename ("Top");
      break;
    case VIEW_BOTTOM: 
      camera->lookat (vec3f (0,-10,0),0.0f);
      camera->rename ("Bottom");
      break;
  }
  
  camera->enable (ENTITY_TEMP);
  
  SetGrid (0.0f,camera->GetWorldTM ()*vec3f (0,0,1));
  SetView (camera);
  
  camera->release ();
}

Camera* Viewport::GetCamera ()
{
  return impl->camera;
}

void Viewport::enable (size_t flags)
{
  impl->flags |= flags;  
}

void Viewport::disable (size_t flags)
{
  impl->flags &= ~flags;
}

bool Viewport::IsEnabled (size_t flags)
{
  return (impl->flags & flags) != 0;
}

void Viewport::SetGrid (const vec3f& pos,const vec3f& dir)
{
  impl->grid_pos = pos;
  impl->grid_dir = dir;
}

void Viewport::SetGridSlices (size_t slices)
{
  impl->grid_slices = min (max (slices,MIN_GRID_SLICES),MAX_GRID_SLICES);
}

void Viewport::SetGridSize (const vec2f& size)
{
  impl->grid_size = size;
}

const vec3f& Viewport::GetGridPos ()
{
  return impl->grid_pos;
}

const vec3f& Viewport::GetGridDir ()
{
  return impl->grid_dir;
}

const vec2f& Viewport::GetGridSize ()
{
  return impl->grid_size;
}

size_t Viewport::GetGridSlices ()
{
  return impl->grid_slices;
}
    
void Viewport::repaint ()
{  
  bool active = IsActive ();
  
  glViewport   (left (),top (),width (),height ());
  glPushAttrib (GL_ALL_ATTRIB_BITS);
    
  if (GetCamera ())
  {
    glDepthRange   (0.2,1.0);
    glMatrixMode   (GL_PROJECTION);
    glLoadIdentity ();
    glMatrixMode   (GL_MODELVIEW);
    glLoadIdentity ();
    rDrawGrid      (this);
    glMatrixMode   (GL_PROJECTION);
    glLoadIdentity ();
    glMatrixMode   (GL_MODELVIEW);
    glLoadIdentity ();
    rDrawScene     (this,GetCamera ()->scene (),GetCamera ());
    
    if (!impl->select_rect.empty () && IsActive ())  //draw select rect
    {       
      glDisable      (GL_LIGHTING);
      glDisable      (GL_DEPTH_TEST);
      glMatrixMode   (GL_PROJECTION);
      glLoadIdentity ();
      glOrtho        (left (),right (),top (),bottom (),-1,1);    
      glMatrixMode   (GL_MODELVIEW);
      glLoadIdentity ();
      glPushAttrib   (GL_LINE_BIT);
      glEnable       (GL_LINE_STIPPLE);
      glLineStipple  (1,0xAAAA);
      glColor3f      (0.2f,0.2f,0.2f);
      
      glBegin        (GL_LINE_LOOP);
        glVertex2i (impl->select_rect.left (),impl->reverseY (impl->select_rect.top ()));
        glVertex2i (impl->select_rect.right (),impl->reverseY (impl->select_rect.top ()));
        glVertex2i (impl->select_rect.right (),impl->reverseY (impl->select_rect.bottom ()));
        glVertex2i (impl->select_rect.left (),impl->reverseY (impl->select_rect.bottom ()));
      glEnd          ();
      glPopAttrib    ();
    }
  }
  
  glDepthRange   (0.0,0.2);
  glDisable      (GL_CULL_FACE);  
  glMatrixMode   (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho        (left (),right (),top (),bottom (),-1,1);
  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();
  glTranslatef   (0.0f,(float)GetRender ()->GetWindowRect ().height (),0.0f);
  glScalef       (1.0f,-1.0f,1.0f);      
  rDrawScene     (this,&impl->helper_scene,NULL);    
    
  glMatrixMode   (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho        (0,10,0,10,-1,1);
  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();
  
  if (IsActive ()) glColor3f (1,1,1);
  else             glColor3f (0,0,0);
  
  glBegin (GL_LINE_LOOP);  
    glVertex2f (0,0);
    glVertex2f (10,0);
    glVertex2f (10,10);
    glVertex2f (0,10);
  glEnd ();
  
  glRasterPos2f (0.2f,9.5f);
  rOutText      (GLUT_BITMAP_HELVETICA_12,"%s",name ());    
  
  glPopAttrib ();    
}

#pragma pack(1)

struct SelectEntry
{
  size_t number_of_names;
  size_t min_depth;
  size_t max_depth;
  size_t object_id;
};

inline bool depth_compare (const SelectEntry& a,const SelectEntry& b)
{
  return a.min_depth < b.min_depth;
}
    
void Viewport::select (EntityList& list,int x,int y,int sel_width,int sel_height,size_t flags)
{
  if (!GetRender ()->activate () || !GetCamera ())
    return;

  int    viewport_coords [4] = {left (),top (),width (),height ()};
  size_t draw_flags = 0;
  
  if (flags & SELECT_OBJECTS) draw_flags |= STUFF_DRAW_OBJECTS|STUFF_DRAW_LIGHTS;
  if (flags & SELECT_HELPERS) draw_flags |= STUFF_DRAW_HELPERS;
  
  glViewport     (left (),top (),width (),height ());    
  glMatrixMode   (GL_PROJECTION);
  glLoadIdentity ();
  gluPickMatrix  (x+sel_width/2,impl->reverseY (y+sel_height/2),sel_width,sel_height,viewport_coords);
  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();  
  
  vector<SelectEntry> v;

  v.resize       (MAX_SELECTED_ENTITIES);    
  glSelectBuffer (MAX_SELECTED_ENTITIES*4,(GLuint*)&v [0]);
  glRenderMode   (GL_SELECT);
  glInitNames    ();
  glPushName     (0);
  rDrawScene     (this,GetCamera ()->scene (),GetCamera (),draw_flags);
        
  int size  = glRenderMode (GL_RENDER);
  
  list.clear ();
  
  if (size == -1)
    return;
  
  v.resize (size);
  sort     (v.begin (),v.end (),depth_compare);
    
  for (int i=0;i<size;i++)
    if (v [i].object_id)
      list.insert ((Entity*)v [i].object_id);
}

void Viewport::SetSelectRect (int x,int y,int width,int height)
{
  impl->select_rect.set (x,y,width,height);
}

const Rect& Viewport::GetSelectRect ()
{
  return impl->select_rect;
}

static vec3f intersect (const vec3f& plane_pos,const vec3f& plane_dir,const vec3f& p1,const vec3f& p2)
{
  vec3f dir = normalize (p2-p1);
  float d   = -dot (plane_dir,plane_pos),
        t   = dot (plane_dir,dir);        
  
  if (fabs (t) < EPS) //прямая параллельна плоскости
    return plane_pos;

  t = -(d+dot (plane_dir,p1))/t;  
                           
  return p1 + t * dir;
}

vec3f Viewport::ProjectPointOnGrid (int screen_x,int screen_y)
{
  return ProjectPointOnPlane (screen_x,screen_y,GetGridPos (),GetGridDir ());        
}

vec3f Viewport::ProjectPointOnPlane (int screen_x,int screen_y,const vec3f& pos,const vec3f& dir)
{
  if (!GetCamera ())
    return pos;
    
  mat4f projview = GetCamera ()->GetProjTM () * invert (GetCamera ()->GetWorldTM ());  
  vec3f p;  
    
  p.x = 2.0f * float (screen_x-left ())/float (width ()) - 1.0f;
  p.y = 2.0f * float (impl->reverseY (screen_y)-top ())/float (height ()) - 1.0f;
  p.z = -1.0f;  
  
  if (fabs (GetCamera ()->GetProjTM ()[3][3]-1.0f) < EPS) //неперспективные проекции
  {
    vec3f plane_pos = projview * pos,
          plane_dir = projview * vec4f (dir,0.0f), p1, p2;    

    return invert (projview) * intersect (plane_pos,plane_dir,p,vec3f (p.x,p.y,-p.z));
  }
  else //перспективные проекции
  {   
    return intersect (pos,dir,GetCamera ()->GetWorldTM () * vec3f (0.0f),invert (projview) * p);
  }    
}

vec3f Viewport::ProjectPointOnPlane (int screen_x,int screen_y,const mat4f& object_tm)
{
  if (!GetCamera ())
    return object_tm * vec3f (0);
    
  return ProjectPointOnPlane (screen_x,screen_y,object_tm * vec3f (0),GetCamera ()->GetWorldTM () * vec4f (0,0,1,0));
}

vec2f Viewport::ProjectWorldPoint   (const vec3f& pos)
{
  vec2f t = GetCamera()->ProjectWorldPoint(pos);
  t.x = left() + (t.x + 1) / 2 * width();
  t.y = top() + (t.y + 1) / 2 * height();

  return t;
}

Scene* Viewport::GetHelperScene ()
{
  return &impl->helper_scene;
}
