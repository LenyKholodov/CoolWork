#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>

#include "converter.h"
#include <core.h>

namespace My3D
{
  ref class PropertyForm;
  ref class Vec3f;

/*
===============================================================================

  Entity

===============================================================================
*/

  public ref class Entity : public System::Object
  {
    public:
      Entity (CoreEntityHandle handle);
      ~Entity ();

      #pragma region Properties

      [System::ComponentModel::Description ("��� �������"), System::ComponentModel::Category ("�����")]
      property System::String^ Name
      {
        System::String^ get ();
        void set (System::String^ value);
      };

      [System::ComponentModel::Description ("��� ��������"), System::ComponentModel::Category ("�����")]
      property System::String^ Parent
      {
        System::String^ get ();
      };

      [System::ComponentModel::Description ("��� �������"), System::ComponentModel::Category ("�����")]
      property System::String^ Type
      {
        System::String^ get ();
        virtual void set (System::String^ value) abstract;
      };

      [System::ComponentModel::Description ("���� �������"), System::ComponentModel::Category ("�����")]
      property System::Drawing::Color WireColor
      {
        System::Drawing::Color get ();
        void set (System::Drawing::Color value);
      };

      [System::ComponentModel::Description ("����� �� ������"), System::ComponentModel::Category ("�����")]
      property bool Visible
      {
        bool get ();
        void set (bool value);
      };

      [System::ComponentModel::Description ("������� �� ������"), System::ComponentModel::Category ("�����")]
      property bool Selected
      {
        bool get ();
      };

      [System::ComponentModel::Description ("�������"), System::ComponentModel::Category ("���������")]
      property Vec3f^ LocalPosition
      {
        Vec3f^ get ();
        void set (Vec3f^ value);
      };

      [System::ComponentModel::Description ("�������"), System::ComponentModel::Category ("���������")]
      property Vec3f^ LocalRotation
      {
        Vec3f^ get ();
        void set (Vec3f^ value);
      };

      [System::ComponentModel::Description ("�������"), System::ComponentModel::Category ("���������")]
      property Vec3f^ LocalScale
      {
        Vec3f^ get ();
        void set (Vec3f^ value);
      };

      [System::ComponentModel::Description ("����������� ��� OZ �������"), System::ComponentModel::Category ("���������")]
      property Vec3f^ LocalDir
      {
        Vec3f^ get ();
//        void set (Vec3f^ value);
      };

      [System::ComponentModel::Description ("����������� ��� OY �������"), System::ComponentModel::Category ("���������")]
      property Vec3f^ LocalUp
      {
        Vec3f^ get ();
//        void set (Vec3f^ value);
      };

      [System::ComponentModel::Description ("�������"), System::ComponentModel::Category ("����������")]
      property Vec3f^ GlobalPosition
      {
        Vec3f^ get ();
        void set (Vec3f^ value);
      };

      [System::ComponentModel::Description ("�������"), System::ComponentModel::Category ("����������")]
      property Vec3f^ GlobalRotation
      {
        Vec3f^ get ();
        void set (Vec3f^ value);
      };

      [System::ComponentModel::Description ("�������"), System::ComponentModel::Category ("����������")]
      property Vec3f^ GlobalScale
      {
        Vec3f^ get ();
        void set (Vec3f^ value);
      };

      [System::ComponentModel::Description ("����������� ��� OZ �������"), System::ComponentModel::Category ("����������")]
      property Vec3f^ GlobalDir
      {
        Vec3f^ get ();
//        void set (Vec3f^ value);
      };

      [System::ComponentModel::Description ("����������� ��� OY �������"), System::ComponentModel::Category ("����������")]
      property Vec3f^ GlobalUp
      {
        Vec3f^ get ();
//        void set (Vec3f^ value);
      };

      #pragma endregion

      CoreEntityHandle GetHandle ();

      virtual System::String^ ToString () override;
      
      /*virtual*/ void OnCreate ();
      /*virtual*/ void OnDelete ();
      virtual void OnModify ();

    protected:
      CoreEntityHandle handle;
      System::String^ type;

    private:
      System::String^ name;
      System::Drawing::Color color;
      Vec3f^ local_position;
      Vec3f^ local_scale;
      Vec3f^ local_rotation;
      Vec3f^ local_dir;
      Vec3f^ local_up;
      Vec3f^ global_position;
      Vec3f^ global_scale;
      Vec3f^ global_rotation;
      Vec3f^ global_dir;
      Vec3f^ global_up;
      bool selected;
      bool visible;
  };

/*
===============================================================================

  ModelEntity class

===============================================================================
*/

