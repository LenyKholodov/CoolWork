#include <cmd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { MAX_ARG_COUNT = 32, BUF_SIZE = 1024 };

struct CmdArgs::Impl {          //описание реализации аргументов команды
  char   buf  [BUF_SIZE];       //буфер с аргументами
  char*  argv [MAX_ARG_COUNT];  //указатели на аргументы
  size_t argc;                  //количество аргументов
  size_t pos;                   //смещение
  
  void parse ();
};

CmdArgs::CmdArgs (Command& cmd)
  : command (cmd), etc (cmd.etc ())
{
  impl = new Impl;

  reset ();
}

CmdArgs::CmdArgs (Command& cmd,const char* command)
  : command (cmd), etc (cmd.etc ())
{
  impl = new Impl;
  
  parse (command);
}

CmdArgs::~CmdArgs ()
{
  delete impl;
}

size_t CmdArgs::argc () const
{
  size_t count = impl->argc-impl->pos-1;

  return count < impl->argc ? count : 0;
}

const char* CmdArgs::tag () const
{
  return impl->argc ? impl->argv [0] : NULL;
}

const char* CmdArgs::gets (size_t index) const
{
  return index + impl->pos + 1 < impl->argc ? impl->argv [index+impl->pos+1] : NULL;
}

float CmdArgs::getf (size_t index) const
{
  const char* s = gets (index);
  
  return s ? (float)atof (s) : 0.0f;
}

int CmdArgs::geti (size_t index) const
{
  const char* s = gets (index);
  
  return s ? atoi (s) : 0;
}

size_t CmdArgs::getui (size_t index) const
{
  return (size_t)geti (index);
}

void CmdArgs::reset ()
{
  *impl->buf = 0;
  impl->argc = 0;
  impl->pos  = 0;
}

void CmdArgs::shift (int offset)
{
  impl->pos += offset;
}

static char* nextword (char*& s)
{
  s += strspn (s," \t\r");

  char* first = s, *t = " \t\r";

  switch (*s)
  {
    case '\'': 
      first = ++s; 
      t     = "'";
      break;
    case '"':  
      first = ++s;
      t     = "\"";
      break;
    case '/':
      if (s [1] == '/')
        return NULL;
      break;
    case '\0':
      return NULL;
  }

  char* last = strpbrk (s,t);

  if (last)
  {
    s    = last;
    *s++ = 0;
  }  
  else s = s + strlen (s);

  return first;  
}

void CmdArgs::Impl::parse ()
{
  for (char* s=buf,**i=argv,**end=argv+MAX_ARG_COUNT;i != end && (*i=nextword(s));i++);

  argc = i-argv;
}

void CmdArgs::parse (const char* command)
{
  reset ();

  if (!command)
    return;

  strncpy (impl->buf,command,sizeof (impl->buf));
  impl->parse ();
}

void CmdArgs::vparsef (const char* fmt,va_list list)
{
  reset ();

  if (!fmt)
    return ;

  _vsnprintf  (impl->buf,sizeof (impl->buf),fmt,list);
  impl->parse ();
}

void CmdArgs::parsef (const char* fmt,...)
{
  va_list list;

  va_start (list,fmt);
  vparsef  (fmt,list);
}
