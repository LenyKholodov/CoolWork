#include <scene.h>
#include <string>
#include <hash_map>
#include "misc.h"

using namespace std;
using namespace stdext;

typedef hash_map<string,string> ExpressionMap;

struct Entity::Impl {        //описание реализации класса Entity
  Entity*       owner;       //владелец
  mat4f         local_tm;    //локальная матрица положения объекта
  mat4f         world_tm;    //мировая матрица положения объекта
  size_t        ref_count;   //счётчик активных ссылок
  string        name;        //имя
  size_t        flags;       //флаги (ENTITY_VISIBLE, ENTITY_WIREFRAME, etc.)
  Entity*       parent;      //родительский объект  
  Entity*       first;       //первый потомок
  Entity*       last;        //последний ребёнок
  Entity*       prev;        //предыдущий узел на данном уровне иерархии
  Entity*       next;        //следующий узел на данном уровне иерархии
  bool          dirty_tm;    //флаг, сигнализирующий об обновлении положения
  vec3f         wire_color;  //цвет каркасного представления
  Scene*        scene;       //сцена
  bool          notify;      //флаг указывающий о необходимости сигнализирования
  bool          reg_vars;    //флаг указывающий, что переменные состояния зарегистрированы
  ExpressionMap expressions; //математические формулы для параметров  
  
  static size_t entity_counter;
  
  bool is_parent (Entity*);
  
  void set_local_transform (const vec3f& pos,const vec3f& rot,const vec3f& scale);
  void set_world_transform (const vec3f& pos,const vec3f& rot,const vec3f& scale);
  
  void InvalidateTM ();
  void _InvalidateTM (); 

  void OnSetName          (Var&);    
  void OnGetName          (Var&);  
  void OnGetParent        (Var&);  
  void OnSetWireColor     (Var&);
  void OnGetWireColor     (Var&);
  void OnSetLocalPosition (Var&);
  void OnGetLocalPosition (Var&);
  void OnSetLocalRotation (Var&);
  void OnGetLocalRotation (Var&);
  void OnSetLocalScale    (Var&);
  void OnGetLocalScale    (Var&);
  void OnSetWorldPosition (Var&);
  void OnGetWorldPosition (Var&);
  void OnSetWorldRotation (Var&);
  void OnGetWorldRotation (Var&);
  void OnSetWorldScale    (Var&);
  void OnGetWorldScale    (Var&);  
  void OnSetVisible       (Var&);
  void OnGetVisible       (Var&);
  void OnSetSelect        (Var&);
  void OnGetSelect        (Var&);
  
  void OnGetLocalDirection (Var&);
  void OnGetLocalUp        (Var&);  
  void OnGetWorldDirection (Var&);
  void OnGetWorldUp        (Var&);
  void OnSetEmpty          (Var&) {}
};

size_t Entity::Impl::entity_counter = 0;

Entity::Entity (Scene* scene)
{
  impl = new Impl;

  impl->owner    = this;  
  impl->local_tm = 1.0f;
  impl->world_tm = 1.0f;
  impl->dirty_tm = true;
  
  impl->first  = NULL;
  impl->last   = NULL;
  impl->prev   = NULL;  
  impl->next   = NULL;
  impl->parent = NULL;  
  impl->notify = false;
  impl->reg_vars = false;  
  impl->scene  = NULL;
  
  impl->wire_color = vrand (vec3f (0.3f),vec3f (0.9f));
  impl->ref_count  = 1;
  impl->flags      = ENTITY_VISIBLE;  
  
  rename (NULL);
  
  impl->scene = scene;
}

Entity::~Entity ()
{
  while (impl->first)
    impl->first->bind (impl->parent);
    
  unbind ();

  delete impl;
}

void Entity::destroy ()
{
  if (this == NULL) //не очень верно, зато безопасно
    return;

  while (first ())
    first ()->destroy ();
  
  OnDelete ();
  _UnregisterVars ();

  delete this;  
}

void Entity::addref ()
{
  impl->ref_count++;
}

void Entity::release ()
{
  if (this == NULL) //не очень верно, зато безопасно
    return;

  if (!--impl->ref_count)
  {
    OnDelete ();
    _UnregisterVars ();

    delete this;
  }
}

