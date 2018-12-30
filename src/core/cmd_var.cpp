#include <etc.h>
#include <stdlib.h>
#include <string>
#include "expression.h"

using namespace std;

struct Dummy 
{ 
  typedef void (Dummy::*OnUpdateHandler)(Var&); 
};

struct Var::Impl {               //описание реализации управл€ющей переменной
  Etc&        etc;               //окружение, которому принадлежит переменна€
  string      name;              //им€ переменной
  string      str_value [3];     //строковое значение переменной  
  Expression  expression;        //выражение дл€ вычислений
  bool        is_dynamic;        //динамическое выражение (есть вычислен€ по формуле)
  bool        is_class_callback; //флаг, сигнализирующий о необходимости вызова метода класса
  bool        is_in_update;      //дл€ избегани€ рекурсии в функци€х update (обход повторной входимости)
  size_t      max_string_size;   //максимальный размер устанавливаемого буфера ссылки (только дл€ bind (const char*))
  union {                        //указатель на объект
    Dummy*    dummy;
    void*     obj;
    int*      integer_link;
    float*    float_link;
    vec3f*    vec3f_link;
    char*     string_link;
  };
  union {                     //обработчик установки переменной
    Dummy::OnUpdateHandler class_on_set_handler;
    Var::OnUpdateHandler   static_on_set_handler;
    void*                  on_set_handler;
  };
  union {                     //обработчик чтени€ переменной
    Dummy::OnUpdateHandler class_on_get_handler;
    Var::OnUpdateHandler   static_on_get_handler;
    void*                  on_get_handler;
  };
  
  Impl (Etc& _etc,Var& var) : etc (_etc), expression (var) {}
  
  static void set_initeger (Var&);
  static void get_initeger (Var&);
  static void set_float    (Var&);
  static void get_float    (Var&);
  static void set_string   (Var&);
  static void get_string   (Var&);
  static void set_vec3f    (Var&);
  static void get_vec3f    (Var&);
  
  void set (const char*);
  void set (int);
  void set (float);
  void set (const vec3f&);  
  
  const char* gets  (size_t i=0);
  int         geti  ();
  float       getf  ();
  vec3f       get3f ();
  
  void on_set (Var&);
  void on_get (Var&);
};

Var::Var (Etc& etc,const char* name)
{
  impl = new Impl (etc,*this);
  
  impl->name            = name ? name : "";
  impl->max_string_size = 0;
  impl->is_in_update    = false;
  impl->is_dynamic      = false;

  unbind ();
}

Var::~Var ()
{
  delete impl;
}

Etc& Var::etc ()
{
  return impl->etc;
}

const char* Var::name ()
{
  return impl->name.c_str ();
}

void Var::Impl::on_set (Var& var)
{
  if (is_in_update)
    return;
    
  is_in_update = true;           
  is_dynamic   = false;  
  
  expression.set ("");

  var.update ();              

  if (on_set_handler && dummy)
  {
    if (is_class_callback) (dummy->*class_on_set_handler) (var);
    else                   static_on_set_handler (var);
  }
  
  is_in_update = false;
}

void Var::Impl::on_get (Var& var)
{
  if (is_in_update)
    return;

  is_in_update = true;

  if (is_dynamic)
  {        
    vec3f value = expression.compute ();
        
    for (int i=0;i<3;i++)
    {
      char buf [32];

      _snprintf (buf,sizeof (buf),"%f",value [i]);

      str_value [i] = buf;
    }
    
    if (on_set_handler && dummy)
    {
      if (is_class_callback) (dummy->*class_on_set_handler) (var);
      else                   static_on_set_handler (var);
    }    
  }
  
  if (on_get_handler && dummy)
  {        
    if (is_class_callback) (dummy->*class_on_get_handler) (var);
    else                   static_on_get_handler (var);    
  }    

  is_in_update = false;      
}

inline const char* Var::Impl::gets (size_t i)
{
  return str_value [i].size () ? str_value [i].c_str () : "";
}

inline int Var::Impl::geti ()
{
  return atoi (gets ());
}

inline float Var::Impl::getf ()
{
  return (float)atof (gets ());
}

inline vec3f Var::Impl::get3f ()
{
  return vec3f ((float)atof (gets (0)),(float)atof (gets (1)),(float)atof (gets (2)));
}

inline void Var::Impl::set (const char* value)
{
  if (!value)
    value = "";
      
  str_value [0] = str_value [1] = str_value [2] = value;  
}

inline void Var::Impl::set (int value)
{
  char buf [16];
  
  _snprintf (buf,sizeof (buf),"%d",value);
  
  set (buf);
}

