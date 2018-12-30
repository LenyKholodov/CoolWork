#include "editorform.h"
#include "renderform.h"
#include "utils.h"
#include "GUI.h"
#include <core.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
using namespace WeifenLuo::WinFormsUI;

namespace My3D {

RenderForm::RenderForm (EditorForm^ editor)
{
  InitializeComponent ();

  this->editor = editor;
}

RenderForm::~RenderForm ()
{
  if (CoreIsRenderInit ())
    CoreShutdownRender ();
}

void RenderForm::InitializeComponent ()
{
  Name           = "RenderForm";
  TabText        = "Render";
  Text           = "Render";
  ShowHint       = WeifenLuo::WinFormsUI::DockState::Document;
  DockableAreas  = ((DockAreas)((DockAreas::Float | DockAreas::Document)));
  MinimumSize    = System::Drawing::Size (270, 300);
  HideOnClose    = true;
  ShowHint       = WeifenLuo::WinFormsUI::DockState::Document;
//  AllowDrop      = true;
  DoubleBuffered = true;

  DockStateChanged += gcnew EventHandler (this, &RenderForm::RenderForm_DockChanged);
  Resize           += gcnew EventHandler (this, &RenderForm::RenderForm_Resize);
  Deactivate       += gcnew EventHandler (this, &RenderForm::RenderForm_Deactivate);
//  DragDrop         += gcnew DragEventHandler (this, &RenderForm::RenderForm_DragDrop);
//  DragOver         += gcnew DragEventHandler (this, &RenderForm::RenderForm_DragOver);

  ResumeLayout (false);
}

void RenderForm::RenderForm_DockChanged (System::Object^ /*sender*/, EventArgs^  /*e*/)
{
  if (CoreIsRenderInit ()) 
    CoreShutdownRender ();

  CoreInitRender ( (void*) Handle );
}

void RenderForm::RenderForm_Resize (Object^ /*sender*/, EventArgs^ /*e*/)
{
  Rectangle^ rect = this->ClientRectangle;
  CoreResize (rect->Left,rect->Top,rect->Width,rect->Height);
}

void RenderForm::RenderForm_Deactivate (System::Object^, System::EventArgs^)
{
  GUI::LogMessage ("RenderForm deactivated");  
}

void RenderForm::RenderForm_DragDrop (System::Object^ sender, System::Windows::Forms::DragEventArgs^ e)
{
/*
  ToolBoxItem^ dragItem = nullptr;
  String strItem = "";

  if (e->Data->GetDataPresent (Silver::UI::ToolBoxItem::GetType ()))
  {
    dragItem = e->Data->GetData (Silver::UI::ToolBoxItem::GetType ()); //as ToolBoxItem;

    if (nullptr != dragItem && nullptr != dragItem->Object)
    {
      strItem = dragItem->Object->ToString ();
      MessageBox::Show (this, strItem, "Drag Drop");
    }
  }
*/
  editor->LogMessage ("Drag Drop!!!");
}

void RenderForm::RenderForm_DragOver (System::Object^ sender, System::Windows::Forms::DragEventArgs^ e)
{
/*
  if (e->Data->GetDataPresent (Silver::UI::ToolBoxItem))
  {
    e->Effect = DragDropEffects::Copy;
  }
*/
  editor->LogMessage ("Drag Over!!!");  
}

}

