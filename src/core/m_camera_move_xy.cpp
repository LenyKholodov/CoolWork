#include "manipulators.h"

using namespace std;

class CameraMoveManipulatorXY: public Manipulator
{
  public:
    struct Error {};
    float step_x,step_y,screen_x,screen_y;

    CameraMoveManipulatorXY (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();
      camera      = viewport->GetCamera ();
      screen_x = 0;
      screen_y = 0;
      step_x = 0;
      step_y = 0;
             
      if (!scene)
        throw Error ();

    }
    
    ~CameraMoveManipulatorXY ()
    {
    }
	
    void select ()
    {
	prev_x = cursor.GetPosition ().x;
	prev_y = cursor.GetPosition ().y;
	step_y = 0.3f;
	step_x = 0.3f;
    }
	

    void accept ()
    {
	screen_y = 0;
	screen_x = 0;
	step_x = 0;
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

	camera->translate(vec3f(screen_x,screen_y,0));
	prev_y = cursor.GetPosition ().y;
	prev_x = cursor.GetPosition ().x;
	screen_y = 0;
	screen_x = 0;
    }

    
  private:
    int prev_x,prev_y;
    Viewport*   viewport;    
    Scene*      scene;
    Camera* camera;
};

Manipulator* manipCameraMoveRL_UD (RenderCursor& cursor,const char*)
{
  return new CameraMoveManipulatorXY (cursor);
}