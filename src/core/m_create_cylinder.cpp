#include <math.h>
#include "manipulators.h"

using namespace std;

#define EDIT_STEP 0.02

class CreateCylinderManipulator: public Manipulator
{
  public:
    struct Error {};

    CreateCylinderManipulator (RenderCursor& cursor, bool is_half_body) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();
      cylinder    = NULL;      
      click_count = 0;
                
      if (!scene)
        throw Error ();
      if (is_half_body) type = MODEL_HALF_CYLINDER;
      else              type = MODEL_CYLINDER;
    }
    
    ~CreateCylinderManipulator ()
    {
      if (cylinder)
        cylinder->release();
    }

    void select ()
    {
      if (click_count < 2)
        start_y = cursor.GetPosition ().y;

      cur_y_val = 0;
      
      if (!cylinder)
      {
        cylinder = scene->CreateModel();        
        cylinder->SetModelType (type);
        cylinder->SetParam     ("radius",0);
        cylinder->SetParam     ("height",0);

        click_count = 0;
      }

      if (!click_count)
        start_grid_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);

      click_count++;
    }

    void accept ()
    {
      if (click_count < 3)
        start_y = cursor.GetPosition ().y;

      click_count++;

      if (click_count == 4)
      {
        cylinder    = NULL;
        click_count = 0;
      }
    }
    
    void process (int x,int y)
    {
      if(!click_count || !cylinder)
        return;

      cur_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);

      cur_y_val = abs(y - start_y);

      switch (click_count)
      {
        case 1: 
        {
          cylinder->SetParam("radius", length(cur_point - start_grid_point)); 
          cylinder->identity();
          cylinder->translate(vec3f(start_grid_point.x, 0, start_grid_point.z));
          break;
        }
        case 2: 
        case 3:
        {
          if (y < start_y) y_dir = 1;
          else             y_dir = -1;

          cylinder->SetParam("height", (float)(cur_y_val * EDIT_STEP)); 
          cylinder->identity();
          cylinder->translate(vec3f(start_grid_point.x, 0, start_grid_point.z));
          cylinder->scale(vec3f(1, (float)y_dir,1));
          break;
        }
      }
    }

  private:
    Viewport*      viewport;    
    ParamModel*    cylinder;
    ParamModelType type;
    Scene*         scene;
    vec4f          start_grid_point, cur_point;
    int            click_count, start_y, cur_y_val, y_dir;
};

Manipulator* manipCreateCylinder (RenderCursor& cursor,const char*)
{
  return new CreateCylinderManipulator (cursor, false);
}

Manipulator* manipCreateHalfCylinder (RenderCursor& cursor,const char*)
{
  return new CreateCylinderManipulator (cursor, true);
}
