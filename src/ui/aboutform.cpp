#include "aboutform.h"
#include "GUI.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Collections;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace System::Reflection;

namespace My3D
{
AboutForm::AboutForm (bool timeout)
{
  InitializeComponent ();

  bmp = static_cast <Bitmap^> (GUI::Editor->Resources->GetObject ("ABOUT"));

  if (bmp == nullptr)
    Close ();

  if (timeout)
    timer->Start ();
}

AboutForm::~AboutForm ()
{
  if (components)
    delete components;
}

#pragma region Windows Form Designer generated code

void AboutForm::InitializeComponent ()
{
  components = gcnew System::ComponentModel::Container ();
  timer = gcnew System::Windows::Forms::Timer (components);

  timer->Interval = 1500;
  timer->Tick += gcnew System::EventHandler (this, &AboutForm::timerTick);
  //
  // AboutForm
  //
  AutoScaleBaseSize = System::Drawing::Size (5, 13);
  ClientSize = System::Drawing::Size (642, 426);
  FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
  Name = "AboutForm";
  ShowInTaskbar = false;
  StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
  Text = "О программе";
  Click += gcnew System::EventHandler (this, &AboutForm::AboutFormClick);
}

#pragma endregion

void AboutForm::OnPaint (PaintEventArgs^ e) 
{
  Graphics^ g = e->Graphics;
  if (bmp != nullptr)
    g->DrawImage (bmp,0,0,642,426);

  Form::OnPaint (e);
}

void AboutForm::AboutFormClick (Object^ sender, EventArgs^ e)
{
  Close ();
}

void AboutForm::timerTick (Object^ sender, System::EventArgs^ e)
{
  timer->Stop ();
  Close ();
}

}
