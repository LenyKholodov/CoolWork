#include <render.h>
#include <material.h>
#include <vector>
#include <algorithm>
#include <gl/glu.h>
#include <mesh.h>
#include "draw_stuff.h"
#include "misc.h"

using namespace std;

const int   MAX_LIGHTS_COUNT      = 8;
const float LIGHT_INNER_INTENSITY = 0.2f;

enum RenderModelType
{
  RENDER_PARAM_MODEL,
  RENDER_LANDSCAPE_MODEL
};

struct RenderModel
{
  RenderModelType type;
  union {
    ParamModel*     model;
    LandscapeModel* landscape;
  };
  float           distance;
};

class RenderVisitor: public EntityVisitor
{
  public:
    void visit (Light*);
    void visit (ParamModel*);
    void visit (LandscapeModel*);    
    void visit (HelperEntity*);
    
    typedef vector<RenderModel>   ModelList;
    typedef vector<Light*>        LightList;        
    typedef vector<HelperEntity*> HelperList;
    
    ModelList  solid_models;
    ModelList  blend_models;
    LightList  lights;
    HelperList helpers;
    mat4f      invView;
};

void RenderVisitor::visit (Light* light)
{
  lights.push_back (light);
}

void RenderVisitor::visit (ParamModel* model)
{
  if (!model->IsEnabled (ENTITY_VISIBLE))
    return;
    
  RenderModel render_model;

  render_model.type     = RENDER_PARAM_MODEL;
  render_model.model    = model;
  render_model.distance = (invView * model->GetWorldTM () * vec3f (0.0f)).z;

  if (model->GetMaterial ().GetBlendMode () == MATERIAL_BLEND_SOLID) 
    solid_models.push_back (render_model);
  else
    blend_models.push_back (render_model);
}

void RenderVisitor::visit (LandscapeModel* model)
{
  if (!model->IsEnabled (ENTITY_VISIBLE))
    return;
    
  RenderModel render_model;

  render_model.type      = RENDER_LANDSCAPE_MODEL;
  render_model.landscape = model;
  render_model.distance  = (invView * model->GetWorldTM () * vec3f (0.0f)).z;

//  if (model->GetMaterial ().GetBlendMode () == MATERIAL_BLEND_SOLID) 
    solid_models.push_back (render_model);
//  else
//    blend_models.push_back (render_model);
}

void RenderVisitor::visit (HelperEntity* helper)
{
  helpers.push_back (helper);
}

static void rSetLights (size_t count,Light** light_ptr)
{
  glEnable (GL_LIGHTING);
  
  int i, light_number = 0;

  for (i=0;i<min (MAX_LIGHTS_COUNT,(int)count);light_ptr++,i++)
  {    
    Light& light = **light_ptr;
    
    if (!light.state ())
      continue;    

    glPushMatrix  ();
    glMultMatrixf (transpose (light.GetWorldTM ())[0]);
    
    static float pos [] = {0,0,0,light.GetLightType ()!=LIGHT_DIRECT};
    static float dir [] = {0,0,1,0};
    
    float linear_attenuation = (1.0f-LIGHT_INNER_INTENSITY)/LIGHT_INNER_INTENSITY/light.GetInnerRadius ();

    glEnable  (GL_LIGHT0+light_number);
    glLightfv (GL_LIGHT0+light_number,GL_AMBIENT,vec4f (light.GetLightColor (LIGHT_AMBIENT_COLOR),0.0f));
    glLightfv (GL_LIGHT0+light_number,GL_DIFFUSE,vec4f (light.GetLightColor (LIGHT_DIFFUSE_COLOR),0.0f));
    glLightfv (GL_LIGHT0+light_number,GL_SPECULAR,vec4f (light.GetLightColor (LIGHT_SPECULAR_COLOR),0.0f));    
    glLightfv (GL_LIGHT0+light_number,GL_POSITION,pos);
    glLightfv (GL_LIGHT0+light_number,GL_SPOT_DIRECTION,dir);
    glLightf  (GL_LIGHT0+light_number,GL_CONSTANT_ATTENUATION,1);
    glLightf  (GL_LIGHT0+light_number,GL_LINEAR_ATTENUATION,linear_attenuation);
    glLightf  (GL_LIGHT0+light_number,GL_QUADRATIC_ATTENUATION,0);
    
    if (light.GetLightType () == LIGHT_SPOT)
    {
      glLightf (GL_LIGHT0+light_number,GL_SPOT_CUTOFF,light.GetSpotAngle ());
      glLightf (GL_LIGHT0+light_number,GL_SPOT_EXPONENT,light.GetSpotExponent ());
    }
    else
    {
      glLightf (GL_LIGHT0+light_number,GL_SPOT_CUTOFF,180);
      glLightf (GL_LIGHT0+light_number,GL_SPOT_EXPONENT,0);
    }

    glPopMatrix ();
    
    light_number++;
  }
  
  for (;light_number<MAX_LIGHTS_COUNT;light_number++)
    glDisable (GL_LIGHT0+light_number);
}

