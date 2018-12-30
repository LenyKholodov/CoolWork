#include <math.h>
#include "manipulators.h"

using namespace std;

class CreateLandscapeManipulator: public Manipulator
{
  public:
    struct Error {};

    CreateLandscapeManipulator (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport    = cursor.GetActiveViewport ();
      scene       = cursor.GetActiveScene ();
      landscape   = NULL;
      click_count = 0;
                      
      if (!scene)
        throw Error ();
    }
    
    ~CreateLandscapeManipulator ()
    {
      if (!click_count)
        landscape->release();
    }

    void select ()
    {
      landscape = scene->CreateLandscape ();
    }

    void accept ()
    {
      click_count++;
      if (scale_x > scale_z)
        landscape->resize (128, (size_t)(128 / (scale_x / scale_z)));
      else
        landscape->resize ((size_t)(128 / (scale_z / scale_x)), 128);
      finish ();
      return;
    }
    
    void process (int x,int y)
    { 
      if (!landscape)
        return;
      
      cur_point = viewport->ProjectPointOnGrid (x, y);

      scale_x = abs (cur_point.x) * 2;
      scale_z = abs (cur_point.z) * 2;

      landscape->identity ();
      landscape->scale(vec3f(scale_x, 1.f, scale_z)); 
    }

  private:
    Viewport*       viewport;    
    LandscapeModel* landscape;
    Scene*          scene;
    vec3f           cur_point;
    int             click_count;
    float           scale_x, scale_z;
};

Manipulator* manipCreateLandscape (RenderCursor& cursor,const char*)
{
  return new CreateLandscapeManipulator (cursor);
}
