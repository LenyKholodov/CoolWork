#include <math.h>
#include "manipulators.h"

using namespace std;

#define EDIT_STEP 0.02

class CreateBoxManipulator: public Manipulator
{
  public:
    struct Error {};

    CreateBoxManipulator (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();
      box         = NULL;
      click_count = 0;
                      
      if (!scene)
        throw Error ();
    }
    
    ~CreateBoxManipulator ()
    {
      if (click_count < 4)
        box->release();
    }

    void select ()
    {
      if (click_count < 2)
        start_y = cursor.GetPosition ().y;

      if (!box)
      {
        box = scene->CreateModel();
        
        box->SetModelType (MODEL_BOX);
        box->SetParam     ("width",0);
        box->SetParam     ("depth",0);
        box->SetParam     ("height",0);
        click_count = 0;
      }

      if (!click_count)
        start_grid_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);

      click_count++;
      cur_y_val = 0;
    }

    void accept ()
    {
      if (click_count < 3)
        start_y = cursor.GetPosition ().y;

      click_count++;

      if (click_count == 4)
      {
        box         = NULL;
        click_count = 0;
      }
    }
    
    void process (int x,int y)
    { 
      if (!click_count || !box)
        return;
      
      cur_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);
      cur_y_val = abs (y-start_y);

      float y_edit_val = (float)(cur_y_val * EDIT_STEP);

        switch (click_count)
        {
          case (1):
          { 
            width = cur_point.x - start_grid_point.x;
            depth = cur_point.z - start_grid_point.z;
            box->SetParam("width", abs(width)); 
            box->SetParam("depth", abs(depth)); 
            box->identity();
            box->translate(vec3f(start_grid_point.x + width / 2, 0, start_grid_point.z + depth / 2));
            break;       
          }
          case (2):
          case (3):
          {
            if (y < start_y) y_dir = 1;
            else             y_dir = -1;

            box->SetParam("height", y_edit_val);
            box->identity();
            box->translate(vec3f(start_grid_point.x + width / 2, y_edit_val / 2 * y_dir, start_grid_point.z + depth / 2));
            break;
          }
        }
    }

  private:
    Viewport*   viewport;    
    ParamModel* box;
    Scene*      scene;
    vec4f       start_grid_point, cur_point;
    int         click_count, start_y, cur_y_val, y_dir;
    float       width, depth;
};

Manipulator* manipCreateBox (RenderCursor& cursor,const char*)
{
  return new CreateBoxManipulator (cursor);
}
