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

const float EPS = 0.01f;

class ScaleManipulator: public Manipulator
{
  public:
    enum State {
      WAIT,
      SELECT,
      SCALE      
    };
    
    struct Error {};
    
    ScaleManipulator (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport = cursor.GetActiveViewport ();
      Scene* scene = cursor.GetActiveScene ();
      Camera* camera = viewport->GetCamera ();
          
      if (!camera)
        throw Error ();          

      if (!scene)
        throw Error ();
        
      cursor.NewSelection ();

      ort_mask = 0.0f;
      
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

        ort_mask[i] = 0;
      }            
      
      state = WAIT;

      ChangeSelection ();
    }
    
    ~ScaleManipulator ()
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
      if (equal (ort_mask,vec3f (0.0f),EPS) || !entities.size ())
      {
        ChangeSelection ();
       
        return;
      }

      if (equal (ort_mask,vec3f (0.0f),EPS))
      {
        if (state != WAIT)
          cursor.SetManipulator ("select");
        
        return;
      }
      
      for (size_t i=0;i<entities.size ();i++)      
        start_tm [i] = entities.item (i)->GetWorldTM ();
        
//      start_grid_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y);
      start_grid_point = viewport->ProjectPointOnPlane (cursor.GetPosition ().x,cursor.GetPosition ().y,
                         entities.item (0)->GetWorldTM ());      
      state            = SCALE;
    }
    
    void accept ()
    {
      state = SELECT;
      
      for (int i=0;i<3;i++)
        ort_mask [i] = 0.0f;
    }
    
    void process (int x,int y)
    {
      switch (state)
      {
        case WAIT:
        case SELECT:
        {
          EntityList selection;
          
          viewport->select (selection,cursor.GetPosition ().x,cursor.GetPosition ().y,4,4,SELECT_HELPERS);
          
          bool ort_selected [3];          
          bool rect_selected [3] = {false,false,false};                
          
          for (int i=0;i<3;i++)      
            ort_selected [i] = false;
                
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
        case SCALE:
        {
//          vec3f grid_point = viewport->ProjectPointOnGrid (cursor.GetPosition ().x,cursor.GetPosition ().y),
//                delta      = 1;
          vec3f grid_point = viewport->ProjectPointOnPlane (x,y,start_tm [0]),
                delta      = 1;
          mat4f local_tm;   

          delta += (grid_point - start_grid_point) * ort_mask;

          for (size_t i=0;i<entities.size ();i++)
          {
            Entity* entity = entities.item (i);
                  
            entity->identity  ();
            entity->transform (invert (entity->GetWorldTM ())* start_tm [i] * scale(delta));
          }

          basis->identity ();
          basis->translate (entities.item (0)->GetWorldTM ()*vec3f (0.0f));

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
    EntityList    entities;    
    Entity*   basis;
    HelperEntity* ort [3];        
    HelperEntity* rect [3];
    vec3f         ort_mask;
    State         state;
    vector<mat4f> start_tm;
    vec4f         start_grid_point;
};

Manipulator* manipCreateScale (RenderCursor& cursor,const char*)
{
  return new ScaleManipulator (cursor);
}
