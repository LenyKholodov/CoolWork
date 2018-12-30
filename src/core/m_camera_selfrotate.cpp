#include "manipulators.h"

using namespace std;

class CameraSelfRotateManipulator: public Manipulator
{
  public:
    struct Error {};

    CameraSelfRotateManipulator (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();
      camera      = viewport->GetCamera ();
      in_rotate   = false;
             
      if (!scene)
        throw Error ();
    }    
  
    void select ()
    {
      prev_x    = cursor.GetPosition ().x;
      prev_y    = cursor.GetPosition ().y;
      startTM   = camera->GetWorldTM ();
      in_rotate = true;
    }  

    void accept ()
    {
      in_rotate = false;
    }
    
    static mat4f getrotate (const vec3f& from,const vec3f& to)
    {
      vec3f n    = normalize (cross (from,to));
      float _cos = normalize (from)&normalize (to);
  
      return rotate (acos (_cos),n);  
    }

    void process (int x,int y)
    {    
      if (!in_rotate)
        return;
      
      camera->identity  ();        
      camera->transform (startTM);
      camera->rotate    (90.0f*(float)(prev_y-y)/(float)viewport->height (),vec3f (1,0,0));
      camera->rotate    (90.0f*(float)(x-prev_x)/(float)viewport->width (),vec3f (0,1,0));
    }
    
  private:
    bool        in_rotate;
    mat4f       startTM;
    int         prev_x,prev_y;
    Viewport*   viewport;    
    Scene*      scene;
    Camera*     camera;
};

Manipulator* manipCameraSelfRotate (RenderCursor& cursor,const char*)
{
  return new CameraSelfRotateManipulator (cursor);
}
