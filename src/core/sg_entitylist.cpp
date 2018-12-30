#include <scene.h>
#include <vector>
#include <algorithm>

using namespace std;

typedef vector<Entity*> EntityListImpl;

struct EntityList::Impl { //реализация списка объектов
  EntityListImpl entities;
};

EntityList::EntityList ()
{
  impl = new Impl;
}

EntityList::EntityList (const EntityList& el)
{
  impl = new Impl;
  
  impl->entities = el.impl->entities;  
}
    
EntityList::~EntityList ()
{
  clear ();
    
  delete impl;  
}

EntityList& EntityList::operator = (const EntityList& el)
{
  clear ();
  
  impl->entities = el.impl->entities;  
  
  return *this;
}

void EntityList::clear ()
{
  impl->entities.clear ();
}

size_t EntityList::size () const
{
  return impl->entities.size ();
}

Entity* EntityList::item (size_t index) const
{
  return index < impl->entities.size () ? impl->entities [index] : NULL;
}

bool EntityList::test (Entity* entity) const
{
  return find (impl->entities.begin (),impl->entities.end (),entity) != impl->entities.end ();
}
 
void EntityList::insert (Entity* entity)
{
  if (!entity)
    return;
    
  impl->entities.push_back (entity);  
}

void EntityList::remove (Entity* entity)
{
  if (!entity)
    return;
    
  EntityListImpl::iterator first = std::remove (impl->entities.begin (),impl->entities.end (),entity);  
    
  impl->entities.erase (first,impl->entities.end ());
}
    
void EntityList::traverse (EntityVisitor* visitor,Traverse type)
{
  for (EntityListImpl::iterator i=impl->entities.begin ();i!=impl->entities.end ();i++)
    (*i)->traverse (visitor,type);
}
