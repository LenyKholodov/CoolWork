#include <mesh.h>
#include <algorithm>

using namespace std;

struct Build
{
  DrawVertex* verts;
  Triangle*   tris;
  vec3f       color;
  int         vpos;

  inline int push_vertex (const vec3f& pos,const vec3f& n)
  {
    verts->pos   = pos;
    verts->n     = n;
    verts->color = color;
    verts++;
    return vpos++;
  }

  inline void push_face (int v1,int v2,int v3)
  {
    tris->v [0] = v1;
    tris->v [1] = v2;
    tris->v [2] = v3;
    tris++;
  }
};

static void build_volume (Build& build,bool half,size_t parallels,size_t meridians)
{
  float angle = half ? (float)M_PI : 2.0f * (float)M_PI;
  float a1    = 0.0f,
        da1   = angle / float (meridians-1),
        da2   = (float)M_PI / float (parallels+2);
        
  DrawVertex* verts = build.verts;        
        
  build.push_vertex (vec3f (0.0f,1.0f,0.0f),vec3f (0.0f,1.0f,0.0f));
  build.push_vertex (vec3f (0.0f,-1.0f,0.0f),vec3f (0.0f,-1.0f,0.0f));
        
  int vbase = build.vpos;
        
  for (size_t i=0;i<meridians;i++,a1+=da1)
  {
    float a2 = da2;
    
    float x = cos (a1),
          z = sin (a1);
    
    for (size_t j=0;j<parallels;j++,a2+=da2)
    {
      float r = sin (a2);
      vec3f v (r*x,cos (a2),r*z);
      
      build.push_vertex (v,v);
    }        
  }
  
  for (size_t i=0;i<meridians;i++) 
  {
    int v1 = vbase + i * parallels,
        v2 = vbase + (i+1) % meridians * parallels;
        
    build.push_face (0,v1,v2);
    build.push_face (1,v2+parallels-1,v1+parallels-1);
    
    for (size_t j=0;j<parallels-1;j++)
    {            
      build.push_face (v1+j,v1+j+1,v2+j+1);
      build.push_face (v1+j,v2+j+1,v2+j);
    }
  }
  
  if (half)
  {
    int v1 = vbase,
        v2 = vbase + parallels*(meridians-1);
    
    for (size_t j=0;j<parallels;j++)
      verts [v1+j].n = verts [v2+j].n = vec3f (0.0f,0.0f,-1.0f);
  }
}

static Surface* _CreateSphere (float radius,bool half,size_t parallels,size_t meridians)
{
  if (half)
    meridians /= 2;

  parallels = max (1u,parallels);
  meridians = max (2u,meridians);
  
  Surface* surface = Surface::create (parallels*meridians+2,2*(parallels+1)*meridians);
  
  Build build = {surface->GetVertexes (),surface->GetTriangles (),vec3f (1.0f,0.0f,0.0f),0};
  
  build_volume (build,half,parallels,meridians);
  
  size_t      vcount = surface->GetVertexesCount ();
  DrawVertex* vert   = surface->GetVertexes ();
  
  for (size_t i=0;i<vcount;i++,vert++)
    vert->pos *= radius;
  
  return surface;
}

Surface* Primitives::CreateSphere (float radius,size_t parallels,size_t meridians)
{
  return _CreateSphere (radius,false,parallels,meridians);
}

Surface* Primitives::CreateHalfSphere (float radius,size_t parallels,size_t meridians)
{
  return _CreateSphere (radius,true,parallels,meridians);
}