static void rSetMaterial (Material& mtl,bool blend_pass)
{
  vec4f color [MATERIAL_COLORS_NUM];
  
  for (int i=0;i<MATERIAL_COLORS_NUM;i++)
    color [i] = vec4f (mtl.GetColor ((MaterialColor)i),1.0f-mtl.GetTransparency ());

  glMaterialfv (GL_FRONT_AND_BACK,GL_AMBIENT,color [MATERIAL_AMBIENT_COLOR]);
  glMaterialfv (GL_FRONT_AND_BACK,GL_DIFFUSE,color [MATERIAL_DIFFUSE_COLOR]);
  glMaterialfv (GL_FRONT_AND_BACK,GL_SPECULAR,color [MATERIAL_SPECULAR_COLOR]);
  glMaterialfv (GL_FRONT_AND_BACK,GL_EMISSION,color [MATERIAL_EMISSION_COLOR]);
  glMaterialf  (GL_FRONT_AND_BACK,GL_SHININESS,mtl.GetShininess ());  
  
//  if (!blend_pass && mtl.IsEnabled (MATERIAL_TWO_SIDED))
    glDisable (GL_CULL_FACE);
    
  switch (mtl.GetBlendMode ())
  {
    case MATERIAL_BLEND_FILTER: glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); break;
    default:    
    case MATERIAL_BLEND_ADD:    glBlendFunc (GL_ONE,GL_ONE); break;
  }
}

static void rDrawSurface (Surface* surface)
{
  glPushClientAttrib  (GL_CLIENT_VERTEX_ARRAY_BIT);
  glEnableClientState (GL_VERTEX_ARRAY);
  glEnableClientState (GL_NORMAL_ARRAY);
//  glEnableClinetState (GL_COLOR_ARRAY);  
  glVertexPointer     (3,GL_FLOAT,sizeof (DrawVertex),&surface->GetVertexes ()->pos);
  glNormalPointer     (GL_FLOAT,sizeof (DrawVertex),&surface->GetVertexes ()->n);
  glColorPointer      (3,GL_FLOAT,sizeof (DrawVertex),&surface->GetVertexes ()->color);
  glDrawElements      (GL_TRIANGLES,surface->GetTrianglesCount ()*3,GL_UNSIGNED_INT,surface->GetTriangles ()->v);
  glPopClientAttrib   ();
}

static void rDrawLandscape (HeightMap& map)
{
  glPushAttrib (GL_ENABLE_BIT);
  glEnable     (GL_AUTO_NORMAL);
  glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);    
//  glPolygonMode (GL_FRONT_AND_BACK,GL_POLYGON);    
  
  glPushMatrix ();    
  glTranslatef (-0.5f,0.0f,-0.5f);
  glScalef     (1.0f/float (map.width),1.0f,1.0f/float (map.height));

  for (size_t i=0;i<map.height-1;i++)
  {
    glBegin (GL_QUAD_STRIP);
    
    float* line [2] = {map.map+i*map.width,map.map+(i+1)*map.width};
    
    for (size_t j=0;j<map.width;j++)
    {
      glVertex3f ((float)j,line [0][j],(float)i);
      glVertex3f ((float)j,line [1][j],(float)(i+1));
    }
    
    glEnd();
  }  

  glPopMatrix ();      
  glPopAttrib ();  
}

