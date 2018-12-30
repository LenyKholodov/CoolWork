#include "manipulators.h"

using namespace std;

class BindManipulator: public Manipulator
{
  public:
    struct Error {};

    BindManipulator (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport = cursor.GetActiveViewport ();
      scene    = cursor.GetActiveScene ();
                
      if (!scene)
        throw Error ();

      hlp          = viewport->GetHelperScene ()->CreateHelper ();
      click_status = false;
    }
    
    ~BindManipulator ()
    {
      hlp->release();
    }
    
    void select ()
    {
      cursor.NewSelection ();
      
      start_x       = cursor.GetPosition ().x;
      start_y       = cursor.GetPosition ().y;
      startEntities = cursor.GetSelection ();      
      
      if (!startEntities.size ())
      {
        hlp->disable (ENTITY_VISIBLE);
        click_status = false;
        return;
      }
      
      hlp->enable (ENTITY_VISIBLE);
      
      click_status = true;
    }
    
    void process (int x,int y)
    {
      if (click_status)
        hlp->SetLine (vec3f ((float)start_x,(float)start_y,0),vec3f((float)x,(float)y,0));
    }

    void accept ()
    {
      if (!click_status)
        return;
      
      int end_x = cursor.GetPosition ().x,
          end_y = cursor.GetPosition ().y;
      EntityList selectedList;
      
      viewport->select (selectedList,end_x,end_y,2,2,SELECT_OBJECTS);
      
      if (selectedList.size())
      {
        Entity* parent = selectedList.item (0);
        
        for (size_t i=0;i<startEntities.size();i++)                
          startEntities.item (i)->bind (parent,true);
      }              
      
      startEntities.clear ();
      cursor.NewSelection ();
            
      click_status = false;
      hlp->disable (ENTITY_VISIBLE);
    }
    
  private:
    int           start_x,start_y;
    bool          click_status;
    Viewport*     viewport;    
    Scene*        scene;
    HelperEntity* hlp;
    EntityList    startEntities;    
};

Manipulator* manipCreateBind (RenderCursor& cursor,const char*)
{
  return new BindManipulator (cursor);
}
