#include <scene.h>
#include <list>

using namespace std;

typedef list<SceneListener*> ListenerList;

struct Scene::Impl {       //описание реализации сцены
  Entity*      root;       //корневой объект
  EntityList   selection;  //выделенные объекты
  ListenerList listeners;  //слушатели сцены
  bool         reg_vars;
};

Scene::Scene (bool reg_vars)
{  
  impl           = new Impl;
  impl->root     = new Entity (this);
  impl->reg_vars = reg_vars;
  
  impl->root->rename ("SceneRoot");  
  impl->root->_RegisterVars ();
}

Scene::Scene (const char* file_name,bool reg_vars)
{
  impl           = new Impl;
  impl->root     = new Entity (this);
  impl->reg_vars = reg_vars;  
  
  impl->root->rename ("SceneRoot");    
  impl->root->_RegisterVars ();  
  
  load (file_name);
}

Scene::~Scene ()
{
  while (!impl->listeners.empty ())
    UnregisterListener (impl->listeners.back ());

  impl->root->destroy ();
  delete impl;
}
      
Entity* Scene::root ()
{
  return impl->root;
}   

/*
    Сброс сцены
*/

void Scene::reset ()
{
  impl->root->destroy ();
  
  impl->root = new Entity (this);
  impl->root->rename ("SceneRoot");
  
  if (impl->reg_vars)
    impl->root->_RegisterVars ();
}

/*
    Создание объектов
*/

template <class T>
T* Scene::CreateObject ()
{
  T* obj = new T;
  
  if (impl->reg_vars)
    obj->_RegisterVars ();  
    
  obj->bind (root ());
  
  return obj;
}

Entity* Scene::CreateEntity ()
{
  return CreateObject<Entity> ();
}

Light* Scene::CreateLight ()
{
  return CreateObject<Light> ();
}

Camera* Scene::CreateCamera ()
{
  return CreateObject<Camera> ();
}

ParamModel* Scene::CreateModel ()
{
  return CreateObject<ParamModel> ();
}

HelperEntity* Scene::CreateHelper ()
{
  return CreateObject<HelperEntity> ();
}

LandscapeModel* Scene::CreateLandscape ()
{
  return CreateObject<LandscapeModel> ();
}

/*
    Работа с выделением
*/

void Scene::select (Entity* entity,bool state)
{
  if (state) impl->selection.insert (entity);
  else       impl->selection.remove (entity);
}

void Scene::ClearSelection ()
{
  while (impl->selection.size ())
    impl->selection.item (0)->select (ENTITY_CLEAR_SELECT);
}

const EntityList& Scene::GetSelection ()
{
  return impl->selection;
}

/*
    Прослушивание сцены
*/

class RegisterListenerVisitor: public EntityVisitor
{
  public:
    RegisterListenerVisitor (SceneListener& _listener) : listener (_listener) {}
  
    void visit (Entity* obj)       { listener.OnCreate (obj); }
    void visit (Light* obj)        { listener.OnCreate (obj); }
    void visit (Camera* obj)       { listener.OnCreate (obj); }
    void visit (ParamModel* obj)   { listener.OnCreate (obj); }
    void visit (HelperEntity* obj) { listener.OnCreate (obj); }               
  
  private:
    SceneListener& listener;
};

class UnregisterListenerVisitor: public EntityVisitor
{
  public:
    UnregisterListenerVisitor (SceneListener& _listener) : listener (_listener) {}
  
    void visit (Entity* obj)       { listener.OnDelete (obj); }
    void visit (Light* obj)        { listener.OnDelete (obj); }
    void visit (Camera* obj)       { listener.OnDelete (obj); }
    void visit (ParamModel* obj)   { listener.OnDelete (obj); }
    void visit (HelperEntity* obj) { listener.OnDelete (obj); }               
  
  private:
    SceneListener& listener;
};

void Scene::RegisterListener (SceneListener* listener)
{
  if (!listener)
    return;
    
  impl->listeners.push_back (listener);    
    
  root ()->traverse (&RegisterListenerVisitor (*listener));
}

void Scene::UnregisterListener (SceneListener* listener)
{
  if (!listener)
    return;    
    
  root ()->traverse (&UnregisterListenerVisitor (*listener),TRAVERSE_BOTTOM_TO_TOP);
  
  impl->listeners.remove (listener);
}

enum NotifyType {
  NOTIFY_CREATE,
  NOTIFY_DELETE,
  NOTIFY_MODIFY,
  NOTIFY_SELECT
};

template <NotifyType notify_type> struct Notify
{
  template <class T> static void doNotify (const ListenerList&,T*) {}  
};

template <> struct Notify<NOTIFY_CREATE>
{
  template <class T> static void doNotify (const ListenerList& list,T* obj) {
    for (ListenerList::const_iterator i=list.begin ();i!=list.end ();++i)  (*i)->OnCreate (obj);    
  }  
};

template <> struct Notify<NOTIFY_DELETE>
{
  template <class T> static void doNotify (const ListenerList& list,T* obj) {
    for (ListenerList::const_iterator i=list.begin ();i!=list.end ();++i) (*i)->OnDelete (obj);    
  }  
};

template <> struct Notify<NOTIFY_MODIFY>
{
  template <class T> static void doNotify (const ListenerList& list,T* obj) {
    for (ListenerList::const_iterator i=list.begin ();i!=list.end ();++i) (*i)->OnModify (obj);    
  }
};

template <> struct Notify<NOTIFY_SELECT>
{
  template <class T> static void doNotify (const ListenerList& list,T* obj) {
    for (ListenerList::const_iterator i=list.begin ();i!=list.end ();++i) (*i)->OnSelect (obj);
  }
};

template <NotifyType notify_type>
class NotifyVisitor: public EntityVisitor
{
  public:
    NotifyVisitor (const ListenerList& _listeners) : listeners (_listeners) {}
  
    void visit (Entity* obj)       { Notify<notify_type>::doNotify (listeners,obj); }
    void visit (Light* obj)        { Notify<notify_type>::doNotify (listeners,obj); }
    void visit (Camera* obj)       { Notify<notify_type>::doNotify (listeners,obj); }
    void visit (ParamModel* obj)   { Notify<notify_type>::doNotify (listeners,obj); }
    void visit (HelperEntity* obj) { Notify<notify_type>::doNotify (listeners,obj); }    
    
  private:
    const ListenerList& listeners;    
};

void Scene::OnCreate (Entity* entity)
{
  entity->visit (&NotifyVisitor<NOTIFY_CREATE> (impl->listeners));
}

void Scene::OnModify (Entity* entity)
{
  entity->visit (&NotifyVisitor<NOTIFY_MODIFY> (impl->listeners));
}

void Scene::OnDelete (Entity* entity)
{
  entity->visit (&NotifyVisitor<NOTIFY_DELETE> (impl->listeners));
}

void Scene::OnSelect (Entity* entity)
{
  Notify<NOTIFY_SELECT>::doNotify (impl->listeners,entity);
}