static void rDrawAABB (const AABB& bbox,const vec3f& color)
{
  glPushAttrib  (GL_ENABLE_BIT);
  glDisable     (GL_LIGHTING);
  glDisable     (GL_BLEND);
  glDisable     (GL_CULL_FACE);
  rDrawBoundBox (bbox.min,bbox.max,color);
  glPopAttrib   ();
}

static void rDrawModels (Viewport* viewport,size_t count,RenderModel* model_ptr,bool blend_pass)
{  
  glPushMatrix ();
  glScalef     (5,5,5);
//  rDrawAxes    ();
  glPopMatrix  ();
  
  static Material default_material;

  for (size_t i=0;i<count;i++)
  {
    Entity*   object;
    Material* mtl;
    
    switch (model_ptr [i].type)
    {
      case RENDER_PARAM_MODEL:
        object = model_ptr [i].model;
        mtl    = &model_ptr [i].model->GetMaterial ();
        break;
      case RENDER_LANDSCAPE_MODEL:
      {
        object = model_ptr [i].landscape;
        mtl    = &default_material;
        
        default_material.SetColor (MATERIAL_DIFFUSE_COLOR,object->GetWireColor ());
        break;
      }
      default: continue;
    }       
    
    bool wireframe = mtl->IsEnabled (MATERIAL_WIREFRAME) || viewport->IsEnabled (VIEWPORT_WIREFRAME);   
    
    if (wireframe)
    {
      glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
      glDisable     (GL_CULL_FACE);
      glDisable     (GL_LIGHTING);
      
      if (object->IsSelected ())
        glColor3f (1,1,1);
      else
        glColor3fv (object->GetWireColor ());
    }
    else
    {
      glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
//      glEnable      (GL_CULL_FACE);
      glEnable      (GL_LIGHTING);
    }
    
    glPushMatrix  ();
    glMultMatrixf (transpose (object->GetWorldTM ())[0]);    
    glLoadName    ((GLuint)object);
    
    if (!wireframe)
      rSetMaterial (*mtl,blend_pass);
    
    switch (model_ptr [i].type)
    {
      case RENDER_PARAM_MODEL:
      {
        ParamModel& model   = *model_ptr [i].model;
        Surface*    surface = model.GetSurface ();
            
        if (surface)
        {
          rDrawSurface (surface);                
          
          if (model.IsSelected ())
            rDrawAABB (surface->GetAABB (),1.0f);
        }
                
        break;
      }
      case RENDER_LANDSCAPE_MODEL:
      {                
        HeightMap map;
        
        model_ptr [i].landscape->GetHeightMap (map);
        
        rDrawLandscape (map);
        break;
      }
    }                       
    
//    rDrawAxes ();
        
    glPopMatrix ();
  }
  
  glLoadName (0);  
}

inline bool compare_helpers (HelperEntity* a,HelperEntity* b)
{
  return a->GetWorldTM () * vec3f (0.0f).z < b->GetWorldTM () * vec3f (0.0f).z;
}

void rDrawHelpers (Viewport* viewport,size_t count,HelperEntity** helper_ptr)
{
  sort (helper_ptr,helper_ptr+count,compare_helpers);

  for (size_t i=0;i<count;i++)
  {
    HelperEntity& helper = *helper_ptr [i];
    
    if (!helper.IsEnabled (ENTITY_VISIBLE))
      return;
    
    glPushMatrix  ();
    glMultMatrixf (transpose (helper.GetWorldTM ())[0]);
    glLoadName    ((GLuint)&helper);

    switch (helper.GetHelperId ())
    {
      case HELPER_POINT:
        rDrawPoint (0.0f,helper.GetWireColor ());
        break;
      case HELPER_ARROW:
        rDrawArrow (0.0f,vec3f (0,0,1),helper.GetWireColor ());
        break;
      case HELPER_RECT:
        rDrawRect (0.0f,vec3f (0,0,1),helper.GetWireColor ());
        break;
      case HELPER_AXES:
        rDrawAxes ();
        break;
      case HELPER_ARC:
        rDrawArc (0.0f, vec3f (0, 0, 1), helper.GetWireColor (), 20);
        break;
      case HELPER_SECTOR:
        rDrawSector (0.0f, vec3f (0, 0, 1), helper.GetWireColor (), 20, helper.GetHelperAngle());
        break;
      case HELPER_CIRCLE:
        rDrawCircle (0.0f, vec3f (0, 0, 1), helper.GetWireColor (), 40);
        break;
      case HELPER_LINE:
        rDrawLine (0.0f,vec3f (0.0f,0.0f,1.0f),helper.GetWireColor ());
        break;
    }

    glPopMatrix ();      
  }  
  
  glLoadName (0);    
}

