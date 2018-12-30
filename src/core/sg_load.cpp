#include <scene.h>
#include <material.h>
#include <parser.h>

inline const char* attrs (ParseNode* p,const char* tag,const char* def_value="")
{
  if (!p)
    return def_value;

  p = p->first (tag);
  
  return p && p->argc () ? p->arg (0) : def_value;
}

inline float attrf (ParseNode* p,const char* tag,float def_value=0.0f)
{
  if (!p)
    return def_value;

  p = p->first (tag);
  
  return p && p->argc () ? (float)atof (p->arg (0)) : def_value;  
}

inline int attri (ParseNode* p,const char* tag,int def_value=0)
{
  if (!p)
    return def_value;

  p = p->first (tag);
  
  return p && p->argc () ? atoi (p->arg (0)) : def_value;  
}

inline vec3f attr3f (ParseNode* p,const char* tag,const vec3f& def_value=0.0f)
{
  if (!p)
    return def_value;

  p = p->first (tag);
  
  return p && p->argc () >= 3 ? vec3f ((float)atof (p->arg (0)),(float)atof (p->arg (1)),(float)atof (p->arg (2))) : def_value;
}

static bool is_value (const char* s)
{
  if (!isdigit (*s) && *s != '-' && *s != '+')
    return false;
    
  s++;  

  while (isdigit (*s) || *s == '.' || *s == '-' || *s == '+' || *s == 'e') s++;
  
  return *s == 0;
}

static void read_entity_param3f (ParseNode* p,const char* tag,Entity* entity,const char* pname)
{
  p = p->first (tag);
  
  if (!p || !p->argc ())
    return;

  if (p->argc () >= 3 && is_value (p->arg (0)) && is_value (p->arg (1)) && is_value (p->arg (2)))
  {
    vec3f v ((float)atof (p->arg (0)),(float)atof (p->arg (1)),(float)atof (p->arg (2)));    
    
    Command::instance ().etc ().set (entity->property (pname),v);
  }
  else entity->SetExpression (pname,p->arg (0));
}

static void read_entity_param3f (ParseNode* p,Entity* entity,const char* pname)
{
  read_entity_param3f (p,pname,entity,pname);
}

static void read_entity_paramf (ParseNode* p,const char* tag,Entity* entity,const char* pname)
{
  p = p->first (tag);
  
  if (!p || !p->argc ())
    return;

  if (is_value (p->arg (0)))
  {    
    Command::instance ().etc ().set (entity->property (pname),(float)atof (p->arg (0)));
  }
  else entity->SetExpression (pname,p->arg (0));
}

static void read_entity_paramf (ParseNode* p,Entity* entity,const char* pname)
{
  read_entity_paramf (p,pname,entity,pname);
}

static void read_entity_parami (ParseNode* p,const char* tag,Entity* entity,const char* pname)
{
  read_entity_paramf (p,tag,entity,pname);
}

static void read_entity_parami (ParseNode* p,Entity* entity,const char* pname)
{
  read_entity_parami (p,pname,entity,pname);
}

static bool test_entity_param (ParseNode* p,const char* tag)
{
  return p->first (tag) != NULL;
}

static Light* parse_light (ParseNode* p,Entity* parent)
{
  const char* type = attrs (p,"type");
  
  if (!*type)
  {
    p->error ("No light type specified");
    return NULL;
  }
  
  Light* obj  = parent->scene ()->CreateLight ();  
  
  if      (!strcmp (type,"point"))  obj->SetPointLight (0.0f);
  else if (!strcmp (type,"direct")) obj->SetDirectLight (vec3f (0.0f,0.0f,1.0f));
  else if (!strcmp (type,"spot"))   obj->SetSpotLight (0.0f,vec3f (0.0f,0.0f,1.0f));
  else
  {
    p->error ("Unknown light type '%s'",type);
    obj->release ();
    return NULL;
  }    
  
  obj->SetLightColor (LIGHT_AMBIENT_COLOR,attr3f (p,"ambient"));
  obj->SetLightColor (LIGHT_DIFFUSE_COLOR,attr3f (p,"diffuse",0.8f));
  obj->SetLightColor (LIGHT_SPECULAR_COLOR,attr3f (p,"specular"));
    
  if (test_entity_param (p,"disable"))  
    obj->off ();
    
  obj->SetSpotExponent (attrf (p,"spot_exponent"));

  if (p->first ("radius"))
    obj->SetInnerRadius (attrf (p,"radius"));  

  if (p->first ("spot_angle"))    
    obj->SetSpotAngle (attrf (p,"spot_angle",45.0f));      
    
  return obj;  
}

