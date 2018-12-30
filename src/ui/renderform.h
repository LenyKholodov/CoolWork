#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
#using <WeifenLuo.WinFormsUI.Docking.dll>
#using <ToolBox.dll>

namespace My3D
{
  ref class EditorForm;

  public ref class RenderForm : public WeifenLuo::WinFormsUI::DockContent
  {
    public :
      RenderForm (EditorForm^ parent);

    protected :
      ~RenderForm ();

    private :
      void InitializeComponent ();

      void RenderForm_DockChanged (System::Object^, System::EventArgs^);
      void RenderForm_Resize      (System::Object^, System::EventArgs^);
      void RenderForm_Deactivate  (System::Object^, System::EventArgs^);
      void RenderForm_DragDrop    (System::Object^ sender, System::Windows::Forms::DragEventArgs^ e);
      void RenderForm_DragOver    (System::Object^ sender, System::Windows::Forms::DragEventArgs^ e);

    private :
      EditorForm^ editor;
  };
}
