#include "gui_vector.h"
#include "utils.h"

#using <System.Windows.Forms.dll>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Globalization;

namespace My3D {

/*
===============================================================================

  Методы для работы со свойствами вектора через PropertyGrid

===============================================================================
*/

bool Vec3fConverter::CanConvertTo (ITypeDescriptorContext^ context, Type^ destinationType)
{
  if (destinationType == Vec3f::typeid)
    return true;
  return ExpandableObjectConverter::CanConvertTo (context, destinationType);
}

Object^ Vec3fConverter::ConvertTo (ITypeDescriptorContext^ context, CultureInfo^ culture, Object^ value, Type^ destinationType )
{
  if (destinationType == String::typeid && isinst <Vec3f^> (value))
  {
    Vec3f^ vec3f = (Vec3f^) value;

    return "( " + String::Format ("{0:F}", vec3f->X) + "; " + 
                  String::Format ("{0:F}", vec3f->Y) + "; " + 
                  String::Format ("{0:F}", vec3f->Z) + " )";
  }
  return ExpandableObjectConverter::ConvertTo (context, culture, value, destinationType);
}

bool Vec3fConverter::CanConvertFrom (ITypeDescriptorContext^ context, Type^ sourceType)
{
  if (sourceType == String::typeid)
    return true;

  return ExpandableObjectConverter::CanConvertFrom (context, sourceType);
}

Object^ Vec3fConverter::ConvertFrom (ITypeDescriptorContext^ context, CultureInfo^ culture, Object^ value)
{
  if (isinst <String^> (value))
  {
    try
    {
      String^ s = (String^) value;

      int colon = s->IndexOf ('(');
      int comma = s->IndexOf (';');

      if (colon != -1 && comma != -1)
      {
        String^ X = s->Substring (colon+1, comma-1);

        colon = comma + 1;
        comma = s->IndexOf (';', colon);

        String^ Y = s->Substring (colon, (comma - colon));

        colon = comma + 1;
        comma = s->IndexOf (')', comma + 1);

        String^ Z = s->Substring (colon, (comma - colon));

        return gcnew Vec3f (Single::Parse (X),
                            Single::Parse (Y),
                            Single::Parse (Z));
      }
    }
    catch (...)
    {
      throw gcnew ArgumentException ("Cannot convert '" + (String^)value + "' to type Vec3f");
    }
  }
  return ExpandableObjectConverter::ConvertFrom (context, culture, value);
}

}