static Camera* parse_camera (ParseNode* p,Entity* parent)
{
  const char* type = attrs (p,"projection");
  
  if (!*type)
  {
    p->error ("No projection specified");
    return NULL;
  }
  
  Camera* obj = parent->scene ()->CreateCamera ();  
  
  const char* valid_proj_type [] = {"ortho","isometry","dimetry","trimetry","cabinet","free",
                                    "perspective","perspective_special"};
  size_t valid_count = sizeof (valid_proj_type)/sizeof (const char*);
  
  for (size_t i=0;i<valid_count;i++)
    if (!strcmp (type,valid_proj_type [i]))
      break;
      
  if (i == valid_count)
  {
    p->error ("Unknown projection '%s'",type);
    obj->release ();
    return NULL;
  }
  
  Command::instance ().etc ().set (obj->property ("projection"),type);    
    
  read_entity_paramf  (p,obj,"left");
  read_entity_paramf  (p,obj,"right");
  read_entity_paramf  (p,obj,"top");
  read_entity_paramf  (p,obj,"bottom");
  read_entity_paramf  (p,obj,"znear");
  read_entity_paramf  (p,obj,"zfar");
  read_entity_paramf  (p,obj,"alpha");
  read_entity_paramf  (p,obj,"beta");
  read_entity_param3f (p,obj,"distance");  
  read_entity_param3f (p,obj,"point1");    
  read_entity_param3f (p,obj,"point2");
  read_entity_param3f (p,obj,"point3");    
  
  return obj;
}

static void parse_material (ParseNode* p,Material& mtl)
{
  mtl.SetColor (MATERIAL_AMBIENT_COLOR,attr3f (p,"ambient"));
  mtl.SetColor (MATERIAL_DIFFUSE_COLOR,attr3f (p,"diffuse",0.8f));  
  mtl.SetColor (MATERIAL_SPECULAR_COLOR,attr3f (p,"specular"));
  mtl.SetColor (MATERIAL_EMISSION_COLOR,attr3f (p,"emission"));
  
  mtl.SetTransparency (attrf (p,"transparency"));
  mtl.SetShininess    (attrf (p,"shininess"));
  
  if (p->first ("two_sided")) mtl.enable (MATERIAL_TWO_SIDED);
  if (p->first ("wireframe")) mtl.enable (MATERIAL_WIREFRAME);
  
  const char* blend_mode = attrs (p,"blend","solid");
  
  if      (!strcmp (blend_mode,"solid"))  mtl.SetBlendMode (MATERIAL_BLEND_SOLID);
  else if (!strcmp (blend_mode,"filter")) mtl.SetBlendMode (MATERIAL_BLEND_FILTER);
  else if (!strcmp (blend_mode,"add"))    mtl.SetBlendMode (MATERIAL_BLEND_ADD);
}

static ParamModel* parse_model (ParseNode* p,Entity* parent,ParamModelType type)
{
  ParamModel* obj = parent->scene ()->CreateModel ();
  
  obj->SetModelType (type);  
  
  switch (type)
  {
    case MODEL_BOX:
      read_entity_paramf (p,obj,"width");
      read_entity_paramf (p,obj,"height");
      read_entity_paramf (p,obj,"depth");    
      break;
    case MODEL_HALF_SPHERE:
    case MODEL_SPHERE:
      read_entity_paramf (p,obj,"radius");
      read_entity_parami (p,obj,"parallels");
      read_entity_parami (p,obj,"meridians");    
      break;
    case MODEL_HALF_CYLINDER:
    case MODEL_CYLINDER:
      read_entity_paramf (p,obj,"radius");
      read_entity_paramf (p,obj,"height");
      read_entity_parami (p,obj,"slices");    
      break;
    case MODEL_HALF_CONE:
    case MODEL_CONE:
      read_entity_paramf (p,obj,"radius");
      read_entity_paramf (p,obj,"radius2");
      read_entity_paramf (p,obj,"height");  
      read_entity_parami (p,obj,"slices");
      break;
    case MODEL_TETRAHEDRON:
    case MODEL_HEXAHEDRON:
    case MODEL_OCTAHEDRON:
    case MODEL_DODECAHEDRON:
    case MODEL_ICOSAHEDRON:   
      read_entity_paramf (p,obj,"radius");
      break;
    case MODEL_HALF_TORUS:
    case MODEL_TORUS:
      read_entity_paramf (p,obj,"radius");
      read_entity_paramf (p,obj,"radius2");    
      read_entity_parami (p,obj,"parallels");
      read_entity_parami (p,obj,"meridians");          
      break;
  }
  
  read_entity_paramf (p,obj,"shear_x");
  read_entity_paramf (p,obj,"shear_z");  
  
  ParseNode* mtl_node = p->first ("material");
  
  if (mtl_node)
  {
    ParseNode* next_node = mtl_node->next ("material");
    
    if (next_node)
      next_node->error ("Model doesn't support multi materials");
      
    parse_material (mtl_node,obj->GetMaterial ());  
  }  
  
  return obj;
}

