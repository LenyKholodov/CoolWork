#include "manipulators.h"
#include <algorithm>

using namespace std;

struct OrtInfo
{
  vec3f       dir;
  vec3f       up;
  const char* name;
};

struct RectInfo
{
  vec3f center;
  vec3f dir;
};

static OrtInfo ort_info [] = {
  {vec3f (1,0,0),vec3f (0,1,0),"X"},
  {vec3f (0,1,0),vec3f (1,0,0),"Y"},
  {vec3f (0,0,1),vec3f (0,1,0),"Z"}
};

static RectInfo rect_info [] = {
  {vec3f (0,0.5f,0.5f),vec3f (1,0,0)},
  {vec3f (0.5f,0,0.5f),vec3f (0,1,0)},
  {vec3f (0.5f,0.5f,0),vec3f (0,0,1)},
};

enum VisitState {
  VISIT_NOP,
  VISIT_GET_SHEARS,
  VISIT_SET_SHEARS,
};

class ShearVisitor: public EntityVisitor {
public:
  float x,z;
  vec2f* old_shears;
  VisitState state;
  void visit(ParamModel* pmodel) {
    if (!old_shears)
      return;
    switch (state) {
    case VISIT_GET_SHEARS:
      old_shears->x=pmodel->GetParam("shear_x");
      old_shears->y=pmodel->GetParam("shear_z");
      break;
    case VISIT_SET_SHEARS:
      pmodel->SetParam("shear_x",old_shears->x+x);
      pmodel->SetParam("shear_z",old_shears->y+z);
      break;
    }
  }
};

const float EPS = 0.01f;

class ShearManipulator: public Manipulator
{
  public:
    enum State {
      WAIT,
      SELECT,
      MOVE      
    };
    
    struct Error {};
    
    ShearManipulator (RenderCursor& cursor) : Manipulator (cursor)
    {     
      Scene* scene = cursor.GetActiveScene ();
      
      if (!scene)
      {
        logPrintf ("No active scene");
        throw Error ();
      }      
      
      viewport = cursor.GetActiveViewport ();                  
      cursor.NewSelection ();
      
      ort_mask = 0.0f;
      state = WAIT;
      
      basis = scene->CreateEntity ();
      
      basis->enable (ENTITY_TEMP);
        
      for (int i=0;i<3;i++)
      {        
        ort [i] = scene->CreateHelper ();
        
        ort [i]->SetArrow     (0.0f,ort_info [i].dir);
        ort [i]->SetWireColor (ort_info [i].dir);
        ort [i]->scale        (4.0f);
        ort [i]->bind         (basis);
        ort [i]->rename       (ort_info [i].name);        
        ort [i]->enable       (ENTITY_TEMP);
                  
        rect [i] = scene->CreateHelper ();
        
        rect [i]->SetRect      (rect_info [i].center,rect_info [i].dir);
        rect [i]->SetWireColor (0.2f);
        rect [i]->bind         (basis);
        rect [i]->enable       (ENTITY_TEMP);
        
        ort_mask [i] = 0.0f;
      }            
      
      ChangeSelection ();
    }
    
    ~ShearManipulator ()
    {
      for (int i=0;i<3;i++)
      {
        ort [i]->release ();
        rect [i]->release ();
      }
      
      if (basis)
        basis->release ();      
    }
    
    void select ()
    {
      if (equal (ort_mask,vec3f (0.0f),EPS))
      {
        if (state != WAIT)
          ChangeSelection ();
        
        return;
      }
      
      old_shears.resize(entities.size ());
      visitor.state=VISIT_GET_SHEARS;
      for (size_t i=0;i<entities.size ();i++) {
        Entity *e=entities.item (i);
        start_tm [i] = e->GetWorldTM ();
        visitor.old_shears=&old_shears[i];
        e->traverse(&visitor);
      }
      
      start_grid_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);
      state            = MOVE;
    }
    
    void accept ()
    {
      state = SELECT;
      
      for (int i=0;i<3;i++)
        ort_mask [i] = 0.0f;
    }
    
    void process (int x,int y)
    {
      if (!viewport || !entities.size ())
        return;      
      
      switch (state)
      {
        case WAIT:
        case SELECT:
        {
          EntityList selection;
          
          viewport->select (selection,cursor.GetPosition ().x,cursor.GetPosition ().y,4,4,SELECT_HELPERS);
          
          bool ort_selected [3] = {false,false,false};
          bool rect_selected [3] = {false,false,false};                
          
          for (size_t i=0;i<selection.size ();i++)
            for (int j=0;j<3;j++)
            {
              if      (selection.item (i) == ort [j])  ort_selected [j] = true;
              else if (selection.item (i) == rect [j]) rect_selected [j] = true;
            }
              
          for (int i=0;i<3;i++)
          {
            rect_selected [i] |= ort_selected [i] && ort_selected [(i+1)%3];
            ort_selected  [i] |= rect_selected [(i+1)%3] || rect_selected [(i+2)%3];
            ort_mask [i]       = ort_selected [i] ? 1.0f : 0.0f;
            
            if (ort_selected [i]) ort [i]->SetWireColor (vec3f (0.8f,0.8f,0.0f));
            else                  ort [i]->SetWireColor (ort_info [i].dir);        
            
            if (rect_selected [i]) rect [i]->SetWireColor (vec3f (0.8f,0.8f,0.0f));
            else                   rect [i]->SetWireColor (0.2f);
          }
          break;
        }
        case MOVE:
        {
          vec3f grid_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y),
                delta      = (grid_point - start_grid_point) * ort_mask;
          
          visitor.x=rad2deg(atan(delta.x));
          visitor.z=rad2deg(atan(delta.z));
          visitor.state=VISIT_SET_SHEARS;
          
          for (size_t i=0;i<entities.size ();i++)
          {
            Entity* entity = entities.item (i);
            visitor.old_shears=&old_shears[i];  
            entity->traverse(&visitor);      
          }
          
          break;
        }
      }
    }

    void ChangeSelection ()
    {
      entities.clear ();             
        
      entities = cursor.GetSelection ();
              
      start_tm.resize (entities.size ());            
      
      if (entities.size ())
      {
        basis->identity ();
        basis->translate (entities.item (0)->GetWorldTM ()*vec3f (0.0f));        
        
        state = SELECT;
      }
      else 
      {
        finish ();
        return;
      }
    }

    Viewport*     viewport;
    vector<vec2f> old_shears;
    ShearVisitor  visitor;
    EntityList    entities;
    Entity*       basis;    
    HelperEntity* ort [3];        
    HelperEntity* rect [3];
    vec3f         ort_mask;
    State         state;
    vector<mat4f> start_tm;
    vec4f         start_grid_point;
};

Manipulator* manipCreateShear (RenderCursor& cursor,const char*)
{
  return new ShearManipulator (cursor);
}
