#include <mesh.h>
#include <algorithm>

using namespace std;

typedef enum {
  Tetrahedron,
  Hexahedron,
  Octahedron,
  Dodecahedron,
  Icosahedron
} PlatonicShapes;

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

static void build_volume(Build& build,PlatonicShapes shape) {
  const float _t=0.70710678118654752f;//0.866025403784438645f;//1.73205080756887729f;
  const float _h=0.70710678118654752f;
  //const float _da=0.61803398874989484820458f,_db=1.61803398874989484820458f;
  const float _da=0.30901699437494742410229f,_db=0.80901699437494742410229f,_dc=0.5f;
  const float _a=0.525731112119133606f,_b=0.850650808352039932f;
  switch (shape) {
  
  case Tetrahedron: //?
    build.push_vertex(vec3f(_t,_t,_t),vec3f(_t,_t,_t));
    build.push_vertex(vec3f(_t,-_t,-_t),vec3f(_t,-_t,-_t));
    build.push_vertex(vec3f(-_t,_t,-_t),vec3f(-_t,_t,-_t));
    build.push_vertex(vec3f(-_t,-_t,_t),vec3f(-_t,-_t,_t));
    
    build.push_face(3,1,0);
    build.push_face(0,1,2);
    build.push_face(0,2,3);
    build.push_face(3,2,1);
    break;

  case Hexahedron: //+
    build.push_vertex(vec3f(_h,_h,_h),vec3f(_h,_h,_h));
    build.push_vertex(vec3f(_h,_h,-_h),vec3f(_h,_h,-_h));
    build.push_vertex(vec3f(_h,-_h,_h),vec3f(_h,-_h,_h));
    build.push_vertex(vec3f(_h,-_h,-_h),vec3f(_h,-_h,-_h));

    build.push_vertex(vec3f(-_h,_h,_h),vec3f(-_h,_h,_h));
    build.push_vertex(vec3f(-_h,_h,-_h),vec3f(-_h,_h,-_h));
    build.push_vertex(vec3f(-_h,-_h,_h),vec3f(-_h,-_h,_h));
    build.push_vertex(vec3f(-_h,-_h,-_h),vec3f(-_h,-_h,-_h));

    build.push_face(0,4,5);
    build.push_face(5,1,0);
    build.push_face(5,7,3);
    build.push_face(3,1,5);
    build.push_face(0,1,3);
    build.push_face(3,2,0);
    build.push_face(0,2,6);
    build.push_face(6,4,0);
    build.push_face(4,5,7);
    build.push_face(6,7,4);
    build.push_face(2,3,7);
    build.push_face(7,6,2);
    break;
  case Octahedron: //+
    build.push_vertex(vec3f(1.0f,0.0f,0.0f), vec3f(1.0f,0.0f,0.0f));
    build.push_vertex(vec3f(-1.0f,0.0f,0.0f),vec3f(-1.0f,0.0f,0.0f));
    build.push_vertex(vec3f(0.0f,1.0f,0.0f), vec3f(0.0f,1.0f,0.0f));
    build.push_vertex(vec3f(0.0f,-1.0f,0.0f),vec3f(0.0f,-1.0f,0.0f));
    build.push_vertex(vec3f(0.0f,0.0f,1.0f), vec3f(0.0f,0.0f,1.0f));
    build.push_vertex(vec3f(0.0f,0.0f,-1.0f),vec3f(0.0f,0.0f,-1.0f));
    
    build.push_face(0, 4, 2);
    build.push_face(1, 2, 4);
    build.push_face(0, 3, 4);
    build.push_face(1, 4, 3);
    build.push_face(0, 2, 5);
    build.push_face(1, 5, 2);
    build.push_face(0, 5, 3);
    build.push_face(1, 3, 5);
    break;
  case Dodecahedron:
    build.push_vertex(vec3f(-_da,0,_db ),vec3f(-_da,0,_db ));
    build.push_vertex(vec3f(_da,0,_db  ),vec3f(_da,0,_db  ));
    build.push_vertex(vec3f(-_dc,-_dc,-_dc   ),vec3f(-_dc,-_dc,-_dc   ));
    build.push_vertex(vec3f(-_dc,-_dc,_dc    ),vec3f(-_dc,-_dc,_dc    ));
    build.push_vertex(vec3f(-_dc,_dc,-_dc    ),vec3f(-_dc,_dc,-_dc    ));
    build.push_vertex(vec3f(-_dc,_dc,_dc     ),vec3f(-_dc,_dc,_dc     ));
    build.push_vertex(vec3f(_dc,-_dc,-_dc    ),vec3f(_dc,-_dc,-_dc    ));
    build.push_vertex(vec3f(_dc,-_dc,_dc     ),vec3f(_dc,-_dc,_dc     ));
    build.push_vertex(vec3f(_dc,_dc,-_dc     ),vec3f(_dc,_dc,-_dc     ));
    build.push_vertex(vec3f(_dc,_dc,_dc      ),vec3f(_dc,_dc,_dc      ));
    build.push_vertex(vec3f(_db,_da,0  ),vec3f(_db,_da,0  ));
    build.push_vertex(vec3f(_db,-_da,0 ),vec3f(_db,-_da,0 ));
    build.push_vertex(vec3f(-_db,_da,0 ),vec3f(-_db,_da,0 ));
    build.push_vertex(vec3f(-_db,-_da,0),vec3f(-_db,-_da,0));
    build.push_vertex(vec3f(-_da,0,-_db),vec3f(-_da,0,-_db));
    build.push_vertex(vec3f(_da,0,-_db ),vec3f(_da,0,-_db ));
    build.push_vertex(vec3f(0,_db,_da  ),vec3f(0,_db,_da  ));
    build.push_vertex(vec3f(0,_db,-_da ),vec3f(0,_db,-_da ));
    build.push_vertex(vec3f(0,-_db,_da ),vec3f(0,-_db,_da ));
    build.push_vertex(vec3f(0,-_db,-_da),vec3f(0,-_db,-_da));
    
    build.push_face(9, 1,  0  );
    build.push_face(16,9,  0  );
    build.push_face(5, 16, 0  );
    build.push_face(3, 0,  1  );
    build.push_face(18,3,  1  );
    build.push_face(7, 18, 1  );
    build.push_face(11,7,  1  );
    build.push_face(10,11, 1  );
    build.push_face(9, 10, 1  );
    build.push_face(18, 7, 11 );
    build.push_face(19, 18, 11);
    build.push_face(6, 19, 11 );
    build.push_face(16,17, 8  );
    build.push_face(9, 16, 8  );
    build.push_face(10,9,  8  );
    build.push_face(15,14, 2  );
    build.push_face(6, 15, 2  );
    build.push_face(19,6,  2  );
    build.push_face(12,13, 2  );
    build.push_face(4, 12, 2  );
    build.push_face(14,4,  2  );
    build.push_face(18,19, 2  );
    build.push_face(3, 18, 2  );
    build.push_face(13,3,  2  );
    build.push_face(5, 0,  3  );
    build.push_face(12,5,  3  );
    build.push_face(13,12, 3  );
    build.push_face(8, 15, 6  );
    build.push_face(10,8,  6  );
    build.push_face(11,10, 6  );
    build.push_face(8, 17, 4  );
    build.push_face(15,8,  4  );
    build.push_face(14,15, 4  );
    build.push_face(5, 12, 4  );
    build.push_face(16,5,  4  );
    build.push_face(17,16, 4  );
    break;
  case Icosahedron:
    build.push_vertex(vec3f(-_a, 0.0f, _b ),vec3f(-_a, 0.0f, _b ));
    build.push_vertex(vec3f(_a, 0.0f, _b  ),vec3f(_a, 0.0f, _b  ));
    build.push_vertex(vec3f(-_a, 0.0f, -_b),vec3f(-_a, 0.0f, -_b));
    build.push_vertex(vec3f(_a, 0.0f, -_b ),vec3f(_a, 0.0f, -_b ));
    build.push_vertex(vec3f(0.0f, _b, _a  ),vec3f(0.0f, _b, _a  ));
    build.push_vertex(vec3f(0.0f, _b, -_a ),vec3f(0.0f, _b, -_a ));
    build.push_vertex(vec3f(0.0f, -_b, _a ),vec3f(0.0f, -_b, _a ));
    build.push_vertex(vec3f(0.0f, -_b, -_a),vec3f(0.0f, -_b, -_a));
    build.push_vertex(vec3f(_b, _a, 0.0f  ),vec3f(_b, _a, 0.0f  ));
    build.push_vertex(vec3f(-_b, _a, 0.0f ),vec3f(-_b, _a, 0.0f ));
    build.push_vertex(vec3f(_b, -_a, 0.0f ),vec3f(_b, -_a, 0.0f ));
    build.push_vertex(vec3f(-_b, -_a, 0.0f),vec3f(-_b, -_a, 0.0f));
    
    build.push_face(0, 4, 1);
    build.push_face(0, 9, 4);
    build.push_face(9, 5, 4);
    build.push_face(4, 5, 8);
    build.push_face(4, 8, 1);
    build.push_face(8, 10, 1);
    build.push_face(8, 3, 10);
    build.push_face(5, 3, 8);
    build.push_face(5, 2, 3);
    build.push_face(2, 7, 3);
    build.push_face(7, 10, 3);
    build.push_face(7, 6, 10);
    build.push_face(7, 11, 6);
    build.push_face(11, 0, 6);
    build.push_face(0, 1, 6);
    build.push_face(6, 1, 10);
    build.push_face(9, 0, 11);
    build.push_face(9, 11, 2);
    build.push_face(9, 2, 5);
    build.push_face(7, 2, 11);
    break;
  }
}

