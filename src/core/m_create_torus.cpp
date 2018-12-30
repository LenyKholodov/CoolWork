#include <math.h>
#include "manipulators.h"

using namespace std;

class CreateTorusManipulator: public Manipulator
{
  public:
    struct Error {};

    CreateTorusManipulator (RenderCursor& cursor, bool is_half_body) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();
      torus       = NULL;      
      click_count = 0;
                
      if (!scene)
        throw Error ();
      if (is_half_body) type = MODEL_HALF_TORUS;
      else              type = MODEL_TORUS;
    }
    
    ~CreateTorusManipulator ()
    {
      if (torus)
        torus->release();
    }

    void select ()
    {
      if (!torus)
      {
        torus = scene->CreateModel();
        
        torus->SetModelType (type);
        torus->SetParam     ("radius",0);
        torus->SetParam     ("radius2",0);
        click_count = 0;
      }

      if (!click_count)
        start_grid_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);

      click_count++;      
    }

    void accept ()
    {
      if (click_count < 2) second_point = cur_point;

      click_count++;

      if (click_count == 4)
      {
        torus    = NULL;
        click_count = 0;
      }
    }
    
    void process (int x,int y)
    {
      if(!click_count || !torus)
        return;

      cur_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);

      switch (click_count)
      {
        case 1: 
        {
          torus->SetParam("radius", length(cur_point - start_grid_point)); 
          torus->identity();
          torus->translate(vec3f(start_grid_point.x, 0, start_grid_point.z));
          break;
        }
        case 2: 
        case 3:
        {
          float radius2 = (length(start_grid_point - second_point) - length(start_grid_point - cur_point)) / 2; 

          torus->SetParam("radius2", abs(radius2)); 
          torus->SetParam("radius", length(start_grid_point - second_point) - radius2); 
          torus->identity();
          torus->translate(vec3f(start_grid_point.x, 0, start_grid_point.z));
          break;
        }
      }
    }

  private:
    Viewport*      viewport;    
    ParamModel*    torus;
    ParamModelType type;
    Scene*         scene;
    vec4f          start_grid_point, second_point, cur_point;
    int            click_count;
};

Manipulator* manipCreateTorus (RenderCursor& cursor,const char*)
{
  return new CreateTorusManipulator (cursor, false);
}

Manipulator* manipCreateHalfTorus (RenderCursor& cursor,const char*)
{
  return new CreateTorusManipulator (cursor, true);
}
