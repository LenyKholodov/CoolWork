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

static void build_volume (Build& build,bool half,float rr,float pr,size_t segments,size_t pipe_sides)
{
  float a1    = 0.0f,
  			a2		= 0.0f,
        da1   = (float)(half?M_PI:2.0f*M_PI) / float (segments),
        da2   = 2.0f * (float)M_PI / float (pipe_sides);
  
  DrawVertex* verts = build.verts;
  vec3f zn(0,0,-1);
  
  build.push_vertex (vec3f(rr,0,0),zn);
  build.push_vertex (vec3f(-rr,0,0),zn);
  
  int vbase0=build.vpos;
  
  float x = 1.0f,z = 0.0f;
  a2=0.0f;
  
  for (size_t j=0;j<pipe_sides+1;j++,a2+=da2)
  {
    float h = sin (a2);
    float r = cos (a2);
    vec3f v (x*(rr+r*pr),h*pr,z*(rr+r*pr));
    
    build.push_vertex (v,zn);
  }
  
  int vbase1=build.vpos;
  x = -1.0f;
  a2=0.0f;
  
  for (size_t j=0;j<pipe_sides+1;j++,a2+=da2)
  {
    float h = sin (a2);
    float r = cos (a2);
    vec3f v (x*(rr+r*pr),h*pr,z*(rr+r*pr));
    
    build.push_vertex (v,zn);
  }
  
  int vbase=build.vpos;
  
  for (size_t i=0;i<segments+1;i++,a1+=da1)
  {
    a2 = 0.0f;
    x = cos (a1);
    z = sin (a1);
    
    for (size_t j=0;j<pipe_sides+1;j++,a2+=da2)
    {
      float h = sin (a2);
      float r = cos (a2);
      vec3f v (x*(rr+r*pr),h*pr,z*(rr+r*pr));
      vec3f c (x*rr,0.0f,z*rr);
      
      build.push_vertex (v,normalize(v-c));
    }
  }

  for (size_t i=0;i<segments;i++)
    for (size_t j=0;j<pipe_sides;j++) {
      build.push_face((i+1)*(pipe_sides+1)+(j+1)+vbase,i*(pipe_sides+1)+(j+1)+vbase,i*(pipe_sides+1)+j+vbase);
      build.push_face(i*(pipe_sides+1)+j+vbase,(i+1)*(pipe_sides+1)+j+vbase,(i+1)*(pipe_sides+1)+(j+1)+vbase);
    }
  
  if (half) {
  	for (size_t j=0;j<pipe_sides;j++) {
      build.push_face(0,j+vbase0,j+1+vbase0);
      build.push_face(1,j+vbase1,j+1+vbase1);
	  }
	}
}

static Surface* _CreateTorus (float ring_radius,bool half,float pipe_radius,size_t segments,size_t pipe_sides)
{
  segments = max (3u,segments);
  pipe_sides = max (3u,pipe_sides);
  
  Surface* surface = Surface::create ((segments+3)*(pipe_sides+1)+2,2*(segments+2)*(pipe_sides+1));
  
  Build build = {surface->GetVertexes (),surface->GetTriangles (),vec3f (1.0f,0.0f,0.0f),0};
  
  build_volume (build,half,ring_radius,pipe_radius,segments,pipe_sides);
  
  size_t      vcount = surface->GetVertexesCount ();
  DrawVertex* vert   = surface->GetVertexes ();
  
  return surface;
}

Surface* Primitives::CreateTorus (float ring_radius,float pipe_radius,size_t parallels,size_t meridians)
{
  return _CreateTorus (ring_radius,false,pipe_radius,parallels,meridians);
}

Surface* Primitives::CreateHalfTorus (float ring_radius,float pipe_radius,size_t parallels,size_t meridians)
{
  return _CreateTorus (ring_radius,true,pipe_radius,parallels,meridians);
}
