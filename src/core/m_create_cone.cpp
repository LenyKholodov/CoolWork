#include <math.h>
#include "manipulators.h"

using namespace std;

#define EDIT_STEP   0.05
#define EPS         0.01

class CreateConeManipulator: public Manipulator
{
  public:
    struct Error {};

    CreateConeManipulator (RenderCursor& cursor,bool is_half_cone) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();      
      cone        = NULL;
      click_count = 0;
                
      if (!scene)
        throw Error ();
      if (!is_half_cone) type = MODEL_CONE;
         else type = MODEL_HALF_CONE;

    }
        
    ~CreateConeManipulator ()
    {
      if (cone)
        cone->release();
    }
    
    void select ()
    {
      if (click_count < 2)
        start_y  = cursor.GetPosition ().y;

      cur_y_val = 0;

      if (!cone)
      {
        cone = scene->CreateModel();
        cone->SetModelType (type);
        cone->SetParam     ("radius",0);
        cone->SetParam     ("radius2",0);
        cone->SetParam     ("height",0);        
      
        click_count = 0;
      }

      if (!click_count)
        start_grid_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);

      if ((click_count < 2) || (click_count > 3))
        click_count++;
    }

    void accept ()
    {
      if (click_count < 2)
        start_y = cursor.GetPosition ().y;

      click_count++;

      if (click_count == 4) 
      {
        cone = NULL;
        click_count = 0;
      }
    }
    
    void process (int x,int y)
    {      
      if(!click_count || !cone)
        return;

      cur_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);

      cur_y_val = abs(y - start_y);

      switch (click_count)
      {        
        case 1: 
        {
          cone->SetParam("radius", length(cur_point - start_grid_point));
          cone->SetParam("radius2", length(cur_point - start_grid_point));
          cone->identity();
          cone->translate(vec3f(start_grid_point.x, 0, start_grid_point.z));
          break;
        }
        case 2:
        {
          if (y < start_y) y_dir = 1;
          else             y_dir = -1;          

          cone->SetParam("height", (float)(cur_y_val * EDIT_STEP));
          cone->identity();
          cone->translate(vec3f(start_grid_point.x, 0, start_grid_point.z));
          cone->scale(vec3f(1, (float)y_dir, 1)); 
          break;
        }
        case 3: 
        {
           cone->SetParam("radius2", (float)((start_y - y) * EDIT_STEP / 2));
           cone->identity();
           cone->translate(vec3f(start_grid_point.x, 0, start_grid_point.z));
           if (abs(cone->GetParam ("radius2")) < EPS)
             cone->SetParam("radius2",(float)EPS);
          break;
        }
      }
    }

  private:
    Viewport*      viewport;    
    Scene*         scene;
    ParamModel*    cone;
    ParamModelType type;    
    vec4f          start_grid_point, cur_point;
    int            click_count, start_y, cur_y_val, y_dir;
};

Manipulator* manipCreateCone (RenderCursor& cursor,const char*)
{
  return new CreateConeManipulator (cursor, false);
}

Manipulator* manipCreateHalfCone (RenderCursor& cursor,const char*)
{
  return new CreateConeManipulator (cursor, true);
}