void Entity::OnDelete ()
{
  if (!impl->scene || !impl->notify)
    return;
    
  impl->scene->OnDelete (this);
  impl->notify = false;
}

void Entity::OnCreate ()
{
  if (!impl->scene || impl->notify)
    return;
 
  impl->notify = true;
  impl->scene->OnCreate (this);  
}

void Entity::OnModify ()
{
  if (!impl->scene || !impl->notify)
    return;
    
  impl->scene->OnModify (this);
}

void Entity::OnSelect ()
{
  if (!impl->scene || !impl->notify)
    return;
    
  impl->scene->OnSelect (this);  
}

Scene* Entity::scene ()
{
  return impl->scene;
}

void Entity::rename (const char* _name)
{
  char tmp [128];
  int count = 0;
  
  if (_name && !strcmp (_name,name ()))
    return;  
  
  if (!_name)  _snprintf (tmp,sizeof (tmp),"Entity%d",Impl::entity_counter++);
  else         strncpy (tmp,_name,sizeof (tmp));
  
  if (scene () && scene ()->root ()->find (tmp))
  {    
    char etalon [sizeof (tmp)];
    
    strcpy (etalon,tmp);
    
    do
    {
      _snprintf (tmp,sizeof (tmp),"%s%d",etalon,count++);
      
      if (!strcmp (etalon,tmp))
        _snprintf (tmp,sizeof (tmp),"%d%s",count++,etalon);     
    } while (scene ()->root ()->find (tmp));
  }
  
//  logPrintf ("\tEntity: Rename from '%s' to '%s'",name (),tmp);
  
  if (impl->reg_vars)
    UnregisterVars ();
  
  impl->name = tmp;

  if (impl->reg_vars)
    RegisterVars ();    
    
  OnModify ();  
}

const char* Entity::name ()
{
  return impl->name.c_str ();
}

Entity* Entity::find (const char* _name)
{
//  logPrintf ("\ttry entity '%s', name='%s'",name (),_name);
  
  if (!_name)
    return NULL;

  if (!strcmp (_name,name ()))
    return this;
      
  for (Entity* i=first ();i;i=i->next ())
  {
    Entity* result = i->find (_name);
    
    if (result)
      return result;
  }
  
  return NULL;
}

void Entity::enable (size_t flags)
{
  if (!(flags ^ impl->flags))
    return;
    
  bool selected = IsSelected ();
    
  impl->flags |= flags;
  
  if (!selected && (flags & ENTITY_SELECT))
  {
    scene ()->select (this,true);
    
    if (impl->notify)
      Command::instance ().etc ().update (property ("select"));    
    
    OnSelect ();
  }
  
  if (flags & ~ENTITY_SELECT)
    OnModify ();
}

void Entity::disable (size_t flags)
{
  if (!(~flags ^ impl->flags))
    return;
    
  bool selected = IsSelected (); 
  
  impl->flags &= ~flags;  

  if (selected && (flags & ENTITY_SELECT))
  {
    scene ()->select (this,false);
    
    if (impl->notify)
      Command::instance ().etc ().update (property ("select"));
      
    OnSelect ();
  }
  
  if (flags & ~ENTITY_SELECT)
    OnModify ();
}

bool Entity::IsEnabled (size_t flags)
{
  return (impl->flags & flags) != 0;
}

void Entity::select (EntitySelect select)
{
  switch (select)
  {
    case ENTITY_EXCLUSIVE_SELECT:
      if (scene ())
        scene ()->ClearSelection ();
    case ENTITY_GROUP_SELECT: 
      enable (ENTITY_SELECT); 
      break;
    default:
    case ENTITY_CLEAR_SELECT:
      disable (ENTITY_SELECT);
      break;
  }
}

bool Entity::Impl::is_parent (Entity* p)
{  
  if (parent == p)
    return true;

  return parent ? parent->impl->is_parent (p) : false;
}

