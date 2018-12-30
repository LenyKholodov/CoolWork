#include "manipulators.h"
#include <algorithm>

using namespace std;

class SelectManipulator: public Manipulator
{
  public:
    SelectManipulator (RenderCursor& cursor) : Manipulator (cursor)
    {      
      viewport    = cursor.GetActiveViewport ();
      select_mode = false;
    }    
    
    ~SelectManipulator ()
    {
      viewport->SetSelectRect (0,0,0,0);      
    }
    
    void select ()
    {
      viewport->SetSelectRect (0,0,0,0);
            
      start_x     = cursor.GetPosition ().x;
      start_y     = cursor.GetPosition ().y;
      select_mode = true;
    }
    
    void accept ()
    {
      int x = cursor.GetPosition ().x,
          y = cursor.GetPosition ().y;
      
      cursor.NewSelection (min (start_x,x),min (start_y,y),max (abs (start_x-x),2),max (abs (start_y-y),2));
      
      viewport->SetSelectRect (0,0,0,0);
      select_mode = false;
    }
    
    void process (int x,int y)
    {                                                    
      if (select_mode)
        viewport->SetSelectRect (min (start_x,x),min (start_y,y),abs (start_x-x),abs (start_y-y));
    }
    
    Viewport* viewport;
    int       start_x, start_y;
    bool      select_mode;
};

Manipulator* manipCreateSelect (RenderCursor& cursor,const char*)
{
  return new SelectManipulator (cursor);
}