  public ref class ModelEntity : public Entity
  {
    public:
      ModelEntity (CoreEntityHandle handle);
      ~ModelEntity ();

      #pragma region Properties

      [System::ComponentModel::TypeConverter (GeomTypeConverter::typeid),
       System::ComponentModel::Description ("��� �������"), System::ComponentModel::Category ("�����")]
      property System::String^ Type
      {
        virtual void set (System::String^ value) override {};
      };

      [System::ComponentModel::Description ("���� �� ��� X"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single ShearXY
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("���� �� ��� Z"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single ShearZY
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("����������� ������������ ���������"), System::ComponentModel::Category ("��������")]
      property System::Drawing::Color Ambient
      {
        System::Drawing::Color get ();
        void set (System::Drawing::Color value);
      };

      [System::ComponentModel::Description ("���������� ������������ ���������"), System::ComponentModel::Category ("��������")]
      property System::Drawing::Color Diffuse
      {
        System::Drawing::Color get ();
        void set (System::Drawing::Color value);
      };

      [System::ComponentModel::Description ("��������� ������������ ���������"), System::ComponentModel::Category ("��������")]
      property System::Drawing::Color Specular
      {
        System::Drawing::Color get ();
        void set (System::Drawing::Color value);
      };

      [System::ComponentModel::Description ("���� ���������"), System::ComponentModel::Category ("��������")]
      property System::Drawing::Color Emission
      {
        System::Drawing::Color get ();
        void set (System::Drawing::Color value);
      };

      [System::ComponentModel::Description ("������������� [0..128]"), System::ComponentModel::Category ("��������")]
      property System::Single Shininess
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������������ [0..1]"), System::ComponentModel::Category ("��������")]
      property System::Single Transparency
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::TypeConverter (BlendModeConverter::typeid),
       System::ComponentModel::Description ("����� ���������� ������"), System::ComponentModel::Category ("��������")]
      property System::String^ BlendMode
      {
        System::String^ get ();
        void set (System::String^ value);
      };

      [System::ComponentModel::Description ("���������� �� ��������� ������"), System::ComponentModel::Category ("��������")]
      property bool Wireframe
      {
        bool get ();
        void set (bool value);
      };

      [System::ComponentModel::Description ("������������ ��������"), System::ComponentModel::Category ("��������")]
      property bool TwoSided
      {
        bool get ();
        void set (bool value);
      };

      #pragma endregion

      virtual void OnModify () override;

    private :
      System::Drawing::Color ambient_color;
      System::Drawing::Color diffuse_color;
      System::Drawing::Color specular_color;
      System::Drawing::Color emission_color;
      System::Single shininess;
      System::Single transparency;
      System::String^ blend_mode;
      bool wireframe;
      bool two_sided;
      System::Single shear_xy, shear_zy;
   };

/*
===============================================================================

  Light

===============================================================================
*/

  public ref class Light : public Entity
  {
    public:    

      Light (CoreEntityHandle handle);
      ~Light ();

      #pragma region Properties

      [System::ComponentModel::Description ("��� �������"), System::ComponentModel::Category ("�����")]
      property System::String^ Type
      {
        virtual void set (System::String^ value) override {};
      };

      [System::ComponentModel::Description ("��� ���������"), System::ComponentModel::Category ("�����")]
      property System::String^ LightType
      {
        System::String^ get ();
        void set (System::String^ value);
      };

      [System::ComponentModel::Description ("������� �� ��������"), System::ComponentModel::Category ("�����")]
      property bool Enabled
      {
        bool get ();
        void set (bool value);
      };

      [System::ComponentModel::Description ("����������� ������������ ���������"), System::ComponentModel::Category ("��������� ���������")]
      property System::Drawing::Color Ambient
      {
        System::Drawing::Color get ();
        void set (System::Drawing::Color value);
      };

      [System::ComponentModel::Description ("���������� ������������ ���������"), System::ComponentModel::Category ("��������� ���������")]
      property System::Drawing::Color Diffuse
      {
        System::Drawing::Color get ();
        void set (System::Drawing::Color value);
      };

      [System::ComponentModel::Description ("��������� ������������ ���������"), System::ComponentModel::Category ("��������� ���������")]
      property System::Drawing::Color Specular
      {
        System::Drawing::Color get ();
        void set (System::Drawing::Color value);
      };

      [System::ComponentModel::Description ("������ ���������"), System::ComponentModel::Category ("��������� ���������")]
      property System::Single Radius
      {
        System::Single get ();
        void set (System::Single value);
      };

      #pragma endregion

      virtual void OnModify () override;

    private:
      bool enabled;
      System::String^ light_type;
      System::Single radius;
      System::Drawing::Color ambient_color;
      System::Drawing::Color diffuse_color;
      System::Drawing::Color specular_color;
  };

/*
===============================================================================

  SpotLight

===============================================================================
*/

  public ref class SpotLight : public Light
  {
    public:

      SpotLight (CoreEntityHandle handle);
      ~SpotLight ();

      #pragma region Properties

      [System::ComponentModel::Description ("����������� ���� ���������"), System::ComponentModel::Category ("��������� ���������")]
      property System::Single Angle
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("��������� ���������"), System::ComponentModel::Category ("��������� ���������")]
      property System::Single Exponent
      {
        System::Single get ();
        void set (System::Single value);
      };

      #pragma endregion

      virtual void OnModify () override;

    private:
      System::Single angle, exponent;
  };

/*
===============================================================================

  Box

===============================================================================
*/

