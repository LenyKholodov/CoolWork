#include <scene.h>
#include <material.h>
#include <stdio.h>

class WXFFile
{
  public:
    struct Error {};
  
    WXFFile  (const char* name);
    ~WXFFile ();
    
    void begin (size_t level);
    void begin (const char* tag) { begin (tag,""); }
    void begin (const char* tag,const char* format,...);
    void begin ();
    void end   ();
    
    void comment (const char*);
    
    void write (const char* tag);    
    void write (const char* tag,const char* str,bool identifier=true);
    void write (const char* tag,int value);
    void write (const char* tag,float value);
    void write (const char* tag,const vec3f& value);    
    
    void writei  (const char* tag,Entity*,const char* property);
    void writei  (const char* tag,Entity*);        
    void writef  (const char* tag,Entity*,const char* property);
    void writef  (const char* tag,Entity*);    
    void write3f (const char* tag,Entity*,const char* property);
    void write3f (const char* tag,Entity*);    

//    template <class T>
//    void write (const char* tag,size_t count,const T* array,size_t stride=1);    
    
  private:
    void _write   (const char*);
    void _writef  (const char*,...);
    void _vwritef (const char*,va_list);          
    void _write_tabs (size_t len);
    
  private:
    FILE*  file;
    size_t level, pos;
};

class SceneSaveVisitor: public EntityVisitor
{
  public:
    SceneSaveVisitor (const char* file_name);
  
  private:    
    void visit (Entity* obj)        { _save (obj); }
    void visit (Light* obj)         { _save (obj); }
    void visit (Camera* obj)        { _save (obj); }
    void visit (ParamModel* obj)    { _save (obj); }
    void visit (HelperEntity* obj)  { _save (obj); }
        
    template <class T>
    void _save (T*);
    
    template <class T>    
    void save (T*) {}
    
    template <class T>
    const char* get_type (T*);    
  
    void save (Entity*) {}
    void save (Light*);
    void save (Camera*);
    void save (ParamModel*);
    void save (HelperEntity*);    
    void save (Material&);
    
  private:
    WXFFile os;    
    size_t cur_depth;
};

/*
    WXFFile
*/

WXFFile::WXFFile (const char* name)
{
  file = fopen (name,"wt");
  
  if (!file)
    throw Error ();
    
  level = 0;
  pos   = 0;  
}

WXFFile::~WXFFile ()
{
  while (level)
    end ();

  fclose (file);    
}

void WXFFile::_write_tabs (size_t len)
{
  static char tab_buf [128];
  static bool buf_init = false;
  
  if (!buf_init)
  {
    memset (tab_buf,' ',sizeof (tab_buf));
    buf_init = true;
  }
  
  len *= 2;
  
  while (len)
  {     
    size_t wlen = len < sizeof (tab_buf) ? len : sizeof (tab_buf);

    fwrite (tab_buf,1,wlen,file);

    len -= wlen;
  }  
}

void WXFFile::_write (const char* s)
{
  for (const char* first=s;*s;s++)
  {
    if (!pos)
      _write_tabs (pos=level);

    switch (*s)
    {
      case '\n':
      {
        fwrite (first,1,s-first+1,file);
        first = s+1;
        pos   = 0;
        break;
      } 
      default:
        pos++;
        break;
    }
  }

  fwrite (first,1,s-first,file);
}

void WXFFile::_vwritef (const char* format,va_list list)
{
  char default_buf [128], *buf = default_buf;
  
  if (_vsnprintf (buf,sizeof (default_buf),format,list) >= sizeof (default_buf))
    vsprintf (buf=(char*)_alloca (_vscprintf (format,list)),format,list);  

  _write (buf);
}

void WXFFile::_writef (const char* format,...)
{
  va_list list;
  
  va_start (list,format);    
  _vwritef (format,list);
}

void WXFFile::begin ()
{
  _write ("\n{");
  level++;
  _write ("\n");
}

void WXFFile::begin (const char* tag,const char* format,...)
{
  va_list list;
  
  va_start (list,format);  
  _writef  ("%s ",tag);
  _vwritef (format,list);
  begin    ();
}

void WXFFile::begin (size_t new_level)
{
  while (new_level > level) begin ();
  while (new_level < level) end ();
}

void WXFFile::end ()
{
  if (level)
  {
    level--;
    _write ("}\n");
  }
}

void WXFFile::write (const char* tag)
{
  _writef ("%s\n",tag);
}

void WXFFile::write (const char* tag,const char* str,bool identifier)
{
  _writef (identifier?"%s %s\n":"%s '%s'\n",tag,str);
}

void WXFFile::write (const char* tag,int value)
{
  _writef ("%s %d\n",tag,value);
}

void WXFFile::write (const char* tag,float value)
{
  _writef ("%s %.3f\n",tag,value);
}

void WXFFile::write (const char* tag,const vec3f& value)
{
  _writef ("%s %.3f %.3f %.3f\n",tag,value [0],value [1],value [2]);
}

