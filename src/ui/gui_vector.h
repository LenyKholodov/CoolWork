#pragma once

#using <mscorlib.dll>
#using <System.dll>

#include "utils.h"

namespace My3D
{
  ref class Vec3fConverter;

  [System::ComponentModel::TypeConverter (Vec3fConverter::typeid),
   System::ComponentModel::DescriptionAttribute ("Компоненты вектора")]
  public ref class Vec3f : public System::Object
  {
    public:
      Vec3f (System::Single _x, System::Single _y, System::Single _z) : x(_x), y(_y), z(_z)
      {}

      Vec3f () : x(0.0f), y(0.0f), z(0.0f)
      {}

      Vec3f (Vec3f^ v) : x(v->X), y(v->Y), z(v->Z)
      {}

      void assign (Vec3f^ v)
      {
        x = v->X;
        y = v->Y;
        z = v->Z;
      }

      void assign (float v [3])
      {
        x = v [0];
        y = v [1];
        z = v [2];
      }

      #pragma region Properties

      property System::Single X
      {
        System::Single get () { return x; }
        void  set (System::Single value) { x = value; }
      };

      property System::Single Y
      {
        System::Single get () { return y; }
        void  set (System::Single value) { y = value; }
      };

      property System::Single Z
      {
        System::Single get () { return z; }
        void  set (System::Single value) { z = value; }
      };

      #pragma endregion

    private:
      System::Single x,y,z;
  };

  public ref class Vec3fConverter : public System::ComponentModel::ExpandableObjectConverter 
  {
    public :

      virtual bool CanConvertTo (System::ComponentModel::ITypeDescriptorContext^ context, System::Type^ destinationType) override;

      virtual System::Object^ ConvertTo (System::ComponentModel::ITypeDescriptorContext^ context,
                                         System::Globalization::CultureInfo^ culture, 
                                         System::Object^ value, 
                                         System::Type^ destinationType ) override;

      virtual bool CanConvertFrom (System::ComponentModel::ITypeDescriptorContext^ context, System::Type^ sourceType) override;

      virtual System::Object^ ConvertFrom (System::ComponentModel::ITypeDescriptorContext^ context,
                                           System::Globalization::CultureInfo^ culture,
                                           System::Object^ value) override;
  };
}
