#include "wxfform.h"
#include "utils.h"
#include "GUI.h"

#include <core.h>

using namespace System;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace WeifenLuo::WinFormsUI;

namespace My3D {

WXFForm::WXFForm ()
{
  need_save = false;
  InitializeComponent ();
}

WXFForm::~WXFForm ()
{
  if (components)
  {
    delete components;
  }
}

#pragma region Windows Form Designer generated code

void WXFForm::InitializeComponent ()
{
  components = gcnew System::ComponentModel::Container();

  WXFScript = gcnew System::Windows::Forms::RichTextBox ();
  reloadBtn = gcnew System::Windows::Forms::Button ();
  SuspendLayout ();
  // 
  // reloadBtn
  // 
  reloadBtn->Dock = System::Windows::Forms::DockStyle::Top;
  reloadBtn->Name = L"reloadBtn";
  reloadBtn->TabIndex = 0;
  reloadBtn->Text = L"Reload";
  reloadBtn->UseVisualStyleBackColor = true;
  reloadBtn->Click += gcnew System::EventHandler (this, &WXFForm::reloadBtn_Click);
  // 
  // WXFData
  // 
  WXFScript->Dock = System::Windows::Forms::DockStyle::Fill;
  WXFScript->Name = L"WXFData";
  WXFScript->TabIndex = 1;
  WXFScript->Text = L"";
  WXFScript->TextChanged += gcnew System::EventHandler (this, &WXFForm::TextChanged);
  // 
  // Scriptform
  // 
  AutoScaleBaseSize = System::Drawing::Size (6, 14);
  Controls->Add (WXFScript);
  Controls->Add (reloadBtn);
  Font = gcnew Drawing::Font ("Verdana", 8.25F, FontStyle::Regular, GraphicsUnit::Point, 0);
  Name = "Scriptform";
  ShowHint = WeifenLuo::WinFormsUI::DockState::DockLeftAutoHide;
  TabText = "WXF Script";
  Text = "WXF";
  HideOnClose = true;
  ResumeLayout (false);
}

#pragma endregion

void WXFForm::DeleteWXFFIle ()
{
  if ( !GUI::IsValidScene () )
    return;

  try
  {                                 
    String^ scene_filename = gcnew String (CoreGetCurrentSceneFileName ());
    String^ fname = gcnew String (scene_filename == String::Empty ? "./.wxfform" : scene_filename + ".wxfform");

    IO::File::Delete (fname);
  }
  catch ( Exception^ e ) 
  {
     Console::WriteLine ("WXFForm: file delete failed: {0}", e);
  }
}

void WXFForm::Update ()
{}

void WXFForm::TextChanged (System::Object^ sender, System::EventArgs^ e)
{
  need_save = true;
}

void WXFForm::reloadBtn_Click (System::Object^ sender, System::EventArgs^ e)
{
  if ( !GUI::IsValidScene () )
    return;

  GUI::Editor->DisableRefresh ();

  String^ scene_filename = gcnew String (CoreGetCurrentSceneFileName ());
  String^ fname = scene_filename == String::Empty ? "./.wxfform" : scene_filename + ".wxfform";

  AutoStr save_cmd ("save '" + fname + "'");
  CoreDoCommand (save_cmd.c_str ());

  GUI::Editor->curDoc->DeleteContents ();

  if (need_save)
    WXFScript->SaveFile (fname, RichTextBoxStreamType::PlainText);

  AutoStr load_cmd ("load '" + fname + "'");
  CoreDoCommand (load_cmd.c_str ());

  WXFScript->LoadFile (fname, RichTextBoxStreamType::PlainText);
  need_save = !need_save;

  GUI::Editor->EnableRefresh ();
}

}