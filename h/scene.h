#ifndef __COOLWORK_SCENE__
#define __COOLWORK_SCENE__

#include <stddef.h>
#include <mathlib.h>
#include <cmd.h>
#include <mesh.h>
#include <landscape.h>

//forwards
class Entity;
class EntityList;
class Light;
class Camera;
class ParamModel;
class HelperEntity;
class LandscapeModel;
class Scene;
class SceneVisitor;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////        
enum Traverse
{
  TRAVERSE_TOP_TO_BOTTOM, //����� �������� �� ����� � ��������
  TRAVERSE_BOTTOM_TO_TOP, //����� �������� �� �������� � �����
  TRAVERSE_NO_RECURSION   //��������� ������ ���������� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��� ������ ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////        
class EntityVisitor
{
  public:
    virtual void visit (Entity*) {}
    virtual void visit (Light*) {}
    virtual void visit (Camera*)  {}
    virtual void visit (ParamModel*) {}
    virtual void visit (HelperEntity*) {}
    virtual void visit (LandscapeModel*) {}    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////        
class EntityList
{
  public:
    EntityList  ();
    EntityList  (const EntityList&);    
    ~EntityList ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////            
    size_t  size () const;
    Entity* item (size_t index) const;
    bool    test (Entity*) const;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������/�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void insert (Entity*);
    void remove (Entity*);
    void clear  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void traverse (EntityVisitor*,Traverse = TRAVERSE_NO_RECURSION);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////        
    EntityList& operator = (const EntityList&);
      
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum EntityFlags
{
  ENTITY_VISIBLE = 1, //������ �������    
  ENTITY_SELECT  = 2, //������ �������
  ENTITY_TEMP    = 4, //��������� ������ - �� ����������� ��� ������������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum EntitySelect
{
  ENTITY_GROUP_SELECT,     //��������� ���������
  ENTITY_EXCLUSIVE_SELECT, //����������� ��������� (������ ���� ������)
  ENTITY_CLEAR_SELECT,     //�������� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Entity
{
  friend class Scene;
  public:   
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////      
    void addref  ();
    void release ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void destroy ();    
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///���
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        rename (const char* name);
    const char* name   ();
    Entity*     find   (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void enable    (size_t flags);
    void disable   (size_t flags);
    bool IsEnabled (size_t flags);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ������� ����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////        
    Scene* scene ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void bind   (Entity* parent,bool save_world_pos=false); //�������� �������� � ���������
    void unbind (bool save_world_pos=false);                //������������ � ��������� ����

    Entity* parent (); //������������ ������
    Entity* first  (); //������ ������
    Entity* last   (); //��������� ������
    Entity* next   (); //���������� ������
    Entity* prev   (); //���������� ������
    
    size_t GetNodeDepth (); //���������� ������� ���� � ������ ���������
    
    void traverse  (EntityVisitor*,Traverse = TRAVERSE_TOP_TO_BOTTOM); //������������� ����� ������ ��������
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� � ������������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void translate (const vec3f&); 
    void rotate    (const quatf&);
    void rotate    (float angle_in_degrees,const vec3f& axis);
    void rotate    (float tangent,float yaw,float roll);
    void scale     (const vec3f&);
    void lookat    (const vec3f& from,const vec3f& to); //auto vector up calculations
    void lookat    (const vec3f& from,const vec3f& to,const vec3f& up);
    void identity  ();
    void transform (const mat4f&);
    
    const mat4f& GetLocalTM  ();
    const mat4f& GetWorldTM  ();
    const mat4f  GetObjectTM (Entity* object); //����������� ������� ��������� �������    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� (pivot)
///////////////////////////////////////////////////////////////////////////////////////////////////        
    Entity* pivot ();
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////            
    void select     (EntitySelect = ENTITY_GROUP_SELECT); //toogle
    bool IsSelected () { return IsEnabled (ENTITY_SELECT); }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Wire color
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetWireColor (const vec3f& color);
    const vec3f& GetWireColor ();    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    const char* property (const char* name); //return full property name (danger to use!)    

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ��������� (�������������� ������) � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////        
    void        SetExpression (const char* property_name,const char* expression);
    const char* GetExpression (const char* property_name);
    void        RemoveDependencies (); //�������� ������������ �� ���������
    
  protected:
    Entity  (Scene* = NULL);
    virtual ~Entity ();    
    
    virtual void visit (EntityVisitor*);    
    
    void OnModify ();            
    
    virtual void RegisterVars ();
    virtual void UnregisterVars ();    

  private:            
    void _RegisterVars ();
    void _UnregisterVars ();
    
    void OnDelete ();
    void OnCreate ();
    void OnSelect ();
        
  private:
    struct Impl;
    Impl* impl;     
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ProjType
{
  CAMERA_PROJ_ORTHO,        //�������������
  CAMERA_PROJ_ISOMETRY,     //���������
  CAMERA_PROJ_DIMETRY,      //��������
  CAMERA_PROJ_TRIMETRY,     //���������
  CAMERA_PROJ_CABINET,      //����������
  CAMERA_PROJ_FREE,         //���������
  CAMERA_PROJ_PERSPECTIVE,  //������������ ������������� (�������)
  CAMERA_PROJ_PERSPECTIVE_SPECIAL //����������� ������������� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ProjParams
{
  ProjType type;                                  //��� ��������
  float    left, right, top, bottom, znear, zfar; //��������� ���������� ���������
  float    alpha, beta, gamma;                    //���� ��� ��������, ��������� � �.�.
  vec3f    distance;                              //���������� ����� �� ���� ��� ������������� �������� (��������)
  vec3f    point [3];                             //���������� ����� ��� ������������� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Camera: public Entity
{
  friend class Scene;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetPerspective  (float fov,float near_clip,float far_clip);
    void SetPerspective  (float fov,float near_clip,float far_clip,const vec3f& a,const vec3f& b,const vec3f& c); 
    void SetOrtho        (float left,float right,float top,float bottom,float near_clip,float far_clip);
    void SetIsometry     (float left,float right,float top,float bottom,float near_clip,float far_clip);
    void SetDimetry      (float left,float right,float top,float bottom,float near_clip,float far_clip,float angle1,float angle2);
    void SetTrimetry     (float left,float right,float top,float bottom,float near_clip,float far_clip,float angle1,float angle2);
    void SetCabinet      (float left,float right,float top,float bottom,float near_clip,float far_clip);
    void SetFree         (float left,float right,float top,float bottom,float near_clip,float far_clip);        
    
    const mat4f& GetProjTM ();

    ProjType          GetProjType   (); //��� ��������        
    const ProjParams& GetProjParams (); //�������������� ��������� ��������

    vec3f ProjectWorldPoint (const vec3f&); //��������� ��������� � ��������� ���� ����� �� ������� �����������

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void forward (float step=1.0f);
    void back    (float step=1.0f);
    void left    (float step=1.0f);
    void right   (float step=1.0f);
    void pitch   (float angle=5.0f);
    void yaw     (float angle=5.0f);
    void roll    (float angle=5.0f);
    
  protected:
    Camera  ();
    ~Camera ();
    
    void visit (EntityVisitor*);    
    
    void RegisterVars ();
    void UnregisterVars ();
    
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////        
enum LightType
{
  LIGHT_POINT, //�������� ��������
  LIGHT_SPOT,  //��������� ������������ ��������
  LIGHT_DIRECT //���������� �������� ������������ ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////        
enum LightColor
{
  LIGHT_AMBIENT_COLOR,  //����������� ������������
  LIGHT_DIFFUSE_COLOR,  //��������� ������������
  LIGHT_SPECULAR_COLOR, //��������� ������������
  
  LIGHT_COLOR_NUM
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////        
class Light: public Entity
{
  friend class Scene;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////            
    void on    ();
    void off   ();
    bool state ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void      SetLightType (LightType type);
    LightType GetLightType ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ��������� (�������� ��������� ��������� ������� ���������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetPointLight  (const vec3f& pos);
    void SetSpotLight   (const vec3f& pos,const vec3f& dir,float angle=45.0f);
    void SetDirectLight (const vec3f& dir);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetLightColor (LightColor type,const vec3f& color);
    const vec3f& GetLightColor (LightColor type);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ��������� Spot light
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetSpotAngle    (float angle);
    void  SetSpotExponent (float exponent);    
    float GetSpotAngle    ();
    float GetSpotExponent ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////            
    void  SetInnerRadius (float radius);
    float GetInnerRadius ();
        
  protected:
    Light  ();
    ~Light ();  
    
    void visit (EntityVisitor*);    
   
    void RegisterVars   ();
    void UnregisterVars ();
      
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////        
enum ParamModelType
{
  MODEL_UNKNOWN,
  MODEL_BOX,
  MODEL_HALF_SPHERE,  
  MODEL_SPHERE,
  MODEL_CONE,
  MODEL_HALF_CONE,
  MODEL_CYLINDER,
  MODEL_HALF_CYLINDER,
  MODEL_TETRAHEDRON,
  MODEL_HEXAHEDRON,
  MODEL_OCTAHEDRON,
  MODEL_DODECAHEDRON,
  MODEL_ICOSAHEDRON,
  MODEL_HALF_TORUS,
  MODEL_TORUS,
    
  MODEL_NUM      
};

class Surface;
class Material;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////        
class ParamModel: public Entity
{
  friend class Scene;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ������ ���� ������
///////////////////////////////////////////////////////////////////////////////////////////////////            
    void           SetModelType (ParamModelType type);
    ParamModelType GetModelType ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ������ �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void  SetParam (const char* name,float value);
    float GetParam (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////              
    Surface*  GetSurface ();
    Material& GetMaterial ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////                  
    AABB GetLocalAABB ();
    AABB GetWorldAABB ();
    
  private:
    ParamModel  ();
    ~ParamModel ();
    
    void visit (EntityVisitor*);      
    
    void RegisterVars   ();
    void UnregisterVars ();    
    
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum HelperId  
{
  HELPER_POINT,  //�����
  HELPER_ARROW,  //�������
  HELPER_RECT,   //�������������
  HELPER_AXES,   //��� ���������
  HELPER_ARC,    //����
  HELPER_SECTOR, //������
  HELPER_CIRCLE, //����������
  HELPER_LINE,   //�����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
class HelperEntity: public Entity
{
  friend class Scene;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� �������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    void SetPoint  (const vec3f& pos = 0.0f);
    void SetAxes   (const vec3f& pos = 0.0f);    
    void SetArrow  (const vec3f& from = 0.0f,const vec3f& to = vec3f (0,0,1));
    void SetLine   (const vec3f& from = 0.0f,const vec3f& to = vec3f (0,0,1));
    void SetArc    (const vec3f& pos = 0.0f,const vec3f dir = vec3f (0.0f,0.0f,1.0f),const vec3f up = (0.0f,1.0f,0.0f));
    void SetSector (const vec3f& pos = 0.0f,const vec3f dir = vec3f (0.0f,0.0f,1.0f),const vec3f up = (0.0f,1.0f,0.0f),const float angle = 180);
    void SetCircle (const vec3f& pos = 0.0f,const vec3f dir = vec3f (0.0f,0.0f,1.0f),const vec3f up = (0.0f,1.0f,0.0f));
    void SetRect   (const vec3f& center = 0.0f,const vec3f& dir = vec3f (0,0,1),const vec3f& size = 1.0f);
    
    HelperId GetHelperId ();
    float    GetHelperAngle ();
    void     SetHelperAngle (const float& angle);
    
  private:
    HelperEntity ();
    ~HelperEntity ();    
    
    void visit (EntityVisitor*);
  
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class LandscapeModel: public Entity, public Landscape
{
  friend class Scene;
  private:
    LandscapeModel ();
    ~LandscapeModel ();
    
    void visit (EntityVisitor*);
    
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////        
class SceneListener
{
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////          
    virtual void OnCreate (Entity*)       {}
    virtual void OnCreate (Light*)        {}
    virtual void OnCreate (Camera*)       {}
    virtual void OnCreate (ParamModel*)   {}
    virtual void OnCreate (HelperEntity*) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void OnDelete (Entity*)       {}
    virtual void OnDelete (Light*)        {}
    virtual void OnDelete (Camera*)       {}
    virtual void OnDelete (ParamModel*)   {}
    virtual void OnDelete (HelperEntity*) {}
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void OnModify (Entity*)       {}
    virtual void OnModify (Light*)        {}
    virtual void OnModify (Camera*)       {}
    virtual void OnModify (ParamModel*)   {}
    virtual void OnModify (HelperEntity*) {}
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    virtual void OnSelect (Entity*) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////        
class Scene
{
  friend class Entity;
  public:        
            Scene  (bool register_vars=false); //register_vars - ������������� ����������� ����������� ����������
            Scene  (const char* file_name,bool register_vars=false);
    virtual ~Scene ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Entity* root (); 
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void reset ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////                
    Entity*         CreateEntity ();
    Light*          CreateLight  ();
    Camera*         CreateCamera ();
    ParamModel*     CreateModel  ();
    HelperEntity*   CreateHelper ();
    LandscapeModel* CreateLandscape ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const EntityList& GetSelection   ();
    void              ClearSelection ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void RegisterListener   (SceneListener*);
    void UnregisterListener (SceneListener*);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////        
     bool save (const char* file_name);
     bool load (const char* file_name);
    
  private:
    void select (Entity*,bool);
    
    void OnCreate (Entity*);
    void OnModify (Entity*);
    void OnDelete (Entity*);
    void OnSelect (Entity*);
    
    template <class T> T* CreateObject ();
      
  private:
    struct Impl;
    Impl* impl;
};

#endif
