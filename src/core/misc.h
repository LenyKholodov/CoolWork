#ifndef __COOLWORK_CORE_MISC__
#define __COOLWORK_CORE_MISC__

#include <stdlib.h>
#include <mathlib.h>
#include <string>

/*
    Утилиты
*/

void print (const mat4f& m);

template <class T,size_t size>
vec<T,size> vrand (const vec<T,size>& min,const vec<T,size>& max)
{
  vec<T,size> res = min;
  
  for (size_t i=0;i<size;i++)
    res [i] += float (::rand ()) / float (RAND_MAX) * (max [i] - min [i]);
    
  return res;
}

/*
    Протоколирование
*/

void logPrintf (const char*,...);

/*
    Математика
*/

void affine_decomp (const mat4f& tm,vec3f& pos,vec3f& rot,vec3f& scale);

/*
    Сравнение по маске (wildcard)
*/

bool maskcmp (const char* s,const char* mask);

/*
    Блокирование сообщений во внешнюю среду
*/

void CoreLockUpdate ();
void CoreUnlockUpdate ();

/*
    Сообщение о смене манипуляторов
*/

void CoreChangeManipulator (const char* name);

/*
    Вспомогательные функции получения имени файла при загрузке
*/

std::string CoreGetOpenFilename (const char* dir);
std::string CoreGetSaveFilename ();
std::string CoreGetSaveAsFilename (const char* dir);
void        CoreSetCurrentFilename (const char* name);

#endif
