#ifndef __COOLWORK_CORE_DRAW_STUFF__
#define __COOLWORK_CORE_DRAW_STUFF__

#include <stdarg.h>
#include <windows.h>
#include <glew.h>
#include <openglut.h>
#include <gl/glu.h>
#include <render.h>

/*
    Вывод текста
*/

void rOutText  (void* font,const char*,...);
void rVOutText (void* font,const char*,va_list);

/*
    Рисование сцены
*/

enum SceneDrawFlags {
  STUFF_DRAW_OBJECTS = 1,
  STUFF_DRAW_LIGHTS  = 2,
  STUFF_DRAW_HELPERS = 4,
  STUFF_DRAW_ALL     = STUFF_DRAW_OBJECTS|STUFF_DRAW_LIGHTS|STUFF_DRAW_HELPERS
};

void rDrawScene (Viewport*,Scene*,Camera*,size_t flags=STUFF_DRAW_ALL);

/*
    Рисование хэлперов
*/

void rDrawPoint       (const vec3f& pos,const vec3f& color);
void rDrawArrow       (const vec3f& pos,const vec3f& dir,const vec3f& color,const char* msg=NULL);
void rDrawRect        (const vec3f& pos,const vec3f& dir,const vec3f& color);
void rDrawAxes        ();
void rDrawArc         (const vec3f& pos,const vec3f& dir, const vec3f& color,int steps);
void rDrawSector      (const vec3f& pos,const vec3f& dir, const vec3f& color, int steps, float angle);
void rDrawCircle      (const vec3f& pos,const vec3f& dir, const vec3f& color,int steps);
void rDrawLine        (const vec3f& from,const vec3f& to,const vec3f& color);
void rDrawGrid        (Viewport*);
void rDrawBoundBox    (const vec3f&,const vec3f&,const vec3f&);
void rDrawBoundSphere (const vec3f&,float,const vec3f&,int);

/*
    Карта высот
*/

struct HeightMap
{
  size_t width, height;
  float* map;
};

#endif
