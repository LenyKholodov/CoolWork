#include <math.h>
#include "manipulators.h"

using namespace std;

enum MeshType {
  TETRAHEDRON,
  HEXAHEDRON,
  OCTAHEDRON,
  DODECAHEDRON,
  ICOSAHEDRON      
};

class CreatePlatonicManipulator: public Manipulator
{ 
  public:

    struct Error {};

    CreatePlatonicManipulator (RenderCursor& cursor, MeshType type) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      platonic    = NULL;
      scene       = cursor.GetActiveScene ();
      mesh_type   = type;
                
      if (!scene)
        throw Error ();
    }
    
    ~CreatePlatonicManipulator ()
    {
      if (platonic)
        platonic->release();
    }

    void select ()
    {      
      if (platonic)
        return;

      platonic = scene->CreateModel();
      
      switch (mesh_type)
      {
        case TETRAHEDRON:  platonic->SetModelType (MODEL_TETRAHEDRON); break;
        case HEXAHEDRON:   platonic->SetModelType (MODEL_HEXAHEDRON); break;
        case OCTAHEDRON:   platonic->SetModelType (MODEL_OCTAHEDRON); break;
        case DODECAHEDRON: platonic->SetModelType (MODEL_DODECAHEDRON); break;
        case ICOSAHEDRON:  platonic->SetModelType (MODEL_ICOSAHEDRON); break;    
      }

      platonic->SetParam     ("radius",0);

      start_grid_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);
    }
    
    void accept ()
    {
      platonic = NULL;
    }
    
    void process (int x,int y)
    {
      if (!platonic)
        return;

      cur_point = viewport->ProjectPointOnGrid (x,y);
      
      platonic->SetParam  ("radius",length (cur_point-start_grid_point));
      platonic->identity  ();
      platonic->translate (start_grid_point);
    }

  private:
    Viewport*   viewport;    
    ParamModel* platonic;
    Scene*      scene;
    vec4f       start_grid_point, cur_point;
    MeshType    mesh_type;
};

Manipulator* manipCreateTetrahedron (RenderCursor& cursor,const char*)
{
  return new CreatePlatonicManipulator (cursor, TETRAHEDRON);
}

Manipulator* manipCreateHexahedron (RenderCursor& cursor,const char*)
{
  return new CreatePlatonicManipulator (cursor, HEXAHEDRON);
}

Manipulator* manipCreateOctahedron (RenderCursor& cursor,const char*)
{
  return new CreatePlatonicManipulator (cursor, OCTAHEDRON);
}

Manipulator* manipCreateDodecahedron (RenderCursor& cursor,const char*)
{
  return new CreatePlatonicManipulator (cursor, DODECAHEDRON);
}

Manipulator* manipCreateIcosahedron (RenderCursor& cursor,const char*)
{
  return new CreatePlatonicManipulator (cursor, ICOSAHEDRON);
}