static void parse_node_list (ParseNode* p,Entity* parent);

static void parse_node (ParseNode* p,Entity* parent)
{
  const char* type = p->tag ();
  Entity*     obj  = NULL;
    
  if      (!strcmp (type,"node"))         obj = parent->scene ()->CreateEntity ();
  else if (!strcmp (type,"light"))        obj = parse_light   (p,parent);
  else if (!strcmp (type,"camera"))       obj = parse_camera  (p,parent);
  else if (!strcmp (type,"box"))          obj = parse_model   (p,parent,MODEL_BOX);
  else if (!strcmp (type,"sphere"))       obj = parse_model   (p,parent,MODEL_SPHERE);
  else if (!strcmp (type,"halfsphere"))   obj = parse_model   (p,parent,MODEL_HALF_SPHERE);  
  else if (!strcmp (type,"cylinder"))     obj = parse_model   (p,parent,MODEL_CYLINDER);  
  else if (!strcmp (type,"halfcylinder")) obj = parse_model   (p,parent,MODEL_HALF_CYLINDER);
  else if (!strcmp (type,"cone"))         obj = parse_model   (p,parent,MODEL_CONE);
  else if (!strcmp (type,"halfcone"))     obj = parse_model   (p,parent,MODEL_HALF_CONE);
  else if (!strcmp (type,"tetrahedron"))  obj = parse_model   (p,parent,MODEL_TETRAHEDRON);
  else if (!strcmp (type,"hexahedron"))   obj = parse_model   (p,parent,MODEL_HEXAHEDRON);
  else if (!strcmp (type,"octahedron"))   obj = parse_model   (p,parent,MODEL_OCTAHEDRON);
  else if (!strcmp (type,"dodecahedron")) obj = parse_model   (p,parent,MODEL_DODECAHEDRON);
  else if (!strcmp (type,"icosahedron"))  obj = parse_model   (p,parent,MODEL_ICOSAHEDRON);
  else if (!strcmp (type,"torus"))        obj = parse_model   (p,parent,MODEL_TORUS);
  else if (!strcmp (type,"halftorus"))    obj = parse_model   (p,parent,MODEL_HALF_TORUS);
//  else if (p->first ())                   p->error ("Wrong object type '%s'",type);
  
  if (obj)
  {            
    if (p->argc ())  
      obj->rename (p->arg (0));      
      
    obj->bind (parent);
    
    read_entity_param3f (p,obj,"wire_color");
    
//    vec3f pos = attr3f (p,"local_position"),
//          rot = attr3f (p,"local_rotation"),
//          scale = attr3f (p,"local_scale");
          
//    obj->identity ();
//    obj->transform (translate (pos) * rotatef (deg2rad (rot.z),0,0,1) * 
//        rotatef (deg2rad (rot.y),0,1,0) * rotatef (deg2rad (rot.x),1,0,0) * ::scale (scale));
    
    read_entity_param3f (p,obj,"local_scale");
    read_entity_param3f (p,obj,"local_rotation");            
    read_entity_param3f (p,obj,"local_position");
        
    if (test_entity_param (p,"hidden")) obj->disable (ENTITY_VISIBLE);
    
    parse_node_list (p,obj);    
  }  
}

static void parse_node_list (ParseNode* p,Entity* parent)
{
  for (ParseNode* i=p->first ();i;i=i->next ())
    parse_node (i,parent);
}

bool Scene::load (const char* file_name)
{
  if (!file_name)
    return false;

  Parser p (file_name);  
  
  ParseNode* scene_node = p.root ()->first ("scene");
  
  if (scene_node)
  {
    ParseNode* next = scene_node->next ("scene");
    
    if (next)
      next->warning ("More than one scene in file");

    parse_node_list (scene_node,root ());        
  }
  else p.root ()->error ("Scene not found");
  
  const char* active_camera = attrs (p.root (),"active_camera");
  
  if (active_camera)
    Command::instance ().execf ("camera set '%s'",active_camera);
    
  logPrintf ("Parse file '%s':",file_name);
  
  if (p.log ())
    logPrintf ("%s",p.log ());  
  
  return true;
}
