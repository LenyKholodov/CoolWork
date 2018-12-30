#include "manipulators.h"
#include <algorithm>

using namespace std;

#define ROT_SPEED 0.2f

const float EPS = 0.01f;

struct CircleInfo
{
  vec3f       dir;
  vec3f       up;
  const char* name;
};

static CircleInfo circle_info [] = {
  {vec3f (0,1,0),vec3f (1,0,1),"1"},
  {vec3f (0,1,0),vec3f (1,0,1),"2"}
};

struct ArcInfo
{
  vec3f       dir;
  vec3f       up;
  const char* name;
};

static ArcInfo arc_info [] = {
  {vec3f (0,0,1),vec3f (1,0,0),"X"},
  {vec3f (1,0,1),vec3f (0,1,0),"Y"},
  {vec3f (1,0,0),vec3f (0,0,1),"Z"}
};

struct OrtInfo
{
  vec3f       dir;
  vec3f       up;
  const char* name;
};

static OrtInfo ort_info [] = {
  {vec3f (1,0,0),vec3f (0,1,0),"X"},
  {vec3f (0,1,0),vec3f (1,0,0),"Y"},
  {vec3f (0,0,1),vec3f (0,1,0),"Z"}
};

class RotateManipulator: public Manipulator
{
  public:
    enum State {
      WAIT,
      SELECT,
      MOVE      
    };
    
    struct Error {};
    
    RotateManipulator (RenderCursor& cursor) : Manipulator (cursor)
    {
      viewport = cursor.GetActiveViewport ();
      scene = cursor.GetActiveScene ();
      Camera* camera = viewport->GetCamera ();
          
      if (!camera)
        throw Error ();          

      if (!scene)
        throw Error ();
        
      cursor.NewSelection ();

      basis = scene->CreateEntity ();
      
      basis->enable (ENTITY_TEMP);

      for (int i=0; i < 3; i++)
      {
        ort [i] = scene->CreateHelper ();
       
        ort [i]->SetLine      (0.0f,ort_info [i].dir);
        ort [i]->SetWireColor (ort_info [i].dir);
        ort [i]->bind         (basis);
        ort [i]->rename       (ort_info [i].name);        
        ort [i]->enable       (ENTITY_TEMP);

        arc [i] = scene->CreateHelper ();
        
        arc [i]->SetArc       (0.0f,arc_info [i].dir,arc_info [i].up);
        arc [i]->SetWireColor (arc_info [i].dir);
        arc [i]->scale        (2.5f);
        arc [i]->bind         (basis);
        arc [i]->rename       (arc_info [i].name);        
        arc [i]->enable       (ENTITY_TEMP);
                  
        helper_mask[i] = false;
      }

      for (int i = 0; i < 2; i++)
      {
        circle [i] = scene->CreateHelper ();
       
        circle [i]->SetCircle    (0.0f,circle_info[i].dir, circle_info[i].up);
        circle [i]->SetWireColor (vec3f(0.3f,0.3f,0.3f) + vec3f(0.4f,0.4f,0.4f) * (float)i);
        circle [i]->bind         (basis);
        circle [i]->rename       (circle_info [i].name);        
        circle [i]->enable       (ENTITY_TEMP);

        helper_mask[i+3] = false;
      }

      circle [0]->scale(2.5f);
      circle [1]->scale(3.5f);
      
      state = WAIT;

      tan[0] = tan[1] = sector = NULL;

      ChangeSelection ();
    }
    
    ~RotateManipulator ()
    {
      for (int i=0;i<3;i++)
      {
        if (arc[i]) arc [i]->release ();
        if (ort[i]) ort [i]->release ();
        if (i < 2) 
        {
          circle[i]->release ();
          if (tan[i]) 
            tan[i]->release();
        }
      }

      if (sector) sector->release ();
      if (basis)  basis->release ();
    }
    
    void select ()
    {
      if (!(helper_mask[0] || helper_mask[1] || helper_mask[2] || helper_mask[3] || helper_mask[4]) || !entities.size ())
      {
        ChangeSelection ();
        return;
      }

      old_pos = cursor.GetPosition ();
      
      for (size_t i=0;i<entities.size ();i++)      
        start_tm [i] = entities.item (i)->GetWorldTM ();

      state   = MOVE;

      angle_tangent = angle_yaw = angle_roll = 0;

      if (!(helper_mask[0] || helper_mask[1] || helper_mask[2]))
        return;

      vec3f center, world_start, start_dir, end_point;
      int j = 0;

      center = basis->GetWorldTM() * vec3f(0);

      for (; j < 3; j++)
        if (helper_mask [j]) break;

      world_start = viewport->ProjectPointOnPlane((int)start_point.x, (int)start_point.y, center, arc_info[j].up);
      start_dir = world_start - center;
      arc_ort = cross(arc_info[j].up, start_dir);
      arc_ort = normalize (arc_ort) * 0.75;
      rot_coefficient = viewport->ProjectWorldPoint (vec3f(0.f,0.f,0.f)) - viewport->ProjectWorldPoint (arc_ort);
      rot_coefficient = normalize(rot_coefficient);

      for(int i = 0; i < 2; i++)
      {
        tan [i] = scene->CreateHelper ();

        if(i)
          end_point = vec3f(world_start.x - arc_ort.x, world_start.y - arc_ort.y, world_start.z - arc_ort.z);
        else
          end_point = vec3f(world_start.x + arc_ort.x, world_start.y + arc_ort.y, world_start.z + arc_ort.z);
        
        tan [i]->SetLine      (world_start, end_point);
        tan [i]->SetWireColor (vec3f(0.3f,0.3f,0.3f));
        tan [i]->enable       (ENTITY_TEMP);
      }
    
      sector = scene->CreateHelper ();
      
      sector->SetSector    (0.0f, -arc_ort, arc_info[j].up, 0);
      sector->bind         (basis);
      sector->SetWireColor (ort_info[j].dir);
      sector->enable       (ENTITY_TEMP);
      sector->scale        (2.5f);
    }
    