static void rDrawLightHelpers (Camera* camera,size_t count,Light** light_ptr)
{
  glPushAttrib (GL_ENABLE_BIT);
  glDisable    (GL_LIGHTING);
  glLoadName   (0);
  
  for (int i=0;i<min (MAX_LIGHTS_COUNT,(int)count);i++,light_ptr++)
  {
    Light& light = **light_ptr;
    
    if (!light.IsEnabled (ENTITY_VISIBLE))
      continue;
    
    glPushMatrix     ();    
    glMultMatrixf    (transpose (light.GetWorldTM ())[0]);
    glLoadName       ((GLuint)&light);    
    rDrawBoundSphere (0.0f,1.0f,light.IsSelected () ? vec3f (1.0f) : light.GetWireColor (),16);    
    glPopMatrix      ();
  }
  
  glPopAttrib ();
}

inline bool back_to_front (RenderModel& model1,RenderModel& model2)
{
  return model1.distance > model2.distance;
}

void rDrawScene (Viewport* viewport,Scene* scene,Camera* camera,size_t flags)
{
  if (!scene)
    return;
    
  glEnable   (GL_RESCALE_NORMAL);
  glEnable   (GL_NORMALIZE);
  glEnable   (GL_DEPTH_TEST);
  glDisable  (GL_BLEND);
  glFrontFace(GL_CW);  
  glCullFace (GL_BACK);  

  RenderVisitor visitor;
  
  visitor.invView = camera ? invert (camera->GetWorldTM ()) : 1.0f;

  scene->root ()->traverse (&visitor);  
    
  if (viewport->IsEnabled (VIEWPORT_WIREFRAME))
    glDisable (GL_DEPTH_TEST);  

  if (camera)
  {
    glMatrixMode  (GL_PROJECTION);
    glMultMatrixf (transpose (camera->GetProjTM ())[0]);
    glMatrixMode  (GL_MODELVIEW);
    glMultMatrixf (transpose (invert (camera->GetWorldTM ()))[0]);
  }
  
  if (!viewport->IsEnabled (VIEWPORT_WIREFRAME) && (flags & STUFF_DRAW_LIGHTS)) 
    rSetLights (visitor.lights.size (),visitor.lights.size ()?&visitor.lights [0]:NULL);
  else
    glDisable (GL_LIGHTING);  

  if (flags & STUFF_DRAW_OBJECTS)
  {    
    rDrawModels (viewport,visitor.solid_models.size (),visitor.solid_models.size ()?&visitor.solid_models [0]:NULL,false);
    sort        (visitor.blend_models.begin (),visitor.blend_models.end (),back_to_front);
    
    glEnable    (GL_BLEND);
    glDepthMask (GL_FALSE);
    glCullFace  (GL_FRONT);
    rDrawModels (viewport,visitor.blend_models.size (),visitor.blend_models.size ()?&visitor.blend_models [0]:NULL,true);
    glCullFace  (GL_BACK);
    rDrawModels (viewport,visitor.blend_models.size (),visitor.blend_models.size ()?&visitor.blend_models [0]:NULL,true);
    glDepthMask (GL_TRUE);
    glDisable   (GL_BLEND);
  }
    
  glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);  
    
  if (flags & STUFF_DRAW_HELPERS)
  {
    glDisable    (GL_LIGHTING);    
    rDrawHelpers (viewport,visitor.helpers.size (),visitor.helpers.size ()?&visitor.helpers [0]:NULL);    
  }

  if (flags & STUFF_DRAW_OBJECTS)  
    rDrawLightHelpers (camera,visitor.lights.size (),visitor.lights.size ()?&visitor.lights [0]:NULL); 
}