inline void Var::Impl::set (float value)
{
  char buf [16];
  
  _snprintf (buf,sizeof (buf),"%f",value);
  
  set (buf);
}

inline void Var::Impl::set (const vec3f& value)
{
  for (int i=0;i<3;i++)
  {
    char buf [32];

    _snprintf (buf,sizeof (buf),"%f",value [i]);

    str_value [i] = buf;
  }
}

const char* Var::gets ()
{
  impl->on_get (*this);
  return impl->gets ();
}

int Var::geti ()
{
  impl->on_get (*this);
  return impl->geti ();
}

float Var::getf ()
{
  impl->on_get (*this);
  return impl->getf ();  
}

vec3f Var::get3f ()
{  
  impl->on_get (*this);    
  return impl->get3f ();
}

void Var::set (const char* value)
{
  impl->set (value);
  impl->on_set (*this);
}

void Var::set (int value)
{
  impl->set (value);
  impl->on_set (*this);  
}

void Var::set (float value)
{
  impl->set (value);
  impl->on_set (*this);    
}

void Var::set (const vec3f& value)
{
  impl->set (value);  
  impl->on_set (*this);  
}

void Var::Impl::set_initeger (Var& var)
{  
  *var.impl->integer_link = var.impl->geti ();
}

void Var::Impl::get_initeger (Var& var)
{  
  var.impl->set (*var.impl->integer_link);
}

void Var::Impl::set_float (Var& var) 
{  
  *var.impl->float_link = var.impl->getf ();
}

void Var::Impl::get_float (Var& var)
{  
  var.impl->set (*var.impl->float_link);
}

void Var::Impl::set_vec3f (Var& var)
{  
  *var.impl->vec3f_link = var.impl->get3f ();
}

void Var::Impl::get_vec3f (Var& var)
{  
  var.impl->set (*var.impl->vec3f_link);
}

void Var::Impl::set_string (Var& var)
{    
  strncpy (var.impl->string_link,var.impl->gets (),var.impl->max_string_size);
}

void Var::Impl::get_string (Var& var)
{    
  var.impl->set (var.impl->string_link);
}

void Var::bind (char* buf,size_t buf_size)
{
  if (!buf || !buf_size)
  {
    unbind ();
    return;
  }

  bind_handler (Impl::set_string,Impl::get_string);
  
  impl->obj             = buf;
  impl->max_string_size = buf_size;
}

void Var::bind (int& link)
{
  if (!&link)
  {
    unbind ();
    return;
  }

  bind_handler (Impl::set_initeger,Impl::get_initeger);
  
  impl->obj = &link;
}

void Var::bind (float& link)
{
  if (!&link)
  {
    unbind ();
    return;
  }

  bind_handler (Impl::set_float,Impl::get_float);
  
  impl->obj = &link;
}

void Var::bind (vec3f& link)
{
  if (!&link)
  {
    unbind ();
    return;
  }

  bind_handler (Impl::set_vec3f,Impl::get_vec3f);
  
  impl->obj = &link;
}

void Var::_bind_handler (void* obj,void* onSet,void* onGet)
{
  if (!obj)
  {
    unbind ();
    return;
  }

  impl->is_class_callback = true;
  impl->obj               = obj;
  impl->on_set_handler    = onSet;
  impl->on_get_handler    = onGet;
}

void Var::bind_handler (OnUpdateHandler onSet,OnUpdateHandler onGet)
{
  impl->is_class_callback = false;
  impl->obj               = NULL;
  impl->on_set_handler    = onSet;
  impl->on_get_handler    = onGet;
}

void Var::unbind ()
{
  impl->is_class_callback = false;
  impl->obj               = NULL;
  impl->on_set_handler    = NULL;
  impl->on_get_handler    = NULL;
}

void Var::set_expression (const char* expression)
{
  impl->is_dynamic = expression && *expression;
  impl->expression.set (expression);
  update ();
}

const char* Var::get_expression ()
{
  return impl->expression.source ();
}

void Var::update ()
{
  impl->etc.update (name ());
}

static bool is_value (const char* s)
{    
  for (;*s;s++)
    if (!isdigit (*s) && *s != '.' && *s != '-' && *s != '+')
      return false;
      
  return true;      
}

void Var::dump ()
{
  update ();

  if (is_value (impl->str_value [0].c_str ()))
  {
    if (is_value (impl->str_value [1].c_str ()) && is_value (impl->str_value [2].c_str ()))
    {
      vec3f v = get3f ();
      
      logPrintf ("\t%s=(%g,%g,%g)",name (),v.x,v.y,v.z);
    }    
    else logPrintf ("\t%s=%g",name (),getf ());
  }
  else logPrintf ("\t%s='%s'",name (),gets ());
}