    void accept ()
    {
      state = SELECT;
      
      for (int i=0;i<3;i++)
        helper_mask [i] = false;

      for (int i = 0; i < 2; i++)
        if(tan[i])
        {
          tan[i]->release();
          tan[i] = NULL;
        }
  
      if(sector)
      {
        sector->release();
        sector = NULL;
      }
    }
    
    void process (int x,int y)
    {
      switch (state)
      {
        case WAIT:
        case SELECT:
        {
          EntityList selection;
          bool find = false;
          
          viewport->select (selection,cursor.GetPosition ().x,cursor.GetPosition ().y,4,4,SELECT_HELPERS);
      
          start_point = 0;

          for (int i=0;i<5;i++)      
            helper_mask [i] = false;
                
          for (size_t i=0;i<selection.size ();i++)
          {
            for (int j=0;j<3;j++)
            {
              if      (selection.item (i) == arc [j])
              {
                helper_mask [j] = true;
                find = true;

                start_point = vec3f((float)cursor.GetPosition ().x, (float)cursor.GetPosition ().y, 0);
              }
              if      (selection.item (i) == circle [min(j,1)])
              {
                helper_mask [j+3] = true;
                find = true;
              }
            }
            if (find) break;
          }
              
          for (int i=0;i<3;i++)
          {
            if (helper_mask [i])
            {
              arc [i]->SetWireColor (vec3f (0.8f,0.8f,0.0f));
              ort [i]->SetWireColor (ort_info [i].dir);
            }
            else
            {
              arc [i]->SetWireColor (ort_info [i].dir);        
              ort [i]->SetWireColor (vec3f (0.6f,0.6f,0.6f));
            }
          }

          for (int i=0;i<2;i++)
          {
            if (helper_mask [i+3])
              circle [i]->SetWireColor (vec3f (0.8f,0.8f,0.0f));
            else
              circle [i]->SetWireColor (vec3f(0.3f,0.3f,0.3f) + vec3f(0.4f,0.4f,0.4f) * (float)i);        
          }

          break;          
        }
        case MOVE:
        {                
          float angle, x_dif, y_dif;          
          float tangent = 0.f, yaw = 0.f, roll = 0.f;
          
          x_dif = (float)(cursor.GetPosition ().x - old_pos.x);
	  y_dif = (float)(cursor.GetPosition ().y - old_pos.y);
          
          if(helper_mask[0] || helper_mask[1] || helper_mask[2])
            angle = (x_dif * rot_coefficient.x - y_dif * rot_coefficient.y) * ROT_SPEED;
          else
            angle = (x_dif - y_dif) * ROT_SPEED;

          for (int i=0;i<5;i++)
            if (helper_mask [i])
            {
              switch (i)
              {
                case 0: 
                  tangent = angle;
                  break;
                case 1: 
                  yaw = angle;
                  break;
                case 2: 
                  roll = angle;
                  break;
                case 3:
                  tangent += (x_dif - y_dif) * ROT_SPEED * 0.7f;
                  roll    += (x_dif + y_dif) * ROT_SPEED * 0.7f;
                  break;
                case 4:
                  yaw     += angle;
                  roll    += angle;
                  break;
              };
            }
            
          angle_tangent += tangent;
          angle_yaw += yaw;
          angle_roll += roll;
          
          old_pos = cursor.GetPosition ();            

          if (tan[0])
            if(angle_tangent < 0 || angle_yaw < 0 || angle_roll < 0)
            {
              tan[0]->SetWireColor (vec3f (0.8f,0.8f,0.0f));
              tan[1]->SetWireColor (vec3f (0.3f,0.3f,0.3f)); 
            }
            else
            {
              tan[0]->SetWireColor (vec3f (0.3f,0.3f,0.3f)); 
              tan[1]->SetWireColor (vec3f (0.8f,0.8f,0.0f)); 
            }          
      
          if (helper_mask[0])
            sector->SetHelperAngle(angle_tangent);
            else if (helper_mask[1])
              sector->SetHelperAngle(angle_yaw);
              else if (helper_mask[2])
                sector->SetHelperAngle(angle_roll);
       
          for (size_t i=0; i<entities.size(); i++)
          {
            Entity* entity = entities.item (i);
            
            vec3f pos, rot, scale;

            affine_decomp (start_tm[i], pos, rot, scale);

            entity->identity  ();
//            entity->transform (start_tm[i]);
            entity->translate (pos);
//            entity->rotate (rot.x, rot.y, rot.z);
            entity->rotate (angle_tangent, angle_yaw, angle_roll);            
            entity->rotate (0, 0, -rot.z);
            entity->rotate (0, -rot.y, 0);
            entity->rotate (-rot.x, 0, 0);
            entity->scale  (scale);
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
    Entity*	  basis;
    Scene* 	  scene;
    HelperEntity  *arc [3], *ort [3], *circle[2], *tan[2], *sector;        
    State         state;
    vec2i         old_pos;
    vec3f	  start_point, arc_ort, rot_coefficient;
    vector<mat4f> start_tm;
    float	  angle_tangent, angle_yaw, angle_roll;
    bool          helper_mask[5];
};

Manipulator* manipCreateRotate (RenderCursor& cursor,const char*)
{
  return new RotateManipulator (cursor);
}
