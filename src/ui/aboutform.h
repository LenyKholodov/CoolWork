#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

namespace My3D
{
	public ref class AboutForm : System::Windows::Forms::Form
	{
    public :
		  AboutForm (bool timeout);
		  ~AboutForm ();

		protected:
      virtual void OnPaint (System::Windows::Forms::PaintEventArgs^ e) override;

		private:
      void InitializeComponent ();
		  void AboutFormClick (System::Object^ sender, System::EventArgs^ e);
		  void timerTick (System::Object^ sender, System::EventArgs^ e);

    private:
		  System::ComponentModel::Container^ components;
		  System::Windows::Forms::Timer^ timer;
		  System::Drawing::Bitmap^ bmp;
	};
}
