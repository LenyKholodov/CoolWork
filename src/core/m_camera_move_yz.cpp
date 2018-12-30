#include "manipulators.h"

using namespace std;

class CameraMoveManipulatorZY: public Manipulator
{
  public:
    struct Error {};
    float step_z,step_y,screen_z,screen_y;

    CameraMoveManipulatorZY (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();
      camera      = viewport->GetCamera ();
      screen_z = 0;
      screen_y = 0;
      step_z = 0;
      step_y = 0;
             
      if (!scene)
        throw Error ();

    }
    
    ~CameraMoveManipulatorZY ()
    {
    }
	
    void select ()
    {
	prev_z = cursor.GetPosition ().x;
	prev_y = cursor.GetPosition ().y;
	step_y = 0.3f;
	step_z = 0.3f;
    }
	

    void accept ()
    {
	screen_y = 0;
	screen_z = 0;
	step_z = 0;
	step_y = 0;
    }

    void process (int x,int y)
    {
	if (prev_y != cursor.GetPosition ().y)
	{
	if (prev_y > cursor.GetPosition ().y)
	{
	screen_y = step_y;
	}
	else
	{
	screen_y = -1*step_y;
	};
	};
	if (prev_z != cursor.GetPosition ().x)
	{
	if (prev_z > cursor.GetPosition ().x)
	{
	screen_z = step_z;
	}
	else
	{
	screen_z = -1*step_z;
	};
	};

	camera->translate(vec3f(0,screen_y,screen_z));
	prev_y = cursor.GetPosition ().y;
	prev_z = cursor.GetPosition ().x;
	screen_y = 0;
	screen_z = 0;
    }

    
  private:
    int prev_z,prev_y;
    Viewport*   viewport;    
    Scene*      scene;
    Camera* camera;
};

Manipulator* manipCameraMoveFB_UD (RenderCursor& cursor,const char*)
{
  return new CameraMoveManipulatorZY (cursor);
}