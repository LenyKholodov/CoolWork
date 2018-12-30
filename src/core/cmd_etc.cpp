#include <etc.h>
#include <hash_map>
#include <set>
#include <string>

using namespace std;
using namespace stdext;

typedef set<string>                 VarNameSet;
typedef hash_map<string,Var*>       VarMap;
typedef hash_map<string,VarNameSet> DepMap;

struct Etc::Impl {  //описание реализации пула переменных
  VarMap     map;   //карта переменных 
  DepMap     deps;  //карта зависимостей
};

Var* Etc::find_or_create (const char* name)
{
  if (!name)  
    return NULL;
    
  VarMap::iterator i   = impl->map.find (name);
  Var*             var = i == impl->map.end () ? new Var (*this,name) : i->second;
  
  impl->map [name] = var;
  
  return var;
}

Etc::Etc ()
{
  impl = new Impl;
}

Etc::~Etc ()
{
  clear ();
  
  delete impl;
}

Var* Etc::find (const char* name)
{
  if (!name)
    return NULL;
    
  VarMap::iterator i = impl->map.find (name);

  if (i == impl->map.end ())
    return NULL;

  return i->second;    
}

bool Etc::present (const char* name)
{
  return find (name) != NULL;
}

void Etc::remove (const char* name)
{
  if (!name)
    return;

  VarMap::iterator i = impl->map.find (name);
  
  if (i == impl->map.end ())
    return;
    
  delete i->second;  

  impl->map.erase (i);    
}

void Etc::clear ()
{
  for (VarMap::iterator i=impl->map.begin ();i!=impl->map.end ();i++)
    delete i->second;
    
  impl->map.clear ();  
}
    
void Etc::bind (const char* name,int& value)
{
  if (!name)
    return;
    
  find_or_create (name)->bind (value);
}

void Etc::bind (const char* name,float& value)
{
  if (!name)
    return;
    
  find_or_create (name)->bind (value);
}

void Etc::bind (const char* name,vec3f& value)
{
  if (!name)
    return;
    
  find_or_create (name)->bind (value);
}

void Etc::bind (const char* name,char* buf,size_t max_size)
{
  if (!name)
    return;

  find_or_create (name)->bind (buf,max_size);
}

void Etc::bind_handler (const char* name,OnUpdateHandler on_set,OnUpdateHandler on_get)
{
  if (!name)
    return;

  find_or_create (name)->bind_handler (on_get,on_set);
}

void Etc::unbind (const char* name)
{
  if (!name)
    return;

  VarMap::iterator i = impl->map.find (name);
  
  if (i == impl->map.end ())
    return;
    
  i->second->unbind ();  
}

void Etc::set (const char* name,const char* value)
{
  if (!name)
    return;  
    
  find_or_create (name)->set (value);
}

void Etc::set (const char* name,int value)
{
  if (!name)
    return;
    
  find_or_create (name)->set (value);    
}

void Etc::set (const char* name,float value)
{
  if (!name)
    return;

  find_or_create (name)->set (value);        
}

void Etc::set (const char* name,const vec3f& value)
{
  if (!name)
    return;
    
  find_or_create (name)->set (value);    
}

void Etc::set_expression (const char* name,const char* expression)
{
  if (!name)
    return;
    
  find_or_create (name)->set_expression (expression);
}

const char* Etc::get_expression (const char* name)
{
  Var* var = find (name);
  
  return var ? var->get_expression () : "";
}

const char* Etc::gets  (const char* name)
{
  Var* var = find (name);
  
  return var ? var->gets () : "";        
}

int Etc::geti  (const char* name)
{
  Var* var = find (name);
  
  return var ? var->geti () : 0;
}

float Etc::getf  (const char* name)
{
  Var* var = find (name);
  
  return var ? var->getf () : 0.0f;
}

vec3f Etc::get3f (const char* name)
{
  Var* var = find (name);  
  
  return var ? var->get3f () : 0.0f;
}

void Etc::group_update (const char* mask)
{
  if (!mask)
    return;  

  for (VarMap::iterator i=impl->map.begin ();i!=impl->map.end ();i++)
    if (maskcmp (i->second->name (),mask))
      update (i->second->name ());
}

void Etc::update (const char* name)
{
  Var* var = find (name);  
  
  if (!var)
    return;        
    
  DepMap::iterator dep = impl->deps.find (name);
  
  if (dep != impl->deps.end ())
  {
    for (VarNameSet::iterator i=dep->second.begin ();i!=dep->second.end ();i++)
      update (i->c_str ());
  }

  var->gets ();  
}

void Etc::add_dep (const char* var_name,const char* dep_name)
{
  if (!var_name || !dep_name || !*var_name || !*dep_name || !strcmp (var_name,dep_name))
    return;        
    
  VarNameSet& l = impl->deps [dep_name];
  
  l.insert (var_name);    
}

void Etc::remove_dep (const char* var_name,const char* dep_name)
{
  if (!var_name || !*var_name || !dep_name || !*dep_name)
    return;
    
  DepMap::iterator i = impl->deps.find (var_name);
  
  if (i == impl->deps.end ())
    return;
  
  i->second.erase (dep_name);
}

inline bool compare_vars (Var*& v1,Var*& v2)
{
  return strcmp (v1->name (),v2->name ()) < 0;
}

void Etc::dump (const char* mask)
{
  logPrintf ("Dump variables (total_count=%u):",impl->map.size ());
  
  if (!mask)
    return;

  vector<Var*> vars;   

  for (VarMap::iterator i=impl->map.begin ();i!=impl->map.end ();i++)
    if (maskcmp (i->second->name (),mask))
      vars.push_back (i->second);
      
  sort (vars.begin (),vars.end (),compare_vars);    
      
  for (vector<Var*>::iterator i=vars.begin ();i!=vars.end ();i++)
    (*i)->dump ();
}
