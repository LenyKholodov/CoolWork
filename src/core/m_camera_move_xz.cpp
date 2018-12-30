#include "manipulators.h"

using namespace std;

class CameraMoveManipulatorXZ: public Manipulator
{
  public:
    struct Error {};
    float step_z,step_x,screen_z,screen_x;

    CameraMoveManipulatorXZ (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();
      camera      = viewport->GetCamera ();
      screen_x = 0;
      screen_z = 0;
      step_z = 0;
      step_x = 0;
             
      if (!scene)
        throw Error ();

    }
    
    ~CameraMoveManipulatorXZ ()
    {
    }
	
    void select ()
    {
	prev_x = cursor.GetPosition ().x;
	prev_z = cursor.GetPosition ().y;
	step_z = 0.3f;
	step_x = 0.3f;
    }
	

    void accept ()
    {
	screen_z = 0;
	screen_x = 0;
	step_x = 0;
	step_z = 0;
    }

    void process (int x,int y)
    {
	if (prev_z != cursor.GetPosition ().y)
	{
	if (prev_z > cursor.GetPosition ().y)
	{
	screen_z = step_z;
	}
	else
	{
	screen_z = -1*step_z;
	};
	};
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

	camera->translate(vec3f(screen_x,0,screen_z));
	prev_z = cursor.GetPosition ().y;
	prev_x = cursor.GetPosition ().x;
	screen_x = 0;
	screen_z = 0;
    }

    
  private:
    int prev_x,prev_z;
    Viewport*   viewport;    
    Scene*      scene;
    Camera* camera;
};

Manipulator* manipCameraMoveRL_FB (RenderCursor& cursor,const char*)
{
  return new CameraMoveManipulatorXZ (cursor);
}