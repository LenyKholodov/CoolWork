#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Drawing;
using namespace System::Drawing::Design;
using namespace System::Windows::Forms;
using namespace System::Windows::Forms::Design;

namespace My3D {

public ref class BlendModeConverter : StringConverter 
{
  public:
    virtual bool GetStandardValuesSupported (ITypeDescriptorContext^) override
    {
      return true;
    }

    virtual TypeConverter::StandardValuesCollection^ GetStandardValues (ITypeDescriptorContext^) override
    {
      array<String^>^ blend_mode = gcnew array<String^> (3);

      blend_mode [0] = "solid";
      blend_mode [1] = "filter";
      blend_mode [2] = "add";

      return gcnew TypeConverter::StandardValuesCollection (blend_mode);
    }
};


public ref class ProjTypeConverter : StringConverter 
{
  public:
    virtual bool GetStandardValuesSupported (ITypeDescriptorContext^) override
    {
      return true;
    }

    virtual TypeConverter::StandardValuesCollection^ GetStandardValues (ITypeDescriptorContext^) override
    {
      array<String^>^ proj_type = gcnew array<String^> (8);

      proj_type [0] = "ortho";
      proj_type [1] = "isometry";
      proj_type [2] = "dimetry";
      proj_type [3] = "trimetry";
      proj_type [4] = "cabinet";
      proj_type [5] = "free";
      proj_type [6] = "perspective";
      proj_type [7] = "perspective_special";
//      proj_type [6] = "perspective1";
//      proj_type [7] = "perspective2";
//      proj_type [8] = "perspective3";

      return gcnew TypeConverter::StandardValuesCollection (proj_type);
    }

};

public ref class GeomTypeConverter : StringConverter 
{
  public:
    virtual bool GetStandardValuesSupported (ITypeDescriptorContext^) override
    {
      return true;
    }

    virtual TypeConverter::StandardValuesCollection^ GetStandardValues (ITypeDescriptorContext^) override
    {
      array<String^>^ geom_type = gcnew array<String^> (7);

      geom_type [0] = "box";
      geom_type [1] = "sphere";
      geom_type [2] = "cylinder";
      geom_type [3] = "cone";
      geom_type [4] = "icosahedron";
      geom_type [5] = "dodecahderon";
      geom_type [6] = "octahedron";

      return gcnew TypeConverter::StandardValuesCollection (geom_type);
    }
};

public ref class TrackForm : public System::Windows::Forms::UserControl
{
  public :

    TrackForm ()
    {
//      MaximizeBox = false;
//      MinimizeBox = false;
//      ControlBox = false;
//      ShowInTaskbar = false;
//      FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
    }

    ~TrackForm () {}
};

// properties to provide a design-mode angle selection interface.
[System::Security::Permissions::PermissionSetAttribute
(System::Security::Permissions::SecurityAction::InheritanceDemand, Name="FullTrust")]
[System::Security::Permissions::PermissionSetAttribute
(System::Security::Permissions::SecurityAction::LinkDemand, Name="FullTrust")]
public ref class FloatEditor : System::Drawing::Design::UITypeEditor
{
  public:

    virtual System::Drawing::Design::UITypeEditorEditStyle GetEditStyle ( System::ComponentModel::ITypeDescriptorContext^ context ) override
    {
      return UITypeEditorEditStyle::DropDown;
    }

    virtual Object^ EditValue ( System::ComponentModel::ITypeDescriptorContext^ context, System::IServiceProvider^ provider, Object^ value ) override
    {
       // Return the value if the value is not of type Int32, Double and Single.
       if ( value->GetType() != double::typeid && value->GetType() != float::typeid && value->GetType() != int::typeid )
          return value;
       
       // Uses the IWindowsFormsEditorService* to display a
       // drop-down UI in the Properties window.
       IWindowsFormsEditorService^ wfes = dynamic_cast<IWindowsFormsEditorService^>(provider->GetService( IWindowsFormsEditorService::typeid ));
       if (wfes != nullptr)
       {
         TrackForm^ trackForm = gcnew TrackForm ();
 //        trackForm->trackBar->Value = (int) value;
 //        trackForm->BarValue = trackForm->trackBar->Value;
 //        trackForm->wfes = wfes;

          wfes->DropDownControl (trackForm);
 //         value = trackForm->BarValue;
        }

       return value;
    }
  private:
     void PaintValue( System::Drawing::Design::PaintValueEventArgs^ e ) new
     {
      
     }

     // Indicates whether the UITypeEditor supports painting a
     // representation of a property's value.
     bool GetPaintValueSupported( System::ComponentModel::ITypeDescriptorContext^ context ) new
     {
        return true;
     }

};

}