  public ref class Box: public ModelEntity
  {
    public:    

      Box (CoreEntityHandle handle);
      ~Box ();

      #pragma region Properties

      [System::ComponentModel::Description ("������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single Width
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single Height
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("�������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single Depth
      {
        System::Single get ();
        void set (System::Single value);
      };

      #pragma endregion

      virtual void OnModify () override;

    private :
      System::Single width, height, depth;
  };

/*
===============================================================================

  Sphere

===============================================================================
*/

  public ref class Sphere: public ModelEntity
  {
    public:    

      Sphere (CoreEntityHandle handle);
      ~Sphere ();

      #pragma region Properties

      [System::ComponentModel::TypeConverter (GeomTypeConverter::typeid),
       System::ComponentModel::Description ("��� �������"), System::ComponentModel::Category ("�����")]
      property System::String^ Type
      {
        virtual void set (System::String^ value) override {};
      };

      [System::ComponentModel::Description ("������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single Radius
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("���������� ����������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Int32 Parallels
      {
        System::Int32 get ();
        void set (System::Int32 value);
      };

      [System::ComponentModel::Description ("���������� ����������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Int32 Meridians
      {
        System::Int32 get ();
        void set (System::Int32 value);
      };

      #pragma endregion

      virtual void OnModify () override;

    private :
      System::Single radius;
      System::Int32 parallels, meridians;
  };

/*
===============================================================================

  Cone

===============================================================================
*/

  public ref class Cone: public ModelEntity
  {
    public:    

      Cone (CoreEntityHandle handle);
      ~Cone ();

      #pragma region Properties

      [System::ComponentModel::Description ("������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single Height
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������ �������� ���������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single Radius
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������ ������� ���������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single SecondRadius
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("���������� ���������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Int32 Slices
      {
        System::Int32 get ();
        void set (System::Int32 value);
      };

      #pragma endregion

      virtual void OnModify () override;

    private :
      System::Single height, radius, second_radius;
      System::Int32 slices;
  };

/*
===============================================================================

  Cylinder

===============================================================================
*/

  public ref class Cylinder: public ModelEntity
  {
    public:    

      Cylinder (CoreEntityHandle handle);
      ~Cylinder ();

      #pragma region Properties

      [System::ComponentModel::Description ("������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single Radius
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single Height
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("���������� ���������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Int32 Slices
      {
        System::Int32 get ();
        void set (System::Int32 value);
      };

      #pragma endregion

      virtual void OnModify () override;

    private :
      System::Single radius, height;
      System::Int32 slices;
  };

	/*
===============================================================================

  Torus

===============================================================================
*/

  public ref class Torus: public ModelEntity
  {
    public:    

      Torus (CoreEntityHandle handle);
      ~Torus ();

      #pragma region Properties

      [System::ComponentModel::Description ("���������� ������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single InnerRadius
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������� ������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single OuterRadius
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("���������� ����������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Int32 Parallels
      {
        System::Int32 get ();
        void set (System::Int32 value);
      };

      [System::ComponentModel::Description ("���������� ����������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Int32 Meridians
      {
        System::Int32 get ();
        void set (System::Int32 value);
      };

      #pragma endregion

      virtual void OnModify () override;

    private :
      System::Single inner_radius, outer_radius;
      System::Int32 parallels, meridians;
  };

/*
===============================================================================

  PlatonicBody

===============================================================================
*/

  public ref class PlatonicBody: public ModelEntity
  {
    public:    

      PlatonicBody (CoreEntityHandle handle);
      ~PlatonicBody ();

      #pragma region Properties

      [System::ComponentModel::Description ("������"), System::ComponentModel::Category ("�������� ��������� ������")]
      property System::Single Radius
      {
        System::Single get ();
        void set (System::Single value);
      };

      #pragma endregion

      virtual void OnModify () override;

    private :
      System::Single radius;
  };

/*
===============================================================================

  Scene

===============================================================================
*/
  
  public ref class Scene
  {
    delegate Entity^ Command (CoreEntityHandle handle);

    typedef System::Collections::Generic::Dictionary <CoreEntityHandle, Entity^> EntityMap;
    typedef System::Collections::Generic::Dictionary <System::String^, Command^> TypeMap;
    typedef System::Collections::Generic::Dictionary <System::String^, Command^> CameraMap;
    typedef System::Collections::Generic::Dictionary <System::String^, Command^> LightMap;

    public:
      Scene ();
      ~Scene ();
    
      Entity^ OnCreateEntity (CoreEntityHandle);
      void    OnDeleteEntity (CoreEntityHandle);
      void    OnModifyEntity (CoreEntityHandle);
      void    OnSelect       ();

      Entity^ CreateCamera       (CoreEntityHandle);
      Entity^ CreateLight        (CoreEntityHandle);
      Entity^ CreateBox          (CoreEntityHandle);
      Entity^ CreateSphere       (CoreEntityHandle);
      Entity^ CreateCone         (CoreEntityHandle);
      Entity^ CreateCylinder     (CoreEntityHandle);
      Entity^ CreateTorus        (CoreEntityHandle);
      Entity^ CreatePlatonicBody (CoreEntityHandle);

      Entity^ CreateBaseCam         (CoreEntityHandle);
      Entity^ CreateIsometryCam     (CoreEntityHandle);
      Entity^ CreateDimetryCam      (CoreEntityHandle);
      Entity^ CreateTrimetryCam     (CoreEntityHandle);
      Entity^ CreatePerspective1Cam (CoreEntityHandle);
      Entity^ CreatePerspective2Cam (CoreEntityHandle);
      Entity^ CreatePerspective3Cam (CoreEntityHandle);

      Entity^ CreatePointLight   (CoreEntityHandle);
      Entity^ CreateSpotLight    (CoreEntityHandle);
      Entity^ CreateDirectLight  (CoreEntityHandle);

      #pragma region Properties

      property EntityMap^ Entities
      {
        EntityMap^ get (void)
        {
          return entity_map;
        }
      }

      property bool Modified
      {
        bool get (void)       { return modified;  }
        void set (bool value) { modified = value; }
      }

      #pragma endregion

    private:
      TypeMap^   CreateTypeMap ();
      CameraMap^ CreateCameraMap ();
      LightMap^  CreateLightMap ();
      Entity^    Create (CoreEntityHandle, System::String^ type);
    
    private:
      EntityMap^ entity_map;
      TypeMap^   type_map;
      CameraMap^ camera_map;
      CameraMap^ light_map;
      bool modified;
  };
}