void Entity::bind (Entity* new_parent,bool save_world_pos)
{
  if (new_parent == this)
    return;
    
  if (new_parent->impl->is_parent (this))
  {        
    logPrintf ("Try to bind parent to it child");
    return;
  }

  Scene* old_scene = scene ();
  bool   selected  = IsSelected ();    
  mat4f  world_tm  = GetWorldTM ();

  if (impl->parent)
    unbind ();

  impl->parent = new_parent;    

  if (!new_parent)
    return;
    
  impl->prev  = new_parent->impl->last;
  impl->next  = NULL;
  impl->scene = new_parent->scene ();

  if (new_parent->impl->last) 
    impl->prev->impl->next = this;        
  else
    new_parent->impl->first = this;    

  new_parent->impl->last = this;  
  
  if (old_scene == scene () && selected)
    enable (ENTITY_SELECT);
    
  if (old_scene != scene ())
    rename (name ());  
    
  if (save_world_pos)
  {
    identity  ();
    transform (invert (GetWorldTM ()) * world_tm);
  }
    
  OnCreate ();  
}

void Entity::unbind (bool save_world_pos)
{
  if (!impl->parent)
    return;
    
  mat4f world_tm = GetWorldTM ();  
    
  OnDelete ();
    
  if (IsSelected ())
    disable (ENTITY_SELECT);

  if (impl->next)  impl->next->impl->prev = impl->prev;
  if (impl->prev)  impl->prev->impl->next = impl->next;
  
  if (impl->parent->impl->first == this) 
    impl->parent->impl->first = impl->next;
    
  if (impl->parent->impl->last == this) 
    impl->parent->impl->last = impl->prev;

  impl->parent = NULL;
  impl->next   = impl->prev = NULL;
  impl->scene  = NULL;
  
  if (save_world_pos)
  {
    identity  ();
    transform (world_tm);
  }
}

size_t Entity::GetNodeDepth ()
{
  return impl->parent ? impl->parent->GetNodeDepth ()+1 : 0;
}

Entity* Entity::parent ()
{
  return impl->parent;
}

Entity* Entity::first ()
{
  return impl->first;
}

Entity* Entity::next ()
{
  return impl->next;
}

Entity* Entity::prev ()
{
  return impl->prev;
}

Entity* Entity::last ()
{
  return impl->last;
}

void Entity::Impl::InvalidateTM ()
{
  Etc& etc = Command::instance ().etc ();
    
  etc.update (owner->property ("local_position"));
  etc.update (owner->property ("local_rotation"));
  etc.update (owner->property ("local_scale"));

  _InvalidateTM ();
  owner->OnModify ();
}

void Entity::Impl::_InvalidateTM ()
{
  if (dirty_tm)
    return;   
    
  dirty_tm = true;

  for (Entity* i=first;i;i=i->impl->next)
    i->impl->InvalidateTM ();
}

void Entity::translate (const vec3f& pos)
{
//  impl->local_tm = ::translate (pos) * impl->local_tm;
  impl->local_tm *= ::translate (pos);
  impl->InvalidateTM ();
}

void Entity::rotate (const quatf& q)
{
//  impl->local_tm = q * impl->local_tm;
  impl->local_tm *= q;
  impl->InvalidateTM ();
}

void Entity::rotate (float angle_in_degrees,const vec3f& axis)
{
//  impl->local_tm = quatf (deg2rad (angle_in_degrees),axis.x,axis.y,axis.z) * impl->local_tm;
  impl->local_tm *= rotatef (deg2rad (angle_in_degrees),axis.x,axis.y,axis.z);
  impl->InvalidateTM ();
}

void Entity::rotate (float tangent,float yaw,float roll)
{
//  impl->local_tm = fromEulerAnglef (tangent,yaw,roll) * impl->local_tm;
  impl->local_tm *= fromEulerAnglef (tangent,yaw,roll);
  impl->InvalidateTM ();
}

void Entity::scale (const vec3f& s)
{
//  impl->local_tm = ::scale (s) * impl->local_tm;
  impl->local_tm *= ::scale (s);
  impl->InvalidateTM ();
}

void Entity::lookat (const vec3f& pos,const vec3f& center,const vec3f& up)
{      
  impl->local_tm = lookatf (pos,center,up);  
  impl->InvalidateTM ();
}

