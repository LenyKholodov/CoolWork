#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
#using <WeifenLuo.WinFormsUI.Docking.dll>

namespace My3D
{
	public ref class OutputForm : public WeifenLuo::WinFormsUI::DockContent
	{
    public :
      OutputForm ();

    protected :
      ~OutputForm ();

    private:
      void InitializeComponent ();

		public:
      void LogMessage (System::String^ msg);
      void LogMessage (const char * msg);
      void ClearOutput ();
      void FocusConsole ();

    private:
      void Commander_KeyPress (System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^  e);

    private:
      System::ComponentModel::Container ^components;
		  System::Windows::Forms::TextBox^ Outputter;
		  System::Windows::Forms::ComboBox^ Commander;
  };
}

