#include "manipulators.h"

using namespace std;

class CameraRotateManipulator: public Manipulator
{
  public:
    struct Error {};
    float step_x,screen_x;

    CameraRotateManipulator (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();
      camera      = viewport->GetCamera ();
      screen_x = 0;
      step_x = 0;
             
      if (!scene)
        throw Error ();
      camera->SetPerspective (120,1,1000);

    }
    
    ~CameraRotateManipulator ()
    {
    }
      
    void select ()
    {
      prev_x = cursor.GetPosition ().x;
      step_x = 3;
    }
      

    void accept ()
    {
      screen_x = 0;
      step_x = 0;
    }

    void process (int x,int y)
    {
      if (prev_x != cursor.GetPosition ().x)
      {
      if (prev_x > cursor.GetPosition ().x)
      {
      screen_x = step_x;
      }
      else
      {
      screen_x = -1*step_x;
      };
      };

      camera->rotate(0,0,screen_x);
      prev_x = cursor.GetPosition ().x;
      screen_x = 0;
    }

    
  private:
    int prev_x;
    Viewport*   viewport;    
    Scene*      scene;
    Camera* camera;
};

Manipulator* manipCameraRotate (RenderCursor& cursor,const char*)
{
  return new CameraRotateManipulator (cursor);
}