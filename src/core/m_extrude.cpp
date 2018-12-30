#include <time.h>
#include "manipulators.h"
#include "landscape.h"
#include <algorithm>

#define CYCLE_TIME 0.01f

using namespace std;

enum ExtrusionType
{
  PENCIL_EXTRUSION,
  BRUSH_EXTRUSION
};

class LandscapeVisitor: public EntityVisitor 
{
  public:
    
    void visit(LandscapeModel* ls_model) 
    {
      planes.push_back (ls_model);
    }

    typedef vector<LandscapeModel*> LandscapeList;
    LandscapeList planes;
};

class ExtrudeManipulator: public Manipulator
{
  public:
    
    enum State {
      SELECT,
      ACTION      
    };

    struct Error {};
    
    ExtrudeManipulator (RenderCursor& cursor, ExtrusionType type) : Manipulator (cursor)
    {
      Etc& etc = Command::instance ().etc ();

      extrusion_type = type;  
      state          = SELECT;

      if (etc.present("manipulator.flow"))
        flow = etc.getf("manipulator.flow");
      else 
        flow = 100.f;
      if (etc.present("manipulator.radius"))
        radius = etc.geti("manipulator.radius");
      else 
        radius = 10;
      if (etc.present("manipulator.mode"))
        mode = etc.geti("manipulator.mode");
      else 
        mode = 0;

      viewport       = cursor.GetActiveViewport ();
      cursor.NewSelection ();
      
      ChangeSelection ();
    }    
    
    void select ()
    {
      //здесь обнаруживать, что щёлкнули вне ландшафтов!!
      if (!ls_visitor.planes.size ())
        if (!ChangeSelection ())       
          return;

      old_maps.clear();
      old_maps.resize(ls_visitor.planes.size ());
      for (size_t i = 0; i < ls_visitor.planes.size (); i++)
        old_maps[i] = ls_visitor.planes[i]->GetHeightMap();      

      state = ACTION;
      time  = clock();
    }
    
    void accept ()
    {
      state = SELECT;
    }
    
    void process (int x,int y)
    {
      if (state != ACTION)
        return;

      extrude ();
    }

    void idle ()
    {
      if (state != ACTION)
        return;

      if ( ((float)(clock() - time) / (float)CLOCKS_PER_SEC) > CYCLE_TIME)
      {
        extrude ();
        time = clock();
      }
    }
    
    bool ChangeSelection ()
    {       
      entities = cursor.GetSelection ();

      if (!entities.size ())
      {
        finish ();
        return false;
      }

      ls_visitor.planes.clear (); 
      entities.traverse (&ls_visitor);
      
      return true;
    }

    void extrude ()
    {
      for (size_t i = 0; i < ls_visitor.planes.size(); i++)
      {
        LandscapeModel* plane = ls_visitor.planes [i];        

        vec3f pos = 0.0f, rot = 0.0f, scale = 1.0f, plane_coords = viewport->ProjectPointOnGrid (cursor.GetPosition ().x, cursor.GetPosition ().y);
        vec2i map_dimensions(plane->GetDimensions());

        affine_decomp (plane->GetWorldTM (), pos, rot, scale);

        plane_coords.x = map_dimensions.x / 2 + plane_coords.x * map_dimensions.x / scale.x;
        plane_coords.z = map_dimensions.y / 2 + plane_coords.z * map_dimensions.y / scale.z;                

        switch(extrusion_type)
        {
          case PENCIL_EXTRUSION:
                                 plane->PencilExtrusion (radius, (int)plane_coords.x, (int)plane_coords.z, flow, mode, &old_maps[i]);
                                 break;
          case BRUSH_EXTRUSION:
                                 plane->BrushExtrusion (radius, (int)plane_coords.x, (int)plane_coords.z, flow, mode, &old_maps[i]);
                                 break;      
        }
      } 
    }

    Viewport*        viewport;
    vector<Bitmap>   old_maps;
    EntityList       entities;
    LandscapeVisitor ls_visitor;    
    State            state;
    ExtrusionType    extrusion_type;
    time_t           time;
    float	     flow;
    int              radius, mode;
};

Manipulator* manipCreatePencilExtrude (RenderCursor& cursor,const char*)
{
  return new ExtrudeManipulator (cursor, PENCIL_EXTRUSION);
}

Manipulator* manipCreateBrushExtrude (RenderCursor& cursor,const char*)
{
  return new ExtrudeManipulator (cursor, BRUSH_EXTRUSION);
}
