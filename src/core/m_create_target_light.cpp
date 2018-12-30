#include <math.h>
#include "manipulators.h"

using namespace std;

enum _LightType
{
  SPOT_LIGHT,
  DIRECT_LIGHT
};

class CreateTargetLightManipulator: public Manipulator
{
  public:
    struct Error {};

    CreateTargetLightManipulator (RenderCursor& cursor, _LightType type) : Manipulator (cursor)
    {
      viewport   = cursor.GetActiveViewport ();
      light      = NULL;
      scene      = cursor.GetActiveScene ();
      light_type = type;
                
      if (!scene)
        throw Error ();
    }
    
    ~CreateTargetLightManipulator ()
    {
      if (light)
        light->release();
    }

    void select ()
    {      
      if(!light)
       light = scene->CreateLight();
      start_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);
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
        case SPOT_LIGHT:   light->SetSpotLight   (start_point, cur_point - start_point); break;
        case DIRECT_LIGHT: light->SetDirectLight (cur_point - start_point);              break;
      }
    }

  private:
    Viewport*   viewport;                                                                
    Light*      light;
    _LightType  light_type;
    Scene*      scene;
    vec4f       cur_point, start_point;
};

Manipulator* manipCreateSpotTargetLight (RenderCursor& cursor,const char*)
{
  return new CreateTargetLightManipulator (cursor, SPOT_LIGHT);
}

Manipulator* manipCreateDirectTargetLight (RenderCursor& cursor,const char*)
{
  return new CreateTargetLightManipulator (cursor, DIRECT_LIGHT);
}
