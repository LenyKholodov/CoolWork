#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>

#include "gui_scene.h"
#include "converter.h"
#include <core.h>

namespace My3D
{

/*
===============================================================================

  BaseCamera

===============================================================================
*/

  public ref class BaseCamera : public Entity
  {
    public:
      BaseCamera (CoreEntityHandle);
      ~BaseCamera ();
    
      #pragma region Properties

      [System::ComponentModel::Description ("��� �������"), System::ComponentModel::Category ("�����")]
      property System::String^ Type
      {
        virtual void set (System::String^ value) override {};
      };
      
      [System::ComponentModel::TypeConverter (ProjTypeConverter::typeid),
       System::ComponentModel::Description ("��� ��������"), System::ComponentModel::Category ("��������� ��������")]
      property System::String^ ProjectionType
      {
        System::String^ get ();
        void set (System::String^);
      };

      [EditorAttribute(FloatEditor::typeid, System::Drawing::Design::UITypeEditor::typeid),
       System::ComponentModel::Description ("����� ������������ ��������� ���������"), System::ComponentModel::Category ("��������� ��������")]
      property System::Single Left
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������ ������������ ��������� ���������"), System::ComponentModel::Category ("��������� ��������")]
      property System::Single Right
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������� �������������� ��������� ���������"), System::ComponentModel::Category ("��������� ��������")]
      property System::Single Top
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������ �������������� ��������� ���������"), System::ComponentModel::Category ("��������� ��������")]
      property System::Single Bottom
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������� ��������� ��������� �� �������"), System::ComponentModel::Category ("��������� ��������")]
      property System::Single NearClip
      {
        System::Single get ();
        void set (System::Single value);
      };

      [System::ComponentModel::Description ("������� ��������� ��������� �� �������"), System::ComponentModel::Category ("��������� ��������")]
      property System::Single FarClip
      {
        System::Single get ();
        void set (System::Single value);
      };

      #pragma endregion

      virtual void OnModify () override;

    private:
      System::String^ proj_type;
      Single left, right, top, bottom, near_clip, far_clip;
  };

/*
===============================================================================

  IsometryCamera

===============================================================================
*/

  public ref class IsometryCamera : public BaseCamera
  {
    public :
      IsometryCamera (CoreEntityHandle);
      ~IsometryCamera ();

      virtual void OnModify () override;

      #pragma region Properties

      [System::ComponentModel::Description ("�����"), System::ComponentModel::Category ("��������� ��������")]
      property System::Single Alpha
      {
        System::Single get ();
        void set (System::Single value);
      };

      #pragma endregion

    protected:
      System::Single alpha;
  };

/*
===============================================================================

  DimetryCamera

===============================================================================
*/

  public ref class DimetryCamera : public IsometryCamera
  {
    public :
      DimetryCamera (CoreEntityHandle);
      ~DimetryCamera ();

      virtual void OnModify () override;

      #pragma region Properties

      [System::ComponentModel::Description ("����"), System::ComponentModel::Category ("��������� ��������")]
      property System::Single Beta
      {
        System::Single get ();
        void set (System::Single value);
      };

      #pragma endregion

    protected:
      System::Single beta;
  };

/*
===============================================================================

  TrimetryCamera

===============================================================================
*/

  public ref class TrimetryCamera : public DimetryCamera
  {
    public :
      TrimetryCamera (CoreEntityHandle);
      ~TrimetryCamera ();

      virtual void OnModify () override;

      #pragma region Properties

      [System::ComponentModel::Description ("�����"), System::ComponentModel::Category ("��������� ��������")]
      property System::Single Gamma
      {
        System::Single get ();
        void set (System::Single value);
      };

      #pragma endregion

    protected:
      System::Single gamma;
  };

/*
===============================================================================

  Perspective1Camera

===============================================================================
*/

  public ref class Perspective1Camera : public BaseCamera
  {
    public :
      Perspective1Camera (CoreEntityHandle);
      ~Perspective1Camera ();

      virtual void OnModify () override;

      #pragma region Properties

      [System::ComponentModel::Description ("������ �����"), System::ComponentModel::Category ("��������� ��������")]
      property Vec3f^ First
      {
        Vec3f^ get ();
        void set (Vec3f^ value);
      };

      #pragma endregion

    protected:
      Vec3f^ first;
  };

/*
===============================================================================

  Perspective2Camera

===============================================================================
*/

  public ref class Perspective2Camera : public Perspective1Camera
  {
    public :
      Perspective2Camera (CoreEntityHandle);
      ~Perspective2Camera ();

      virtual void OnModify () override;

      #pragma region Properties

      [System::ComponentModel::Description ("������ �����"), System::ComponentModel::Category ("��������� ��������")]
      property Vec3f^ Second
      {
        Vec3f^ get ();
        void set (Vec3f^ value);
      };

      #pragma endregion

    protected:
      Vec3f^ second;
  };

/*
===============================================================================

  Perspective3Camera

===============================================================================
*/

  public ref class Perspective3Camera : public Perspective2Camera
  {
    public :
      Perspective3Camera (CoreEntityHandle);
      ~Perspective3Camera ();

      virtual void OnModify () override;

      #pragma region Properties

      [System::ComponentModel::Description ("������ �����"), System::ComponentModel::Category ("��������� ��������")]
      property Vec3f^ Third
      {
        Vec3f^ get ();
        void set (Vec3f^ value);
      };

      #pragma endregion

    protected:
      Vec3f^ third;
  };
}