void WXFFile::writei (const char* tag,Entity* obj,const char* pname)
{
  Etc&        etc        = Command::instance ().etc ();
  const char* expression = etc.get_expression (obj->property (pname));

  if (*expression) _writef ("%s '%s'\n",tag?tag:pname,expression);
  else             _writef ("%s %d\n",tag?tag:pname,etc.geti (obj->property (pname)));
}

void WXFFile::writei (const char* tag,Entity* obj)
{
  writei (tag,obj,tag);
}

void WXFFile::writef (const char* tag,Entity* obj,const char* pname)
{
  Etc&        etc        = Command::instance ().etc ();
  const char* expression = etc.get_expression (obj->property (pname));

  if (*expression) _writef ("%s '%s'\n",tag?tag:pname,expression);
  else             _writef ("%s %.3f\n",tag?tag:pname,etc.getf (obj->property (pname)));
}

void WXFFile::writef (const char* tag,Entity* obj)
{
  writef (tag,obj,tag);
}

void WXFFile::write3f (const char* tag,Entity* obj,const char* pname)
{
  Etc&        etc        = Command::instance ().etc ();
  const char* expression = etc.get_expression (obj->property (pname));

  if (*expression) _writef ("%s '%s'\n",tag?tag:pname,expression);  
  else
  {
    vec3f v = etc.get3f (obj->property (pname));

    _writef ("%s %.3f %.3f %.3f\n",tag?tag:pname,v.x,v.y,v.z);
  }
}

void WXFFile::write3f (const char* tag,Entity* obj)
{
  write3f (tag,obj,tag);
}

void WXFFile::comment (const char* text)
{
  _writef ("//%s\n",text);
}

/*
    SceneSaveVisitor
*/

template <class T> const char* SceneSaveVisitor::get_type (T*)
{ 
  return "node";
}

template <> const char* SceneSaveVisitor::get_type (Light*)
{ 
  return "light"; 
}

template <> const char* SceneSaveVisitor::get_type (Camera*)
{ 
  return "camera"; 
}

template <> const char* SceneSaveVisitor::get_type (ParamModel* obj)
{ 
  switch (obj->GetModelType ())
  {
    case MODEL_BOX:           return "box";
    case MODEL_SPHERE:        return "sphere";
    case MODEL_HALF_SPHERE:   return "halfsphere";
    case MODEL_CYLINDER:      return "cylinder";
    case MODEL_HALF_CYLINDER: return "halfcylinder";    
    case MODEL_CONE:          return "cone";
    case MODEL_HALF_CONE:     return "halfcone";    
    case MODEL_TETRAHEDRON:   return "tetrahedron";
    case MODEL_HEXAHEDRON:    return "hexahedron";
    case MODEL_OCTAHEDRON:    return "octahedron";
    case MODEL_DODECAHEDRON:  return "dodecahedron";
    case MODEL_ICOSAHEDRON:   return "icosahedron";
    case MODEL_TORUS:         return "torus";
    case MODEL_HALF_TORUS:    return "halftorus";
    default:                  return NULL;
  }
}

template <class T>
void SceneSaveVisitor::_save (T* obj)
{  
  const char* type = get_type (obj);
  
  if (!type || obj->IsEnabled (ENTITY_TEMP) || obj == obj->scene ()->root ())
    return;
    
  size_t node_depth = obj->GetNodeDepth ();
    
  os.begin (node_depth);
  os.begin (type,"'%s'",obj->name ());
  
  if (!obj->IsEnabled (ENTITY_VISIBLE))  os.write ("hidden");
    
  os.write3f ("wire_color",obj);
  os.write   ("");      
  
//  vec3f pos,rot,scale;
  
//  affine_decomp (obj->GetLocalTM (),pos,rot,scale);  
//  obj->identity ();
//  obj->transform (translate (pos) * rotatef (deg2rad (rot.z),0,0,1) * 
//        rotatef (deg2rad (rot.y),0,1,0) * rotatef (deg2rad (rot.x),1,0,0) * ::scale (scale));
  
  os.write3f ("local_position",obj);
  os.write3f ("local_rotation",obj);
  os.write3f ("local_scale",obj);
  os.write   ("");
  
  save (obj);
  
  os.write ("");  
}

void SceneSaveVisitor::save (Light* obj)
{
  switch (obj->GetLightType ())
  {
    case LIGHT_POINT:  os.write ("type","point"); break;
    case LIGHT_SPOT:   os.write ("type","spot"); break;
    case LIGHT_DIRECT: os.write ("type","direct"); break;    
    default:           return;
  }
  
  if (!obj->state ())
    os.write ("disable");
    
  os.write ("ambient",obj->GetLightColor (LIGHT_AMBIENT_COLOR));
  os.write ("diffuse",obj->GetLightColor (LIGHT_DIFFUSE_COLOR));
  os.write ("specular",obj->GetLightColor (LIGHT_SPECULAR_COLOR));
  
  if (obj->GetLightType () == LIGHT_SPOT)
  {
    os.write ("spot_angle",obj->GetSpotAngle ());
    os.write ("spot_exponent",obj->GetSpotExponent ());
  }
  
  os.write ("radius",obj->GetInnerRadius ());
}

