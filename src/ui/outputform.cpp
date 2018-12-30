#include "outputform.h"
#include "GUI.h"
#include "utils.h"

#include <core.h>

using namespace System;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace WeifenLuo::WinFormsUI;

namespace My3D {

OutputForm::OutputForm ()
{
  InitializeComponent ();
}

OutputForm::~OutputForm ()
{
  if (components)
  {
    delete components;
  }
}

#pragma region Windows Form Designer generated code

void OutputForm::InitializeComponent ()
{
  this->components = gcnew System::ComponentModel::Container();

  this->Commander = gcnew System::Windows::Forms::ComboBox ();
  this->Outputter = gcnew System::Windows::Forms::TextBox ();
  this->SuspendLayout ();
  // 
  // Commander
  // 
  this->Commander->BackColor = System::Drawing::Color::White;
  this->Commander->ForeColor = System::Drawing::Color::Black;
  this->Commander->Dock = System::Windows::Forms::DockStyle::Bottom;
  this->Commander->Font = gcnew Drawing::Font ("Verdana", 10.0F, FontStyle::Regular, GraphicsUnit::Point, 204);
//  this->Commander->FormattingEnabled = true;
//  this->Commander->FlatStyle = FlatStyle::Flat;
  this->Commander->Name = L"Commander";
  this->Commander->TabIndex = 0;
  this->Commander->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &OutputForm::Commander_KeyPress);
  // 
  // Outputter
  // 
  this->Outputter->BackColor = System::Drawing::Color::White;
  this->Outputter->Dock = System::Windows::Forms::DockStyle::Fill;
  this->Outputter->Location = System::Drawing::Point (0, 0);
  this->Outputter->Multiline = true;
  this->Outputter->Name = "Outputter";
  this->Outputter->ReadOnly = true;
  this->Outputter->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
  this->Outputter->Size = System::Drawing::Size (360, 373);
  this->Outputter->TabIndex = 1;
  this->Outputter->Text = "";
  // 
  // OutputForm
  // 
  this->AutoScaleBaseSize = System::Drawing::Size (6, 14);
  this->ClientSize = System::Drawing::Size (360, 373);
  this->Controls->Add (this->Outputter);
  this->Controls->Add (this->Commander);
  this->Font = gcnew Drawing::Font ("Verdana", 8.25F, FontStyle::Regular, GraphicsUnit::Point, 0);
  this->Name = "OutputForm";
  this->ShowHint = WeifenLuo::WinFormsUI::DockState::DockBottom/*AutoHide*/;
  this->TabText = "Output";
  this->Text = "Output";
  this->HideOnClose = true;
  this->ResumeLayout (false);
}

#pragma endregion

void OutputForm::FocusConsole ()
{
//  LogMessage ("Commander->Focus");
  Commander->Focus ();
}


void OutputForm::Commander_KeyPress (Object^ sender, KeyPressEventArgs^  e)
{
  if ( int (e->KeyChar) == 13 )
  {
    if (Commander->Text == "restart")
    {
      GUI::Editor->CloseDocument ();
      Application::Restart ();
    }
    if ( !Commander->Items->Contains (Commander->Text) )
      Commander->Items->Insert (0, Commander->Text);

    AutoStr cmd (Commander->Text);
    CoreDoCommand (cmd.c_str ());

    Commander->Text = L"";
  }
}

void OutputForm::LogMessage (const char* msg)
{
  LogMessage (gcnew String (msg));
}

void OutputForm::LogMessage (String^ msg)
{
  Outputter->Text += Environment::NewLine + msg;
  Outputter->SelectionStart = this->Outputter->Text->Length;
  Outputter->ScrollToCaret ();
}

void OutputForm::ClearOutput ()
{
  this->Outputter->Text = "";
}

}