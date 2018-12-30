#include <math.h>
#include "manipulators.h"

using namespace std;

enum _LightType
{
  POINT_LIGHT,
  SPOT_LIGHT,
  DIRECT_LIGHT
};

class CreateFreeLightManipulator: public Manipulator
{
  public:
    struct Error {};

    CreateFreeLightManipulator (RenderCursor& cursor, _LightType type) : Manipulator (cursor)
    {
      viewport   = cursor.GetActiveViewport ();
      light      = NULL;
      scene      = cursor.GetActiveScene ();
      light_type = type;
                
      if (!scene)
        throw Error ();
    }
    
    ~CreateFreeLightManipulator ()
    {
      if (light)
        light->release();
    }

    void select ()
    {      
      if(!light)
        light = scene->CreateLight();
    }
    
    void accept ()
    {
      light = NULL;
    }
    
    void process (int x,int y)
    {
      if (!light)
        return;


      cur_point = viewport->ProjectPointOnGrid (x,y);
      
      switch (light_type)
      {
        case POINT_LIGHT:  light->SetPointLight (cur_point);                  break;
        case SPOT_LIGHT:   light->SetSpotLight  (cur_point, vec3f(0, -1, 0)); break;
        case DIRECT_LIGHT: light->SetDirectLight (vec3f(0, -1, 0));           break;
      }
    }

  private:
    Viewport*   viewport;    
    Light*      light;
    _LightType  light_type;
    Scene*      scene;
    vec4f       cur_point;
};

Manipulator* manipCreatePointLight (RenderCursor& cursor,const char*)
{
  return new CreateFreeLightManipulator (cursor, POINT_LIGHT);
}

Manipulator* manipCreateSpotFreeLight (RenderCursor& cursor,const char*)
{
  return new CreateFreeLightManipulator (cursor, SPOT_LIGHT);
}

Manipulator* manipCreateDirectFreeLight (RenderCursor& cursor,const char*)
{
  return new CreateFreeLightManipulator (cursor, DIRECT_LIGHT);
}