void SceneSaveVisitor::save (Camera* obj)
{  
  switch (obj->GetProjType ())
  {
    case CAMERA_PROJ_ORTHO:
      os.write ("projection","ortho");
      break;
    case CAMERA_PROJ_ISOMETRY:
      os.write ("projection","isometry");
      break;
    case CAMERA_PROJ_DIMETRY:
      os.write  ("projection","dimetry");
      os.writef ("alpha",obj);
      os.writef ("beta",obj);      
      break;
    case CAMERA_PROJ_TRIMETRY:
      os.write  ("projection","trimetry");
      os.writef ("alpha",obj);
      os.writef ("beta",obj);
      break;
    case CAMERA_PROJ_CABINET:
      os.write ("projection","cabinet");
      break;
    case CAMERA_PROJ_FREE:
      os.write ("projection","free");
      break;    
    case CAMERA_PROJ_PERSPECTIVE:
      os.write  ("projection","perspective");
      os.writef ("distance",obj);
      break;      
    case CAMERA_PROJ_PERSPECTIVE_SPECIAL:
      os.write3f ("point1",obj);    
      os.write3f ("point2",obj);
      os.write3f ("point3",obj);
      break;
    default:    
      os.write ("projection","user");
      break;
  }
  
  os.writef ("left",obj);
  os.writef ("right",obj);
  os.writef ("top",obj);
  os.writef ("bottom",obj);
  os.writef ("znear",obj);
  os.writef ("zfar",obj);  
}

void SceneSaveVisitor::save (Material& mtl)
{
  os.begin ("material");
  
  if (mtl.IsEnabled (MATERIAL_WIREFRAME)) os.write ("wireframe");
  if (mtl.IsEnabled (MATERIAL_TWO_SIDED)) os.write ("two_sided");
  
  switch (mtl.GetBlendMode ())
  {
    case MATERIAL_BLEND_FILTER: os.write ("blend","filter"); break;
    case MATERIAL_BLEND_ADD:    os.write ("blend","add"); break;
    default:                    os.write ("blend","solid"); break;
  }  
  
  os.write ("ambient",mtl.GetColor (MATERIAL_AMBIENT_COLOR));
  os.write ("diffuse",mtl.GetColor (MATERIAL_DIFFUSE_COLOR));
  os.write ("specular",mtl.GetColor (MATERIAL_SPECULAR_COLOR));
  os.write ("emission",mtl.GetColor (MATERIAL_EMISSION_COLOR));
  os.write ("transparency",mtl.GetTransparency ());
  os.write ("shininess",mtl.GetShininess ());      
  
  os.end ();
}

void SceneSaveVisitor::save (ParamModel* obj)
{
  switch (obj->GetModelType ())
  {
    case MODEL_BOX:
      os.writef ("width",obj);
      os.writef ("height",obj);
      os.writef ("depth",obj);
      break;
    case MODEL_HALF_SPHERE:
    case MODEL_SPHERE:
      os.writef ("radius",obj);
      os.writei ("parallels",obj);
      os.writei ("meridians",obj);
      break;
    case MODEL_HALF_CYLINDER:
    case MODEL_CYLINDER:
      os.writef ("radius",obj);
      os.writef ("height",obj);
      os.writei ("slices",obj);
      break;
    case MODEL_HALF_CONE:
    case MODEL_CONE:
      os.writef ("height",obj);
      os.writef ("radius",obj);
      os.writef ("radius2",obj);
      os.writei ("slices",obj);
      break;
    case MODEL_TETRAHEDRON:
    case MODEL_HEXAHEDRON:
    case MODEL_OCTAHEDRON:
    case MODEL_DODECAHEDRON:
    case MODEL_ICOSAHEDRON:
      os.writef ("radius",obj);
      break;            
    case MODEL_HALF_TORUS:
    case MODEL_TORUS:
      os.writef ("radius",obj);
      os.writef ("radius2",obj);
      os.writei ("parallels",obj);
      os.writei ("meridians",obj);      
      break;
  }
  
  os.writef ("shear_x",obj);
  os.writef ("shear_z",obj);  
  
  save (obj->GetMaterial ());
}

void SceneSaveVisitor::save (HelperEntity*)
{
}

SceneSaveVisitor::SceneSaveVisitor (const char* file_name)
  : os (file_name), cur_depth (0) 
{
  os.begin ("scene");
}

bool Scene::save (const char* file_name)
{
  if (!file_name || !*file_name)
    return false;

  try
  {
    SceneSaveVisitor visitor (file_name);
        
    root ()->traverse (&visitor);
  }
  catch (...)
  {    
    return false;
  }
  
  return true;
}
