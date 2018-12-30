#include <mesh.h>
#include <math.h>
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

static void build_bases (Build& build,float r1,float r2,float h,bool half,int segments)
{
  float da = (float)(half?M_PI:2.0f*M_PI) / float (segments),
        a  = 0.0f;

  vec3f nt(0,1,0),nb(0,-1,0);

  int v = build.push_vertex(vec3f(0,h,0),nt);
  
  for (int i=0;i<segments+1;i++,a+=da) 
    build.push_vertex (vec3f (r1*cos (a),h,r1*sin (a)),nt);

  for (int i=0;i<segments;i++)
    build.push_face (v,v+i+1,v+i+2);
    
  v = build.push_vertex(vec3f(0,0,0),nb);
  a=0.0f;

  for (int i=0;i<segments+1;i++,a+=da) 
    build.push_vertex (vec3f (r2*cos (a),0,r2*sin (a)),nb);

  for (int i=0;i<segments;i++)
    build.push_face (v,v+i+2,v+i+1);
}

static void build_sidewall (Build& build,float r1,float r2,float h,int segments)
{
  int vbase = build.vpos;
  
  vec3f n(0,0,-1);
  
  build.push_vertex(vec3f(r1,h,0),n);
  build.push_vertex(vec3f(r2,0,0),n);

  build.push_vertex(vec3f(-r1,h,0),n);
  build.push_vertex(vec3f(-r2,0,0),n);
  
  build.push_face(vbase+1,vbase,vbase+2);
  build.push_face(vbase+2,vbase+3,vbase+1);
}

static void build_volume (Build& build,float r1,float r2,float h,bool half,int segments)
{
  float da = (float)(half?M_PI:2.0f*M_PI) / float (segments),
        a  = 0.0f;
  float x,z;
  
  int vbase=build.vpos;
  
  vec2f n0(normalize(vec2f(h,r2-r1)));
  
  for (int i=0;i<segments+1;i++,a+=da) {
    x=cos(a);
    z=sin(a);
    
    build.push_vertex(vec3f(x*r2,0,z*r2),normalize(vec3f(x*n0.x,n0.y,z*n0.x)));
    build.push_vertex(vec3f(x*r1,h,z*r1),normalize(vec3f(x*n0.x,n0.y,z*n0.x)));
  }
  
  for (int i=0;i<segments;i++) {
    build.push_face(vbase+i*2,vbase+i*2+3,vbase+i*2+1);
    build.push_face(vbase+i*2,vbase+i*2+2,vbase+i*2+3);
  }
}

static Surface* CreateConeBasedPrimitive (float height,float long_radius,float short_radius,bool half,size_t segments)
{
  if (half)
    segments /= 2;

  int   vcount = (segments+1) * 4 + 6,
        fcount = segments * 4 + 2;

  Surface* surface = Surface::create (vcount,fcount);

  Build build = {surface->GetVertexes (),surface->GetTriangles (),vec3f (1.0f,0.0f,0.0f),0};
  
  build_bases   (build,short_radius,long_radius,height,half,segments);
  build_volume (build,short_radius,long_radius,height,half,segments);
  if (half)
    build_sidewall(build,short_radius,long_radius,height,segments);
  
  return surface;
}

Surface* Primitives::CreateClipCone (float height,float long_radius,float short_radius,size_t segments)
{
//  short_radius = min (max (0.0f,short_radius),long_radius);
  
  return CreateConeBasedPrimitive (height,long_radius,short_radius,false,segments);
}

Surface* Primitives::CreateHalfClipCone (float height,float long_radius,float short_radius,size_t segments)
{  
  return CreateConeBasedPrimitive (height,long_radius,short_radius,true,segments);
}

Surface* Primitives::CreateCone (float radius,float height,size_t segments)
{
  return CreateClipCone (height,radius,radius,segments);
}

Surface* Primitives::CreateHalfCone (float radius,float height,size_t segments)
{
  return CreateHalfClipCone (height,radius,radius,segments);
}

Surface* Primitives::CreateCylinder (float radius,float height,size_t segments)
{
  return CreateConeBasedPrimitive (height,radius,radius,false,segments);
}

Surface* Primitives::CreateHalfCylinder (float radius,float height,size_t segments)
{
  return CreateConeBasedPrimitive (height,radius,radius,true,segments);
}
