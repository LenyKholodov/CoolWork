#include <cursor.h>
#include <hash_map>
#include <string>

using namespace std;
using namespace stdext;

typedef hash_map<string,RenderCursor::create_manipulator_func> ManipulatorMap;

struct RenderCursor::Impl {          //описание реализации курсора сцены
  ManipulatorMap   manipulators;     //карта функций, создающих манипуляторы
  Render*          render;           //указатель на текущий рендер
  Manipulator*     manipulator;      //текущий манипулятор
  string           next_manipulator; //имя манипулятора ожидающего активации
  Viewport*        action_viewport;  //активная область вывода (та, в которой началось действие)
  vec2i            pos;              //текущая позиция
  
  void CreateManipulator (RenderCursor&);
};

RenderCursor::RenderCursor (Render& render)
{
  impl = new Impl;
  
  impl->render           = &render;
  impl->pos              = 0;
  impl->manipulator      = NULL;
  impl->next_manipulator = "";
  impl->action_viewport  = NULL;
}

RenderCursor::~RenderCursor ()
{
  CancelAction ();

  delete impl;
}

void RenderCursor::SetActiveViewport (const char* name)
{
  impl->render->SetActiveViewport (impl->render->FindViewport (name));
}

void RenderCursor::SetActiveViewport (Viewport* vp)
{
  impl->render->SetActiveViewport (vp);
}

Viewport* RenderCursor::GetActiveViewport ()
{
  return impl->render->GetActiveViewport ();
}

Scene* RenderCursor::GetActiveScene ()
{
  if (!GetActiveViewport ())
    return NULL;

  Camera* camera = GetActiveViewport ()->GetCamera ();

  return camera ? camera->scene () : NULL;
}

void RenderCursor::SetPosition (int x,int y)
{
  impl->pos = vec2i (x,y);
  
  if (impl->manipulator && impl->action_viewport->IsInViewport (x,y))
    impl->manipulator->process (x,y);
}

vec2i RenderCursor::GetPosition ()
{
  return impl->pos;
}

void RenderCursor::Impl::CreateManipulator (RenderCursor& cursor)
{
  if (manipulator)
  {
    delete manipulator;
    
    manipulator     = NULL;
    action_viewport = NULL;      
  }
  
  action_viewport = render->FindViewport (pos.x,pos.y);
  
  if (!action_viewport)
    return;
    
  logPrintf ("Activate viewport='%s', manipulator='%s'",action_viewport->name (),next_manipulator.c_str ());    
    
  action_viewport->activate ();

  try
  {
    ManipulatorMap::iterator i = manipulators.find (next_manipulator);
    
    if (i == manipulators.end ())
    {
      action_viewport = NULL;
      return;
    }
    
    i->second (cursor,next_manipulator.c_str ());
    
    //не очень верно. временно:(
    
    if (manipulator)
      CoreChangeManipulator (next_manipulator.c_str ());
  }
  catch (...)
  {
    action_viewport = NULL;
    manipulator     = NULL;
  }  
}

void RenderCursor::BeginAction ()
{
  Viewport* vp = impl->render->FindViewport (impl->pos.x,impl->pos.y);  
  
  if (vp != impl->action_viewport)
    impl->CreateManipulator (*this);
    
  if (impl->manipulator)
    impl->manipulator->select ();
}

void RenderCursor::AcceptAction ()
{
  if (!impl->manipulator)
    return;
    
  Viewport* vp = impl->render->FindViewport (impl->pos.x,impl->pos.y);
  
  if (vp != impl->action_viewport)
    CancelAction ();
  else
    impl->manipulator->accept ();        
}

void RenderCursor::CancelAction ()
{
  if (impl->manipulator)
    delete impl->manipulator;
    
  impl->manipulator     = NULL;
  impl->action_viewport = NULL;  
}

void RenderCursor::IdleAction ()
{
  if (impl->manipulator && impl->action_viewport->IsInViewport (impl->pos.x,impl->pos.y))
    impl->manipulator->idle ();
}

bool RenderCursor::IsActive ()
{
  return impl->manipulator != NULL;
}

void RenderCursor::RegisterManipulator (const char* name,create_manipulator_func f)
{
  if (!name)
    return;

  impl->manipulators [name] = f;
}

void RenderCursor::UnregisterManipulator (const char* name)
{
  if (!name)
    return;

  ManipulatorMap::iterator i = impl->manipulators.find (name);

  if (i == impl->manipulators.end ())
    return;
  
  impl->manipulators.erase (i);
}

void RenderCursor::SetManipulator (const char* name)
{
  impl->next_manipulator = name ? name : "";
  impl->CreateManipulator (*this);
}

const char* RenderCursor::GetManipulator ()
{
  return impl->next_manipulator.c_str ();
}

bool RenderCursor::IsDefaultManipulator ()
{
  return impl->manipulator == NULL;
}

void RenderCursor::SelectEntity (Entity* entity)
{
  if (entity)
    entity->select (ENTITY_GROUP_SELECT);
}

void RenderCursor::SelectEntities (size_t count,Entity** entities)
{
  if (!entities)
    return;

  for (size_t i=0;i<count;i++)
    if (entities [i])
      entities [i]->select (ENTITY_GROUP_SELECT);
}

void RenderCursor::SelectEntities (const EntityList& list)
{
  for (size_t i=0;i<list.size ();i++)
    list.item (i)->select (ENTITY_GROUP_SELECT);
}

void RenderCursor::ClearSelection ()
{
  Scene* scene = GetActiveScene ();  
  
  if (!scene)
    return;

  scene->ClearSelection ();
}

const EntityList& RenderCursor::GetSelection ()
{
  static EntityList err_list;

  Scene* scene = GetActiveScene ();
  
  return scene ? scene->GetSelection () : err_list;
}

void RenderCursor::NewSelection (int x,int y,int width,int height)
{         
  if (!GetActiveScene ())
    return;

  ClearSelection ();
    
  EntityList list;

  GetActiveViewport ()->select (list,x,y,width,height,SELECT_OBJECTS);
                        
  SelectEntities (list);
}

void RenderCursor::NewSelection ()
{
  NewSelection (impl->pos.x,impl->pos.y);
}

/*
    Manipulator
*/

Manipulator::Manipulator (RenderCursor& _cursor)
  : cursor (_cursor)
{ 
  _cursor.impl->manipulator = this;
}
 
Manipulator::~Manipulator ()
{
  if (cursor.impl->manipulator == this)
  {
    cursor.impl->manipulator     = NULL;
    cursor.impl->action_viewport = NULL;    
  }
}
 
void Manipulator::finish ()
{
  delete this;
}
