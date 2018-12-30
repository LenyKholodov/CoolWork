#include <mesh.h>

struct Build
{
  DrawVertex* vert;
  Triangle*   face;
  int         vpos;

  inline void push_face (const vec3f& n) {
    vec3f s1 = vec3f (1.0f) - abs (n),
          s2 = rotatef ((float)M_PI/2.0f,n) * s1;          
    
    vert [0].pos = n + s1;
    vert [1].pos = n + s2;
    vert [2].pos = n - s1;
    vert [3].pos = n - s2;    

    for (int i=0;i<4;i++)
    {
      vert [i].n     = n;
      vert [i].color = 0.0f;
    }

    face [0].v [0] = vpos+3;
    face [0].v [1] = vpos+2;
    face [0].v [2] = vpos;
    face [1].v [0] = vpos+2;
    face [1].v [1] = vpos+1;
    face [1].v [2] = vpos;

    vert += 4;
    vpos += 4;
    face += 2;
  }
};

Surface* Primitives::CreateBox (float width,float height,float depth)
{
  Surface* surface = Surface::create (24,12);
  Build    build   = {surface->GetVertexes (),surface->GetTriangles (),0};
  
  build.push_face (vec3f (1,0,0));
  build.push_face (vec3f (-1,0,0));  
  build.push_face (vec3f (0,1,0));
  build.push_face (vec3f (0,-1,0));
  build.push_face (vec3f (0,0,1));
  build.push_face (vec3f (0,0,-1));  
  
  DrawVertex* verts = surface->GetVertexes ();  
  vec3f       size (width*0.5f,height*0.5f,depth*0.5f);
  
  for (int i=0;i<24;i++)
    verts [i].pos *= size;
  
  return surface;  
}
