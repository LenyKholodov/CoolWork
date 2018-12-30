#ifndef __COOLWORK_MATERIAL__
#define __COOLWORK_MATERIAL__

#include <stddef.h>
#include <mathlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MtlFlags
{
  MATERIAL_WIREFRAME  = 1, //����������� ������
  MATERIAL_TWO_SIDED  = 2, //������������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MaterialColor
{
  MATERIAL_EMISSION_COLOR, //���� ���������
  MATERIAL_AMBIENT_COLOR,  //����������� ������������
  MATERIAL_DIFFUSE_COLOR,  //��������� ������������  
  MATERIAL_SPECULAR_COLOR, //��������� ������������  
  
  MATERIAL_COLORS_NUM
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BlendMode
{
  MATERIAL_BLEND_SOLID,  //�������� �����������
  MATERIAL_BLEND_ADD,    //�������� ������
  MATERIAL_BLEND_FILTER  //������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Material
{  
  public:  
    Material  ();
    ~Material ();      
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void        rename (const char*);
    const char* name   ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    void enable    (size_t flags);
    void disable   (size_t flags);
    bool IsEnabled (size_t flags);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetColor     (MaterialColor,const vec3f&);
    void         SetShininess (float shininess);
    const vec3f& GetColor     (MaterialColor);    
    float        GetShininess ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void      SetBlend        (BlendMode,float alpha);
    void      SetBlendMode    (BlendMode);    
    void      SetTransparency (float alpha);
    float     GetTransparency ();
    BlendMode GetBlendMode    ();

  private:
    void RegisterVars ();
    void UnregisterVars ();

  private:
    struct Impl;
    Impl* impl;
};

#endif