void Entity::lookat (const vec3f& pos,const vec3f& center)
{
  const float EPS = 0.001f;

  vec3f dir = length (center-pos) > EPS ? normalize (center-pos) : vec3f (0.0f,0.0f,1.0f);  
  vec3f up  = equal (abs (dir),vec3f (0.0f,1.0f,0.0f),EPS) ? vec3f (0.0f,0.0f,1.0f) : vec3f (0.0f,1.0f,0.0f);
  
  lookat (pos,pos+dir,up);
}

void Entity::identity ()
{
  impl->local_tm = 1.0f;
  impl->InvalidateTM ();
}

void Entity::transform (const mat4f& tm)
{
  impl->local_tm *= tm;
  impl->InvalidateTM ();
}

const mat4f& Entity::GetLocalTM ()
{
  return impl->local_tm;
}

const mat4f& Entity::GetWorldTM ()
{
  if (impl->dirty_tm)
  {
    impl->world_tm = impl->parent ? impl->parent->GetWorldTM () * GetLocalTM () : GetLocalTM ();
    impl->dirty_tm = false;
  }
    
  return impl->world_tm;  
}

//возвращение матрицы положения в координатах base
const mat4f Entity::GetObjectTM (Entity* obj)
{
  return obj ? invert (GetWorldTM ()) * obj->GetWorldTM () : invert (GetWorldTM ());
}

Entity* Entity::pivot ()
{
  return NULL; //пока центра нет!!!
}

void Entity::SetWireColor (const vec3f& color)
{
  impl->wire_color = color;
  OnModify ();
}

const vec3f& Entity::GetWireColor ()
{
  return impl->wire_color;  
}

void Entity::visit (EntityVisitor* visitor)
{
  visitor->visit (this);
}

void Entity::traverse (EntityVisitor* visitor,Traverse type)
{
  if (!visitor)
    return;

  switch (type)
  {
    case TRAVERSE_BOTTOM_TO_TOP:
    {
      for (Entity* i=impl->first;i;i=i->impl->next)
        i->traverse (visitor,type);
    }
    case TRAVERSE_NO_RECURSION:
      visit (visitor);
      break;
    case TRAVERSE_TOP_TO_BOTTOM:
    {
      visit (visitor);    
    
      for (Entity* i=impl->first;i;i=i->impl->next)
        i->traverse (visitor,type);
      break;  
    }
  }  
}

const char* Entity::property (const char* property_name)
{
  static char buf [256];  
  
  _snprintf (buf,sizeof (buf),"%s.%s",name (),property_name);
  
  return buf;
}

void Entity::Impl::OnSetName (Var& var)
{
  owner->rename (var.gets ());
}

void Entity::Impl::OnGetName (Var& var)
{
  var.set (owner->name ());
}

void Entity::Impl::OnSetWireColor (Var& var)
{
  owner->SetWireColor (var.get3f ());
}

void Entity::Impl::OnGetWireColor (Var& var)
{
  var.set (owner->GetWireColor ());
}

void Entity::Impl::OnSetVisible (Var& var)
{
  if (var.geti ()) owner->enable  (ENTITY_VISIBLE);
  else             owner->disable (ENTITY_VISIBLE);
}

void Entity::Impl::OnGetVisible (Var& var)
{
  var.set (owner->IsEnabled (ENTITY_VISIBLE)!=0);
}

void Entity::Impl::OnSetSelect (Var& var)
{
  if (var.geti ()) owner->enable  (ENTITY_SELECT);
  else             owner->disable (ENTITY_SELECT);
}

void Entity::Impl::OnGetSelect (Var& var)
{
  var.set (owner->IsSelected ()!=0);  
}

void Entity::Impl::set_local_transform (const vec3f& pos,const vec3f& rot,const vec3f& scale)
{
  local_tm = ::translate (pos) * rotatef (deg2rad (rot.z),0,0,1) * 
        rotatef (deg2rad (rot.y),0,1,0) * rotatef (deg2rad (rot.x),1,0,0) * ::scale (scale);

  _InvalidateTM ();
}

