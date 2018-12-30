#include <mesh.h>
#include <vector>

using namespace std;

typedef vector<DrawVertex> VertexArray;
typedef vector<Triangle>   TriangleArray;

struct Surface::Impl {         //описание реализации поверхности мэша  
  VertexArray    verts;        //вершины
  TriangleArray  tris;         //треугольники
  AABB           bound_box;    //ограничивающий ящик
  bool           dirty_bounds; //флаг "грязности" ограничивающих тел
};

Surface::Surface (size_t vcount,size_t tcount)
{
  impl = new Impl;
  
  impl->verts.resize (vcount);
  impl->tris.resize (tcount);
  impl->dirty_bounds = true;
}

Surface::~Surface ()
{
  delete impl;
}

void Surface::release ()
{
  delete this;
}
  
size_t Surface::GetVertexesCount ()
{
  return impl->verts.size ();
}

DrawVertex* Surface::GetVertexes ()
{
  return impl->verts.size () ? &impl->verts [0] : NULL;
}

size_t Surface::GetTrianglesCount ()
{
  return impl->tris.size ();
}

Triangle* Surface::GetTriangles ()
{
  return impl->tris.size () ? &impl->tris [0] : NULL;
}

void Surface::transform (const mat4f& tm)
{
  size_t      vcount = impl->verts.size ();
  DrawVertex* vert   = &impl->verts [0];

  for (size_t i=0;i<vcount;i++,vert++)
  {
    vert->pos = tm * vert->pos;
    vert->n   = tm * vec4f (vert->n,0.0f);
  }
  
  impl->dirty_bounds = true;
}

void Surface::ComputeBounds ()
{
  size_t vcount = impl->verts.size ();
  vec3f  min = 0.0f, max = 0.0f;
         
  if (vcount) 
  {
    min = max = impl->verts[0].pos;
    
    DrawVertex* v = &impl->verts [1];
    
    for (size_t i=1;i<vcount;i++,v++) 
    {
      for (int j=0;j<3;j++)
        if      (v->pos [j] < min [j]) min [j] = v->pos [j];
        else if (v->pos [j] > max [j]) max [j] = v->pos [j];
    }
  }

  impl->bound_box.min = min;
  impl->bound_box.max = max;    
  impl->dirty_bounds  = false;
}

const AABB& Surface::GetAABB ()
{
  if (impl->dirty_bounds)
    ComputeBounds ();
 
  return impl->bound_box;  
}

Surface* Surface::create (size_t vertexes_count,size_t triangles_count)
{
  return new Surface (vertexes_count,triangles_count);
}
