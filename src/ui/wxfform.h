#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
#using <WeifenLuo.WinFormsUI.Docking.dll>

namespace My3D
{
	public ref class WXFForm : public WeifenLuo::WinFormsUI::DockContent
	{
    public :
      WXFForm ();

    protected :
      ~WXFForm ();

    public:
      void Update ();
      void DeleteWXFFIle ();

    private:
      void InitializeComponent ();
	    void reloadBtn_Click (System::Object^, System::EventArgs^);
	    void TextChanged (System::Object^, System::EventArgs^);

    private:
      System::ComponentModel::Container^   components;
		  System::Windows::Forms::RichTextBox^ WXFScript;
		  System::Windows::Forms::Button^      reloadBtn;

      bool need_save;
  };
}