void Entity::Impl::set_world_transform (const vec3f& pos,const vec3f& rot,const vec3f& scale)
{
  mat4f invParent = parent ? invert (parent->GetWorldTM ()) : 1.0f;

  local_tm = invParent * ::translate (pos) * rotatef (deg2rad (rot.z),0,0,1) * 
        rotatef (deg2rad (rot.y),0,1,0) * rotatef (deg2rad (rot.x),1,0,0) * ::scale (scale);

  _InvalidateTM ();
}

void Entity::Impl::OnSetLocalPosition (Var& var)
{
  vec3f pos, rot, scale;   

  affine_decomp (owner->GetLocalTM (),pos,rot,scale);
    
  pos = var.get3f ();    
    
  set_local_transform (pos,rot,scale);  
  
  owner->OnModify ();
}

void Entity::Impl::OnGetLocalPosition (Var& var)
{
  vec3f pos, rot, scale;

  affine_decomp (owner->GetLocalTM (),pos,rot,scale);
  
  var.set (pos);
}

void Entity::Impl::OnSetLocalRotation (Var& var)
{
  vec3f pos, rot, scale;  

  affine_decomp (owner->GetLocalTM (),pos,rot,scale);
    
  rot = var.get3f ();   
    
  set_local_transform (pos,rot,scale);  
  
  owner->OnModify ();  
}

void Entity::Impl::OnGetLocalRotation (Var& var)
{
  vec3f pos, rot, scale;

  affine_decomp (owner->GetLocalTM (),pos,rot,scale);  
  
  var.set (rot);
}

void Entity::Impl::OnSetLocalScale (Var& var)
{
  vec3f pos, rot, scale;

  affine_decomp (owner->GetLocalTM (),pos,rot,scale);
    
  scale = var.get3f ();
  
  set_local_transform (pos,rot,scale);      
  
  owner->OnModify ();  
}

void Entity::Impl::OnGetLocalScale (Var& var)
{
  vec3f pos, rot, scale;

  affine_decomp (owner->GetLocalTM (),pos,rot,scale);
    
  var.set (scale);
}

void Entity::Impl::OnGetLocalDirection (Var& var)
{
  var.set (owner->GetLocalTM () * vec4f (0,0,1,0));
}

void Entity::Impl::OnGetLocalUp (Var& var)
{
  var.set (owner->GetLocalTM () * vec4f (0,1,0,0));
}

void Entity::Impl::OnGetWorldDirection (Var& var)
{
  var.set (owner->GetWorldTM () * vec4f (0,0,1,0));
}

void Entity::Impl::OnGetWorldUp (Var& var)
{
  var.set (owner->GetWorldTM () * vec4f (0,1,0,0));
}

void Entity::Impl::OnSetWorldPosition (Var& var)
{
  vec3f pos, rot, scale;   

  affine_decomp (owner->GetWorldTM (),pos,rot,scale);
    
  pos = var.get3f ();
    
  set_world_transform (pos,rot,scale);  
  
  owner->OnModify ();
}

void Entity::Impl::OnGetWorldPosition (Var& var)
{
  vec3f pos, rot, scale;

  affine_decomp (owner->GetWorldTM (),pos,rot,scale);
    
  var.set (pos);
}

void Entity::Impl::OnSetWorldRotation (Var& var)
{
  vec3f pos, rot, scale;   

  affine_decomp (owner->GetWorldTM (),pos,rot,scale);
    
  rot = var.get3f ();
    
  set_world_transform (pos,rot,scale);  
  
  owner->OnModify ();
}

void Entity::Impl::OnGetWorldRotation (Var& var)
{
  vec3f pos, rot, scale;

  affine_decomp (owner->GetWorldTM (),pos,rot,scale);
    
  var.set (rot);
}

void Entity::Impl::OnSetWorldScale (Var& var)
{
  vec3f pos, rot, scale;   

  affine_decomp (owner->GetWorldTM (),pos,rot,scale);
    
  scale = var.get3f ();
    
  set_world_transform (pos,rot,scale);  
  
  owner->OnModify ();
}

void Entity::Impl::OnGetWorldScale (Var& var)
{
  vec3f pos, rot, scale;

  affine_decomp (owner->GetWorldTM (),pos,rot,scale);
    
  var.set (scale);
}

