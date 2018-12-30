#include <math.h>
#include "manipulators.h"

using namespace std;

class CreateSphereManipulator: public Manipulator
{
  public:
    struct Error {};

    CreateSphereManipulator (RenderCursor& cursor, bool is_half_body) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      sphere      = NULL;
      scene       = cursor.GetActiveScene ();
                
      if (!scene)
        throw Error ();
      if (is_half_body) type = MODEL_HALF_SPHERE;
      else              type = MODEL_SPHERE;
    }
    
    ~CreateSphereManipulator ()
    {
      if (sphere)
        sphere->release();
    }

    void select ()
    {      
      if (sphere)
        return;

      sphere = scene->CreateModel();
      
      sphere->SetModelType (type);
      sphere->SetParam     ("radius",0);

      start_grid_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);
    }
    
    void accept ()
    {
      sphere = NULL;
    }
    
    void process (int x,int y)
    {
      if (!sphere)
        return;

      cur_point = viewport->ProjectPointOnGrid (x,y);
      
      sphere->SetParam  ("radius",length (cur_point-start_grid_point));
      sphere->identity  ();
      sphere->translate (start_grid_point);
    }

  private:
    Viewport*      viewport;    
    ParamModel*    sphere;
    ParamModelType type;
    Scene*         scene;
    vec4f          start_grid_point, cur_point;
};

Manipulator* manipCreateSphere (RenderCursor& cursor,const char*)
{
  return new CreateSphereManipulator (cursor, false);
}

Manipulator* manipCreateHalfSphere (RenderCursor& cursor,const char*)
{
  return new CreateSphereManipulator (cursor, true);
}