static Surface* _CreatePlatonicShape (float radius,PlatonicShapes shape)
{
  const int verts_count[5]={4,8,6,20,12},faces_count[5]={4,12,8,36,20};
  Surface* surface = Surface::create (verts_count[shape],faces_count[shape]);
  
  Build build = {surface->GetVertexes (),surface->GetTriangles (),vec3f (1.0f,0.0f,0.0f),0};
  
  build_volume(build,shape);

  size_t      vcount = surface->GetVertexesCount ();
  DrawVertex* vert   = surface->GetVertexes ();
  
  for (size_t i=0;i<vcount;i++,vert++)
    vert->pos *= radius;
  
  return surface;
}

Surface* Primitives::CreateTetrahedron (float radius)
{
  return _CreatePlatonicShape (radius,Tetrahedron);
}

Surface* Primitives::CreateHexahedron (float radius)
{
  return _CreatePlatonicShape (radius,Hexahedron);
}

Surface* Primitives::CreateOctahedron (float radius)
{
  return _CreatePlatonicShape (radius,Octahedron);
}

Surface* Primitives::CreateDodecahedron (float radius)
{
  return _CreatePlatonicShape (radius,Dodecahedron);
}

Surface* Primitives::CreateIcosahedron (float radius)
{
  return _CreatePlatonicShape (radius,Icosahedron);
}