void Entity::Impl::OnGetParent (Var& var)
{
  var.set (parent ? parent->name () : "");  
}

void Entity::SetExpression (const char* pname,const char* expression)
{
  if (!pname)
    return;

  if (!expression || !*expression)
  {
    impl->expressions.erase (pname);
  }
  else impl->expressions [pname] = expression;
  
  Command::instance ().etc ().set_expression (property (pname),expression);
}

const char* Entity::GetExpression (const char* pname)
{
  return pname ? Command::instance ().etc ().get_expression (property (pname)) : "";  
}

void Entity::RegisterVars ()
{
  Etc& etc = Command::instance ().etc ();
  
  etc.set (property ("type"),"unknown");      
  
  etc.bind_handler (property ("name"),*impl,&Impl::OnSetName,&Impl::OnGetName);
  etc.bind_handler (property ("parent"),*impl,&Impl::OnSetEmpty,&Impl::OnGetParent);  
  etc.bind_handler (property ("wire_color"),*impl,&Impl::OnSetWireColor,&Impl::OnGetWireColor);
  etc.bind_handler (property ("visible"),*impl,&Impl::OnSetVisible,&Impl::OnGetVisible);
  etc.bind_handler (property ("select"),*impl,&Impl::OnSetSelect,&Impl::OnGetSelect);
  etc.bind_handler (property ("local_position"),*impl,&Impl::OnSetLocalPosition,&Impl::OnGetLocalPosition);
  etc.bind_handler (property ("local_rotation"),*impl,&Impl::OnSetLocalRotation,&Impl::OnGetLocalRotation);
  etc.bind_handler (property ("local_scale"),*impl,&Impl::OnSetLocalScale,&Impl::OnGetLocalScale);
  etc.bind_handler (property ("local_dir"),*impl,&Impl::OnSetEmpty,&Impl::OnGetLocalDirection);
  etc.bind_handler (property ("local_up"),*impl,&Impl::OnSetEmpty,&Impl::OnGetLocalUp);  
  etc.bind_handler (property ("world_position"),*impl,&Impl::OnSetWorldPosition,&Impl::OnGetWorldPosition);
  etc.bind_handler (property ("world_rotation"),*impl,&Impl::OnSetWorldRotation,&Impl::OnGetWorldRotation);
  etc.bind_handler (property ("world_scale"),*impl,&Impl::OnSetWorldScale,&Impl::OnGetWorldScale);  
  etc.bind_handler (property ("world_dir"),*impl,&Impl::OnSetEmpty,&Impl::OnGetWorldDirection);
  etc.bind_handler (property ("world_up"),*impl,&Impl::OnSetEmpty,&Impl::OnGetWorldUp);
  
  for (ExpressionMap::iterator i=impl->expressions.begin ();i!=impl->expressions.end ();i++)
    etc.set_expression (property (i->first.c_str ()),i->second.c_str ());
}

void Entity::UnregisterVars ()
{
  Etc& etc = Command::instance ().etc ();

  etc.remove (property ("name"));
  etc.remove (property ("parent")); 
  etc.remove (property ("type"));
  etc.remove (property ("wire_color"));
  etc.remove (property ("visible"));
  etc.remove (property ("select"));  
  etc.remove (property ("local_position"));
  etc.remove (property ("local_rotation"));
  etc.remove (property ("local_scale"));  
  etc.remove (property ("local_dir"));
  etc.remove (property ("local_up"));  
  etc.remove (property ("world_position"));
  etc.remove (property ("world_rotation"));
  etc.remove (property ("world_scale"));    
  etc.remove (property ("world_dir"));
  etc.remove (property ("world_up"));
}

void Entity::_RegisterVars ()
{
  if (!impl->reg_vars)
    RegisterVars ();

  impl->reg_vars = true;
}

void Entity::_UnregisterVars ()
{
  if (impl->reg_vars)
    UnregisterVars ();

  impl->reg_vars = false;
}

void Entity::RemoveDependencies ()
{
  UnregisterVars ();
  
  impl->expressions.clear ();
  
  RegisterVars ();
  
  for (Entity* i=first ();i;i=i->next ())
    i->RemoveDependencies ();
}
