#include "manipulators.h"

using namespace std;

class CameraZoomManipulator: public Manipulator
{
  public:
    struct Error {};
    float step_y,screen_y;

    CameraZoomManipulator (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();
      camera      = viewport->GetCamera ();
      screen_y = 1;
      step_y = 1;
             
      if (!scene)
        throw Error ();

    }
    
    ~CameraZoomManipulator ()
    {
    }
      
    void select ()
    {
      prev_y = cursor.GetPosition ().y;
      step_y = 2;
    }
      

    void accept ()
    {
      screen_y = 1;
      step_y = 1;
    }

    void process (int x,int y)
    {
      if (prev_y != cursor.GetPosition ().y)
      {
      if (prev_y > cursor.GetPosition ().y)
      {
      screen_y = 1.0f/step_y;
      }
      else
      {
      screen_y = step_y;
      };
      };
      camera->scale(vec3f(screen_y,screen_y,1));
      prev_y = cursor.GetPosition ().y;
      screen_y = 1;
    }

    
  private:
    int prev_y;
    Viewport*   viewport;    
    Scene*      scene;
    Camera* camera;
};

Manipulator* manipCameraZoom (RenderCursor& cursor,const char*)
{
  return new CameraZoomManipulator (cursor);
}