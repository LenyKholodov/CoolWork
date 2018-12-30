#include <cmd.h>
#include <hash_map>
#include "misc.h"

using namespace std;
using namespace stdext;

struct Dummy
{
  typedef void (Dummy::*handler_fn)(CmdArgs&);
};

struct CommandDef //описание обработчика команды
{
  union
  {
    void*  obj;
    Dummy* dummy;
  };

  union {
    Dummy::handler_fn           class_handler;
    Command::static_cmd_handler static_handler;
    void*                       handler;
  };
};

typedef hash_map<string,CommandDef> CommandMap;

struct Command::Impl { //описание раелизации командного интерпретатора
  Etc        etc;      //пул переменных
  CommandMap map;      //карта команд    
  
  void set_var (CmdArgs&);
};

Command::Command ()
{
  impl = new Impl;
  
  bind ("set",*impl,&Impl::set_var);
}

Command::~Command ()
{
  delete impl;
}

Etc& Command::etc ()
{  
  return impl->etc;
}

void Command::_bind (const char* command,void* obj,void* handler)
{    
  if (!command)
    return;        

  CommandDef def;
  
  def.obj     = obj;
  def.handler = handler;
  
  impl->map [command] = def;
}

void Command::bind (const char* command,static_cmd_handler handler)
{    
  if (!command)
    return;

  CommandDef def;
  
  def.obj            = NULL;
  def.static_handler = handler;

  impl->map [command] = def;
}

void Command::unbind (const char* command)
{
  if (!command)
    return;
    
  CommandMap::iterator i = impl->map.find (command);
  
  if (i == impl->map.end ())
    return;
    
  impl->map.erase (i);  
}

void Command::exec (const char* command)
{
  CmdArgs args (*this,command);
  
  if (!args.tag ())
    return;
    
  CommandMap::iterator i = impl->map.find (args.tag ());
  
  if (i == impl->map.end ())
  {
    logPrintf ("command '%s' not found",args.tag ());
    return;
  }
  else logPrintf ("CommandDebug: '%s'",command);
  
  CommandDef& def = i->second;
  
  if (def.dummy) (def.dummy->*def.class_handler)(args);
  else           (def.static_handler)(args);
}

void Command::vexecf (const char* format,va_list list)
{
  char buf [256] = {0};    
  
  _vsnprintf (buf,sizeof (buf),format,list);
  
  exec (buf);
}
    
void Command::execf (const char* format,...)
{
  va_list list;
  
  va_start (list,format);  
  vexecf   (format,list);
}    

Command& Command::instance ()
{
  static Command command;
  
  return command;
}

void Command::Impl::set_var (CmdArgs& args)
{
  switch (args.argc ())
  {
    case 0:
      logPrintf ("set <var_name> [<value> ...]");
      break;
    case 1:
    {  
      Var* var = etc.find (args.gets (0));
      
      if (!var)
      {
        logPrintf ("Variable '%s' not found",args.gets (0));
        break;
      }
      
      var->dump ();
      
      break;      
    }
    case 2:
    {
      Var* var = etc.find (args.gets (0));
      
      if (!var)
      {
        logPrintf ("Variable '%s' not found",args.gets (0));
        break;
      }
            
      var->set (args.gets (1));
            
      break;
    }
    default:
    {
      Var* var = etc.find (args.gets (0));
      
      if (!var)
      {
        logPrintf ("Variable '%s' not found",args.gets (0));
        break;
      }
      
      vec3f v;
      
      v [0] = args.getf (1);
      v [1] = args.getf (2);
      v [2] = args.argc () >= 4 ? args.getf (3) : 0.0f;
            
      var->set (v);
            
      break;      
    }
  }  
}
