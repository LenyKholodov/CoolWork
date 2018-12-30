#include "editorform.h"
#include "renderform.h"
#include "outputform.h"
#include "propertyform.h"
#include "aboutform.h"
#include "wxfform.h"
#include "document.h"
#include "GUI.h"
#include "utils.h"

#include <core.h>

using namespace System;
using namespace System::Collections;
using namespace System::Reflection;
using namespace System::Resources;
using namespace System::ComponentModel;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Resources;
using namespace WeifenLuo::WinFormsUI;

namespace My3D {

void OnSceneCreate ();
void OnSceneClose ();  

/*
===============================================================================

  Свойства редактора

===============================================================================
*/

Object^ EditorForm::ActiveEntity::get ()
{
  return propertyForm->ActiveEntity;
}

void EditorForm::ActiveEntity::set (Object^ value)
{
  propertyForm->ActiveEntity = value;
}

PropertyForm^ EditorForm::PropertyPanel::get ()
{
  return propertyForm;
}

WXFForm^ EditorForm::WXF::get ()
{
  return wxfForm;
}

ResourceManager^ EditorForm::Resources::get ()
{
  return resources;
}

/*
===============================================================================

  Конструктор / деструктор

===============================================================================
*/

EditorForm::EditorForm (void)
{
  InitializeComponent ();

  ActivateDocumentMode (false);
}

EditorForm::~EditorForm ()
{
  if (this->components)
  {
    delete this->components;
  }
}

/*
===============================================================================

  Хэлперы

===============================================================================
*/

void EditorForm::CreateShape (const char * create_cmd)
{
  DisableRefresh ();

  CoreDoCommand (create_cmd);

  EnableRefresh ();

}

void EditorForm::EnableRefresh ()
{
  propertyForm->RefreshEnabled = true;
}

void EditorForm::DisableRefresh ()
{
  propertyForm->RefreshEnabled = false;
}

void EditorForm::EditorForm_Shown (Object^ sender, EventArgs^ e)
{
  NewDocument ();
}

void EditorForm::EditorForm_GotFocus (Object^ sender, EventArgs^ e)
{
  outputForm->FocusConsole ();
}

void EditorForm::EditorForm_Closing (Object^ /*sender*/, System::ComponentModel::CancelEventArgs^ e)
{
  e->Cancel = true;
  Exit ();
}

void EditorForm::LogMessage (const char* msg)
{
  outputForm->LogMessage (msg);
}

void EditorForm::LogMessage (System::String^ msg)
{
  outputForm->LogMessage (msg);
}

void EditorForm::ShowRender ()
{
  DisableRefresh ();

  if (renderForm == nullptr) 
    renderForm = gcnew RenderForm (this);

  renderForm->Show (dockPanel);

  OnSceneCreate ();

  EnableRefresh ();
}

void EditorForm::CloseRender ()
{
  OnSceneClose ();

  CoreDoCommand ("reset");

  renderForm->Close ();
  delete renderForm;
  renderForm = nullptr;
}

void EditorForm::ActivateCursor ()
{
  SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
}

void EditorForm::ManipulatorChanged (String^ manipulator_name)
{
//  GUI::LogMessage ("ManipulatorChanged:" + manipulator_name);

  if (manipulator_name == "select")  
    SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
  else if (manipulator_name == "move")
    SetToolButtonsState (0,1,0,0,0,0,0,0,0,0,0); 
  else if (manipulator_name == "rotate")
    SetToolButtonsState (0,0,1,0,0,0,0,0,0,0,0);
  else if (manipulator_name == "scale")
    SetToolButtonsState (0,0,0,1,0,0,0,0,0,0,0);
  else if (manipulator_name == "shear")
    SetToolButtonsState (0,0,0,0,1,0,0,0,0,0,0);
  else if (manipulator_name == "bind")
    SetToolButtonsState (0,0,0,0,0,1,0,0,0,0,0);
  else if (manipulator_name == "pencil_extrude")
    SetToolButtonsState (0,0,0,0,0,0,1,0,0,0,0);
  else if (manipulator_name == "brush_extrude")
    SetToolButtonsState (0,0,0,0,0,0,0,1,0,0,0);
  else if (manipulator_name == "camera_zoom")
    SetToolButtonsState (0,0,0,0,0,0,0,0,1,0,0);
  else if (manipulator_name == "camera_move")
    SetToolButtonsState (0,0,0,0,0,0,0,0,0,1,0);
  else if (manipulator_name == "camera_rotate")
    SetToolButtonsState (0,0,0,0,0,0,0,0,0,0,1);
}

/*
===============================================================================

  Работа с документом

===============================================================================
*/

void EditorForm::NewDocument ()
{
  if (state == EditorState::DocumentProcessing)
    CloseDocument ();

  CreateDoc ();
}

void EditorForm::ActivateDocumentMode (bool enable)
{
  renderMenuItem->Enabled     = enable;
  closeMenuItem->Enabled      = enable;
  saveMenuItem->Enabled       = enable;
  saveAsMenuItem->Enabled     = enable;
  saveButton->Enabled         = enable;
  closeButton->Enabled        = enable;
  cursorButton->Enabled       = enable;
  moveButton->Enabled         = enable;
  rotateButton->Enabled       = enable;
  scaleButton->Enabled        = enable;
  cameraZoomButton->Enabled   = enable;
  cameraMoveButton->Enabled   = enable;
  cameraRotateButton->Enabled = enable;
  createObjStripMenu->Enabled = enable;

  if (enable == true)
  {
    state = EditorState::DocumentProcessing;
    cursorButton->CheckState = CheckState::Checked;
    propertyForm->EnableToolBox ();
  }
  else
  {
    state = EditorState::Idle;
    propertyForm->DisableToolBox ();
  }
}

void EditorForm::CreateDoc ()
{
  if (curDoc != nullptr)
    delete curDoc;

  curDoc = gcnew Document ();

  ActivateDocumentMode (true);

  ShowRender ();
}

void EditorForm::OpenDocument ()
{
  LogMessage ("Open document");

  if (state == EditorState::DocumentProcessing)
    CloseDocument ();

  CreateDoc ();

  DisableRefresh ();

  wxfForm->DeleteWXFFIle ();

  curDoc->Load ();

  EnableRefresh ();
}

void EditorForm::SaveDocument ()
{
  LogMessage ("Save document");

  if (curDoc->NeedSave == false)
    return;

  curDoc->Save ();
}

void EditorForm::SaveAsDocument ()
{
  LogMessage ("Saveas document");

  wxfForm->DeleteWXFFIle ();

  curDoc->SaveAs ();
}

void EditorForm::CloseDocument ()
{
  LogMessage ("Close document: ");

  if (curDoc == nullptr)
    return;

  if (curDoc->NeedSave == true)
  {
    Windows::Forms::DialogResult result;
    result = MessageBox::Show (this, "Сохранить изменения в документе?", "My3D", MessageBoxButtons::YesNoCancel);

    if (result == Windows::Forms::DialogResult::Yes)
      SaveDocument ();

    if (result == Windows::Forms::DialogResult::Cancel)
      return;
  }

  wxfForm->DeleteWXFFIle ();
  
  ActivateDocumentMode (false);

  CloseRender ();

  curDoc->Close ();
  delete curDoc;
  curDoc = nullptr;
}

/*
===============================================================================

  Команды меню

===============================================================================
*/

void EditorForm::Exit ()
{
  if (curDoc != nullptr)
    CloseDocument ();

  if (curDoc != nullptr)
    return;

  Application::Exit ();
}

void EditorForm::ClearAll ()
{
  if (curDoc != nullptr)
    curDoc->DeleteContents ();

//  GUI::PropertyControl->SceneTree->Nodes->Clear ();
}

void EditorForm::About ()
{
  AboutForm^ about= gcnew AboutForm (false);
  about->ShowDialog (this);
}

void EditorForm::HelpTopics ()
{
  Help::ShowHelp (this, "coolwork.chm");
}

void EditorForm::SetToolButtonsState (int cursor, int move, int rotate, int scale, int shear, int bind, int pencil_extrude, int brush_extrude, 
                                      int cam_zoom, int cam_move, int cam_rotate)
{
  cursorButton->Checked        = cursor;
  moveButton->Checked 	       = move;
  rotateButton->Checked        = rotate;
  scaleButton->Checked 	       = scale;
  shearButton->Checked 	       = shear;
  bindButton->Checked 	       = bind;
  pencilExtrudeButton->Checked = pencil_extrude;
  brushExtrudeButton->Checked  = brush_extrude;
  cameraZoomButton->Checked    = cam_zoom;
  cameraRotateButton->Checked  = cam_rotate;
  cameraMoveButton->Checked    = cam_move;
}

/*
===============================================================================

  Обработчик комагд меню

===============================================================================
*/

void EditorForm::ToolbarHandler (Object^ sender, EventArgs^ /*e*/)
{
  if (sender->Equals (newButton))
  {
    NewDocument ();
  }
  else
  if (sender->Equals (openButton))
  {
    OpenDocument ();
  }
  else  
  if (sender->Equals (saveButton))
  {
    SaveDocument ();
  }
  else  
  if (sender->Equals (closeButton))
  {
    CloseDocument ();
  }
  else
  if (sender->Equals (helpButton))
  {
    HelpTopics ();
  }
  else
  if (sender->Equals (cursorButton))
  {
    SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
    CoreDoCommand ("select");
  }
  else
  if (sender->Equals (moveButton))
  {
    if (moveButton->Checked)
    {
      SetToolButtonsState (0,1,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("move");
    }
    else
    {
      SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("select");
    }
  }
  else
  if (sender->Equals (rotateButton))
  {
    if (rotateButton->Checked)
    {
      SetToolButtonsState (0,0,1,0,0,0,0,0,0,0,0);
      CoreDoCommand ("rotate");
    }
    else
    {
      SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("select");
    }
  }
  else
  if (sender->Equals (scaleButton))
  {
    if (scaleButton->Checked)
    {
      SetToolButtonsState (0,0,0,1,0,0,0,0,0,0,0);
      CoreDoCommand ("scale");
    }
    else
    {
      SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("select");
    }
  }
  else
  if (sender->Equals (shearButton))
  {
    if (shearButton->Checked)
    {
      SetToolButtonsState (0,0,0,0,1,0,0,0,0,0,0);
      CoreDoCommand ("shear");
    }
    else
    {
      SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("select");
    }
  }
  else
  if (sender->Equals (bindButton))
  {
    if (bindButton->Checked)
    {
      SetToolButtonsState (0,0,0,0,0,1,0,0,0,0,0);
      CoreDoCommand ("bind");
    }
    else
    {
      SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("select");
    }
  }
  else
  if (sender->Equals (pencilExtrudeButton))
  {
    if (pencilExtrudeButton->Checked)
    {
      SetToolButtonsState (0,0,0,0,0,0,1,0,0,0,0);
      CoreDoCommand ("pencil_extrude 10 30 0");
    }
    else
    {
      SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("select");
    }
  }
  else
  if (sender->Equals (brushExtrudeButton))
  {
    if (brushExtrudeButton->Checked)
    {
      SetToolButtonsState (0,0,0,0,0,0,0,1,0,0,0);
      CoreDoCommand ("brush_extrude 10 30 0");
    }
    else
    {
      SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("select");
    }
  }
  else
  if (sender->Equals (cameraZoomButton))
  {
    if (cameraZoomButton->Checked)
    {
      SetToolButtonsState (0,0,0,0,0,0,0,0,1,0,0);
      CoreDoCommand ("camera zoom");
    }
    else
    {
      SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("select");
    }
  }
  else
  if (sender->Equals (cameraMoveButton))
  {
    if (cameraMoveButton->Checked == true)
    {
      SetToolButtonsState (0,0,0,0,0,0,0,0,0,1,0);
      CoreDoCommand ("camera move");
    }
    else
    {
      SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("select");
    }
  }
  else
  if (sender->Equals (cameraRotateButton))
  {
    if (cameraRotateButton->Checked)
    {
      SetToolButtonsState (0,0,0,0,0,0,0,0,0,0,1);
      CoreDoCommand ("camera rotate");
    }
    else
    {
      SetToolButtonsState (1,0,0,0,0,0,0,0,0,0,0);
      CoreDoCommand ("select");
    }
  }
}

/*
===============================================================================

  Команды панели инструментов

===============================================================================
*/

void EditorForm::boxToolStripMenuItem_Click (System::Object^ sender, System::EventArgs^ e)
{
  CoreDoCommand ("create box");  
}

void EditorForm::sphereToolStripMenuItem_Click (System::Object^ sender, System::EventArgs^ e)
{
  CoreDoCommand ("create sphere");
}

void EditorForm::cylinderToolStripMenuItem_Click (System::Object^ sender, System::EventArgs^ e)
{
  CoreDoCommand ("create cylinder");
}

void EditorForm::coneToolStripMenuItem_Click (System::Object^ sender, System::EventArgs^ e)
{
  CoreDoCommand ("create cone");
}

void EditorForm::torusToolStripMenuItem_Click (System::Object^ sender, System::EventArgs^ e)
{
  CoreDoCommand ("create torus");
}

/*
===============================================================================

  Обработчкие команд панели инструментов

===============================================================================
*/

void EditorForm::MenuItemHandler (Object^ sender, EventArgs^ /* e */)
{
   if ( sender->Equals ( newMenuItem ) )
   {
      NewDocument ();
   }
   else
   if ( sender->Equals( openMenuItem ) )
   {
      OpenDocument ();
   }
   else
   if ( sender->Equals( saveMenuItem ) )
   {
      SaveDocument ();
   }
   else
   if ( sender->Equals( saveAsMenuItem ) )
   {
      SaveAsDocument ();
   }
   else
   if ( sender->Equals( closeMenuItem ) )
   {
      CloseDocument ();
   }
   else
   if ( sender->Equals( exitMenuItem ) )
   {
      Exit ();
   }
   else
   if ( sender->Equals( clearAllMenuItem ) )
   {
      ClearAll ();
   }
   else
   if ( sender->Equals( removeDepsMenuItem ) )
   {
      CoreDoCommand ("remove_deps");
   }
//
//  temporary
//
   else
   if ( sender->Equals( accPlusBrushMenuItem ) )
   {
      CoreDoCommand ("brush_extrude 10 30 2");
   }
   else
   if ( sender->Equals( accMinusBrushMenuItem ) )
   {
      CoreDoCommand ("brush_extrude 10 -30 2");
   }
   else
   if ( sender->Equals( accPlusPencilMenuItem ) )
   {
      CoreDoCommand ("pencil_extrude 10 30 2");
   }
   else
   if ( sender->Equals( accMinusPencilMenuItem ) )
   {
      CoreDoCommand ("pencil_extrude 10 -30 2");
   }
   else
   if ( sender->Equals( noaccMinusBrushMenuItem ) )
   {
      CoreDoCommand ("brush_extrude 10 -30 0");
   }
   else
   if ( sender->Equals( noaccMinusPencilMenuItem ) )
   {
      CoreDoCommand ("pencil_extrude 10 -30 0");
   }
//
//
//
   else
   if ( sender->Equals( boxMenuItem ) )
   {
      CoreDoCommand ("create box");
   }
   else
   if ( sender->Equals( sphereMenuItem ) )
   {
      CoreDoCommand ("create sphere");
   }
   else
   if ( sender->Equals( cylinderMenuItem ) )
   {
      CoreDoCommand ("create cylinder");
   }
   else
   if ( sender->Equals( coneMenuItem ) )
   {
      CoreDoCommand ("create cone");
   }
   else
   if ( sender->Equals( torusMenuItem ) )
   {
      CoreDoCommand ("create torus");
   }
   else
   if ( sender->Equals( landscapeMenuItem ) )
   {
      CoreDoCommand ("create landscape");
   }
   else
   if ( sender->Equals( halfsphereMenuItem ) )
   {
      CoreDoCommand ("create halfsphere");
   }
   else
   if ( sender->Equals( halfcylinderMenuItem ) )
   {
      CoreDoCommand ("create halfcylinder");
   }
   else
   if ( sender->Equals( halfconeMenuItem) )
   {
      CoreDoCommand ("create halfcone");
   }
   else
   if ( sender->Equals( halftorusMenuItem ) )
   {
      CoreDoCommand ("create halftorus");
   }
   else
   if ( sender->Equals( dodecahedronMenuItem ) )
   {
      CoreDoCommand ("create dodecahedron");
   }
   else
   if ( sender->Equals( octahedronMenuItem ) )
   {
      CoreDoCommand ("create octahedron");
   }
   else
   if ( sender->Equals( hexahedronMenuItem ) )
   {
      CoreDoCommand ("create hexahedron");
   }
   else
   if ( sender->Equals( tetrahedronMenuItem ) )
   {
      CoreDoCommand ("create tetrahedron");
   }
   else
   if ( sender->Equals( icosahedronMenuItem ) )
   {
      CoreDoCommand ("create icosahedron");
   }
   else
   if ( sender->Equals( pointLightMenuItem ) )
   {
      CoreDoCommand ("create point");
   }
   else
   if ( sender->Equals( spotFreeLightMenuItem) )
   {
      CoreDoCommand ("create spot_free");
   }
   else
   if ( sender->Equals( spotTargetLightMenuItem ) )
   {
      CoreDoCommand ("create spot_target");
   }
   else
   if ( sender->Equals( directFreeLightMenuItem ) )
   {
	      CoreDoCommand ("create direct_free");
   }
   else
   if ( sender->Equals( directTargetLightMenuItem ) )
   {
      CoreDoCommand ("create direct_target");
   }
   else
   if ( sender->Equals( craneMenuItem ) )
   {
      CreateShape ("create crane");
   }
   else
   if ( sender->Equals( fdcMenuItem) )
   {
      CreateShape ("create fdc");
   }
   else
   if ( sender->Equals( headphonesMenuItem ) )
   {
      CreateShape ("create headphones");
   }
   else
   if ( sender->Equals( spaceshipMenuItem ) )
   {
	      CreateShape ("create spaceship");
   }
   else
   if ( sender->Equals( tankMenuItem ) )
   {
      CreateShape ("create tank");
   }
   else
   if ( sender->Equals (toolbarMenuItem) )
   {
      toolStrip->Visible =  !toolStrip->Visible;
      toolbarMenuItem->Checked = toolStrip->Visible;
   }
   else
   if (sender->Equals (statusbarMenuItem))
   {
      statusStrip->Visible = !statusStrip->Visible;
      statusbarMenuItem->Checked = statusStrip->Visible;
   }
   else
   if ( sender->Equals (renderMenuItem) )
   {
     ShowRender ();
   }
   else
   if ( sender->Equals (WXFMenuItem) )
   {
     wxfForm->Show (dockPanel);
   }
   if ( sender->Equals (outputMenuItem) )
   {
     outputForm->Show (dockPanel);
   }
   else
   if (sender->Equals (propertyMenuItem))
   {
     propertyForm->Show (dockPanel);
   }
   else
   if ( sender->Equals( helpAboutMenuItem ) )
   {
      About ();
   }
   else
   if ( sender->Equals( helpTopicsMenuItem ) )
   {
      HelpTopics ();
   }
}

/*
===============================================================================

  Создание и установка свойств компонентов интерфеса

===============================================================================
*/

#pragma region Windows Form Designer generated code

// Создание главного меню
void EditorForm::CreateMenu ()
{
  // 
  // File menu
  // 
  this->newMenuItem    = gcnew Windows::Forms::MenuItem ("&Новый", gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  this->openMenuItem   = gcnew Windows::Forms::MenuItem ("&Открыть...",gcnew EventHandler( this, &EditorForm::MenuItemHandler ) );
  this->closeMenuItem  = gcnew Windows::Forms::MenuItem ("&Закрыть",gcnew EventHandler( this, &EditorForm::MenuItemHandler ) );
  this->saveMenuItem   = gcnew Windows::Forms::MenuItem ("&Сохранить",gcnew EventHandler( this, &EditorForm::MenuItemHandler ) );
  this->saveAsMenuItem = gcnew Windows::Forms::MenuItem ("Сохранить &как",gcnew EventHandler( this, &EditorForm::MenuItemHandler ) );
  this->exitMenuItem   = gcnew Windows::Forms::MenuItem ("В&ыход",gcnew EventHandler( this, &EditorForm::MenuItemHandler ) );

  this->closeMenuItem->Enabled = false;
  this->saveMenuItem->Enabled = false;
  this->saveAsMenuItem->Enabled = false;

  array<Windows::Forms::MenuItem^>^fileItems = gcnew array<Windows::Forms::MenuItem^>(8);
  fileItems [0] = newMenuItem;
  fileItems [1] = openMenuItem;
  fileItems [2] = closeMenuItem;
  fileItems [3] = gcnew Windows::Forms::MenuItem ("-");
  fileItems [4] = saveMenuItem;
  fileItems [5] = saveAsMenuItem;
  fileItems [6] = gcnew Windows::Forms::MenuItem ("-");
  fileItems [7] = exitMenuItem;
  this->fileMenuItem = gcnew Windows::Forms::MenuItem( "&Файл",fileItems);
  // 
  // Edit menu
  // 
  removeDepsMenuItem = gcnew Windows::Forms::MenuItem ( "Убрать зависимости",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  clearAllMenuItem = gcnew Windows::Forms::MenuItem ( "Очистить все",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  array<Windows::Forms::MenuItem^>^ editItems = gcnew array<Windows::Forms::MenuItem^>(9);
  editItems [0] = removeDepsMenuItem;
  editItems [1] = gcnew Windows::Forms::MenuItem ("-");
  editItems [2] = clearAllMenuItem;

  //
  // Temporary From Gred :)
  //
      
  accPlusBrushMenuItem = gcnew Windows::Forms::MenuItem ( "Brush (positive, accumulation)",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  accMinusBrushMenuItem = gcnew Windows::Forms::MenuItem ( "Brush (negative, accumulation)",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  accPlusPencilMenuItem = gcnew Windows::Forms::MenuItem ( "Pencil (positive, accumulation)",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  accMinusPencilMenuItem = gcnew Windows::Forms::MenuItem ( "Pencil (negative, accumulation)",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  noaccMinusBrushMenuItem = gcnew Windows::Forms::MenuItem ( "Brush (negative, no accumulation)",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  noaccMinusPencilMenuItem = gcnew Windows::Forms::MenuItem ( "Pencil (negative, no accumulation)",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  array<Windows::Forms::MenuItem^>^ manipulatorItems = gcnew array<Windows::Forms::MenuItem^>(6);
  editItems [3] = accPlusBrushMenuItem;
  editItems [4] = accMinusBrushMenuItem;
  editItems [5] = accPlusPencilMenuItem;
  editItems [6] = accMinusPencilMenuItem;
  editItems [7] = noaccMinusBrushMenuItem;
  editItems [8] = noaccMinusPencilMenuItem;
//  manipulatorMenuItem = gcnew Windows::Forms::MenuItem ("&Ландшафтные модификаторы", manipulatorItems);
  editMenuItem = gcnew Windows::Forms::MenuItem ("&Правка", editItems);

  // 
  // Tools menu
  // 
  boxMenuItem          = gcnew Windows::Forms::MenuItem ( "Параллелепипед", gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  sphereMenuItem       = gcnew Windows::Forms::MenuItem ( "Сфера",          gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  cylinderMenuItem     = gcnew Windows::Forms::MenuItem ( "Цилиндр",        gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  coneMenuItem         = gcnew Windows::Forms::MenuItem ( "Конус",          gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  torusMenuItem        = gcnew Windows::Forms::MenuItem ( "Тор",            gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  landscapeMenuItem    = gcnew Windows::Forms::MenuItem ( "Ландшафт",       gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  halfsphereMenuItem   = gcnew Windows::Forms::MenuItem ( "Полусфера",      gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  halfcylinderMenuItem = gcnew Windows::Forms::MenuItem ( "Полуцилиндр",    gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  halfconeMenuItem     = gcnew Windows::Forms::MenuItem ( "Полуконус",      gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  halftorusMenuItem    = gcnew Windows::Forms::MenuItem ( "Полутор",        gcnew EventHandler (this, &EditorForm::MenuItemHandler));

  array<Windows::Forms::MenuItem^>^ primitiveItems = gcnew array<Windows::Forms::MenuItem^>(11);
  primitiveItems [0]  = boxMenuItem;
  primitiveItems [1]  = sphereMenuItem;
  primitiveItems [2]  = cylinderMenuItem;
  primitiveItems [3]  = coneMenuItem;
  primitiveItems [4]  = torusMenuItem;
  primitiveItems [5]  = landscapeMenuItem;
  primitiveItems [6]  = gcnew Windows::Forms::MenuItem ("-");
  primitiveItems [7]  = halfsphereMenuItem;
  primitiveItems [8]  = halfcylinderMenuItem;
  primitiveItems [9]  = halfconeMenuItem;
  primitiveItems [10] = halftorusMenuItem;

  primitiveMenuItem = gcnew Windows::Forms::MenuItem ("Примитив", primitiveItems);

  tetrahedronMenuItem  = gcnew Windows::Forms::MenuItem ( "Тетраэдр",       gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  hexahedronMenuItem   = gcnew Windows::Forms::MenuItem ( "Гексаэдр",       gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  octahedronMenuItem   = gcnew Windows::Forms::MenuItem ( "Октаэдр",        gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  dodecahedronMenuItem = gcnew Windows::Forms::MenuItem ( "Додекаэдр",      gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  icosahedronMenuItem  = gcnew Windows::Forms::MenuItem ( "Икосаэдр",       gcnew EventHandler (this, &EditorForm::MenuItemHandler));

  array<Windows::Forms::MenuItem^>^ platonicsItems = gcnew array<Windows::Forms::MenuItem^>(5);
  platonicsItems [0] = tetrahedronMenuItem;
  platonicsItems [1] = hexahedronMenuItem;
  platonicsItems [2] = octahedronMenuItem;
  platonicsItems [3] = dodecahedronMenuItem;
  platonicsItems [4] = icosahedronMenuItem;

  platonicsMenuItem = gcnew Windows::Forms::MenuItem ("Платоново тело", platonicsItems);

  spaceshipMenuItem  = gcnew Windows::Forms::MenuItem ( "Космический корабль", gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  headphonesMenuItem = gcnew Windows::Forms::MenuItem ( "Наушники",            gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  craneMenuItem      = gcnew Windows::Forms::MenuItem ( "Подъемный кран",      gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  fdcMenuItem        = gcnew Windows::Forms::MenuItem ( "Пожарная машина",     gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  tankMenuItem       = gcnew Windows::Forms::MenuItem ( "Танк",                gcnew EventHandler (this, &EditorForm::MenuItemHandler));

  array<Windows::Forms::MenuItem^>^ shapesItems = gcnew array<Windows::Forms::MenuItem^>(5);
  shapesItems [0] = spaceshipMenuItem;
  shapesItems [1] = headphonesMenuItem;
  shapesItems [2] = craneMenuItem;
  shapesItems [3] = fdcMenuItem;
  shapesItems [4] = tankMenuItem;

  shapesMenuItem = gcnew Windows::Forms::MenuItem ("Сложный объект", shapesItems);

  pointLightMenuItem        = gcnew Windows::Forms::MenuItem ( "Point",         gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  spotFreeLightMenuItem     = gcnew Windows::Forms::MenuItem ( "Spot free",     gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  spotTargetLightMenuItem   = gcnew Windows::Forms::MenuItem ( "Spot target",   gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  directFreeLightMenuItem   = gcnew Windows::Forms::MenuItem ( "Direct free",   gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  directTargetLightMenuItem = gcnew Windows::Forms::MenuItem ( "Direct target", gcnew EventHandler (this, &EditorForm::MenuItemHandler));

  array<Windows::Forms::MenuItem^>^ lightItems = gcnew array<Windows::Forms::MenuItem^>(5);
  lightItems [0] = pointLightMenuItem;
  lightItems [1] = spotFreeLightMenuItem;
  lightItems [2] = spotTargetLightMenuItem;
  lightItems [3] = directFreeLightMenuItem;
  lightItems [4] = directTargetLightMenuItem;

  lightMenuItem = gcnew Windows::Forms::MenuItem ("Источник света", lightItems);

  array<Windows::Forms::MenuItem^>^ toolsItems = gcnew array<Windows::Forms::MenuItem^>(4);
  toolsItems [0] = primitiveMenuItem;
  toolsItems [1] = platonicsMenuItem;
  toolsItems [2] = lightMenuItem;
  toolsItems [3] = shapesMenuItem;

  toolsMenuItem = gcnew Windows::Forms::MenuItem ("Создать", toolsItems);
  // 
  // View menu
  // 
  this->propertyMenuItem  = gcnew Windows::Forms::MenuItem ("Менеджер",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  this->outputMenuItem    = gcnew Windows::Forms::MenuItem ("Консоль",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  this->renderMenuItem    = gcnew Windows::Forms::MenuItem ("Рендер",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  this->WXFMenuItem       = gcnew Windows::Forms::MenuItem ("WXF",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  this->toolbarMenuItem   = gcnew Windows::Forms::MenuItem ("Панель инструментов",gcnew EventHandler( this, &EditorForm::MenuItemHandler ) );
  this->statusbarMenuItem = gcnew Windows::Forms::MenuItem ("Панель статуса",gcnew EventHandler( this, &EditorForm::MenuItemHandler ) );

  this->toolbarMenuItem->Checked = true;
  this->statusbarMenuItem->Checked = true;

  array<Windows::Forms::MenuItem^>^viewItems = gcnew array<Windows::Forms::MenuItem^> (7);
  viewItems [0] = propertyMenuItem;
  viewItems [1] = outputMenuItem;
  viewItems [2] = renderMenuItem;
  viewItems [3] = WXFMenuItem;
  viewItems [4] = gcnew Windows::Forms::MenuItem ("-");
  viewItems [5] = toolbarMenuItem;
  viewItems [6] = statusbarMenuItem;
  this->viewMenuItem = gcnew Windows::Forms::MenuItem ("Вид", viewItems);
  // 
  // Help menu
  // 
  this->helpTopicsMenuItem = gcnew Windows::Forms::MenuItem ("Содержимое",gcnew EventHandler (this, &EditorForm::MenuItemHandler));
  this->helpAboutMenuItem = gcnew Windows::Forms::MenuItem( "&О программе",gcnew EventHandler( this, &EditorForm::MenuItemHandler));
  array<Windows::Forms::MenuItem^>^helpItems = gcnew array<Windows::Forms::MenuItem^>(3);
  helpItems [0] = helpTopicsMenuItem;
  helpItems [1] = gcnew Windows::Forms::MenuItem ("-");
  helpItems [2] = helpAboutMenuItem;
  this->helpMenuItem = gcnew Windows::Forms::MenuItem ("Справка", helpItems);
  // 
  // Main menu
  // 
  array<Windows::Forms::MenuItem^>^ MainMenuItems = gcnew array<Windows::Forms::MenuItem^>(5);
  MainMenuItems [0] = fileMenuItem;
  MainMenuItems [1] = editMenuItem;
  MainMenuItems [2] = toolsMenuItem;
  MainMenuItems [3] = viewMenuItem;
  MainMenuItems [4] = helpMenuItem;
  this->mainMenu = gcnew MainMenu (MainMenuItems);
}

// Создание панели инструментов
void EditorForm::CreateToolbar ()
{
  toolStrip   = gcnew Windows::Forms::ToolStrip ();
  toolStrip->SuspendLayout ();

  newButton   = gcnew Windows::Forms::ToolStripButton ();
  openButton  = gcnew Windows::Forms::ToolStripButton ();
  saveButton  = gcnew Windows::Forms::ToolStripButton ();
  closeButton = gcnew Windows::Forms::ToolStripButton ();
  helpButton  = gcnew Windows::Forms::ToolStripButton ();

  cursorButton = gcnew Windows::Forms::ToolStripButton ();
  moveButton   = gcnew Windows::Forms::ToolStripButton ();
  rotateButton = gcnew Windows::Forms::ToolStripButton ();
  scaleButton  = gcnew Windows::Forms::ToolStripButton ();
  shearButton  = gcnew Windows::Forms::ToolStripButton ();
  bindButton   = gcnew Windows::Forms::ToolStripButton ();

  pencilExtrudeButton = gcnew Windows::Forms::ToolStripButton ();
  brushExtrudeButton  = gcnew Windows::Forms::ToolStripButton ();

  createObjStripMenu        = gcnew Windows::Forms::ToolStripSplitButton ();
  boxToolStripMenuItem      = gcnew Windows::Forms::ToolStripMenuItem ();
  sphereToolStripMenuItem   = gcnew Windows::Forms::ToolStripMenuItem ();
  cylinderToolStripMenuItem = gcnew Windows::Forms::ToolStripMenuItem ();
  coneToolStripMenuItem     = gcnew Windows::Forms::ToolStripMenuItem ();
  torusToolStripMenuItem    = gcnew Windows::Forms::ToolStripMenuItem ();

  cameraMoveButton   = gcnew Windows::Forms::ToolStripButton ();
  cameraRotateButton = gcnew Windows::Forms::ToolStripButton ();
  cameraZoomButton   = gcnew Windows::Forms::ToolStripButton ();

  // 
  // toolStrip
  // 
  array <Windows::Forms::ToolStripItem^>^ toolItems = gcnew array<Windows::Forms::ToolStripItem^> (21);
  toolItems [0]  = newButton;
  toolItems [1]  = openButton;
  toolItems [2]  = saveButton;
  toolItems [3]  = closeButton;
  toolItems [4]  = gcnew Windows::Forms::ToolStripSeparator ();
  toolItems [5]  = cursorButton;
  toolItems [6]  = moveButton;
  toolItems [7]  = rotateButton;
  toolItems [8]  = scaleButton;
  toolItems [9]  = shearButton;
  toolItems [10] = bindButton;
  toolItems [11] = pencilExtrudeButton;
  toolItems [12] = brushExtrudeButton;
  toolItems [13] = gcnew Windows::Forms::ToolStripSeparator ();
  toolItems [14] = cameraZoomButton;
  toolItems [15] = cameraMoveButton;
  toolItems [16] = cameraRotateButton;
  toolItems [17] = gcnew Windows::Forms::ToolStripSeparator ();
  toolItems [18] =  createObjStripMenu;
  toolItems [19] = gcnew Windows::Forms::ToolStripSeparator ();
  toolItems [20] = helpButton;

  toolStrip->Items->AddRange (toolItems);
  toolStrip->Location = Drawing::Point (0, 0);
  toolStrip->Name = "toolStrip";
  toolStrip->Size = System::Drawing::Size (655, 25);
  toolStrip->TabIndex = 4;
  toolStrip->Text = "Tools";
  //             
  // newButton
  // 
  newButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  newButton->Image = Bitmap::FromFile ("media/images/NEW.BMP");
//  this->newButton->Image = ((Drawing::Image^)(resources->GetObject ("newButton.Image")));
  newButton->Image = static_cast <Bitmap^> (resources->GetObject ("NEW"));
  newButton->ImageTransparentColor = System::Drawing::Color::Silver;
  newButton->ImageTransparentColor = Drawing::Color::Magenta;
  newButton->Name = "newButton";
  newButton->Size = Drawing::Size (23, 22);
  newButton->Text = "Создать";
  newButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);
  // 
  // openButton
  // 
  openButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  openButton->Image = Bitmap::FromFile ("media/images/OPEN.BMP");
  openButton->Image = static_cast <Bitmap^> (resources->GetObject ("OPEN"));
  openButton->ImageTransparentColor = System::Drawing::Color::Silver;
  openButton->ImageTransparentColor = Drawing::Color::Magenta;
  openButton->Name = "openButton";
  openButton->Size = Drawing::Size (23, 22);
  openButton->Text = "Открыть";
  openButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);
  // 
  // saveButton
  // 
  saveButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  saveButton->Image = Bitmap::FromFile ("media/images/SAVE.BMP");
  saveButton->Image = static_cast <Bitmap^> (resources->GetObject ("SAVE"));
  saveButton->ImageTransparentColor = System::Drawing::Color::Silver;
  saveButton->ImageTransparentColor = Drawing::Color::Magenta;
  saveButton->Name = "saveButton";
  saveButton->Size = Drawing::Size (23, 22);
  saveButton->Text = "Сохранить";
  saveButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);
  // 
  // closeButton
  // 
  closeButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  closeButton->Image = Bitmap::FromFile ("media/images/CLOSE.BMP");
  closeButton->Image = static_cast <Bitmap^> (resources->GetObject ("CLOSE"));
  closeButton->ImageTransparentColor = System::Drawing::Color::Silver;
  closeButton->Name = "closeButton";
  closeButton->Size = Drawing::Size (23, 22);
  closeButton->Text = "Закрыть документ";
  closeButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);
  // 
  // cursorButton
  // 
  cursorButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  cursorButton->Image = Bitmap::FromFile ("media/images/CURSOR.BMP");
  cursorButton->Image = static_cast <Bitmap^> (resources->GetObject ("CURSOR"));
  cursorButton->ImageTransparentColor = System::Drawing::Color::Silver;
  cursorButton->Name = "cursorButton";
  cursorButton->Size = Drawing::Size (23, 22);
  cursorButton->Text = "Выделение объекта";
  cursorButton->Enabled = true;
  cursorButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);
  // 
  // moveButton
  // 
  moveButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  moveButton->Image = Bitmap::FromFile ("media/images/MOVE.BMP");
  moveButton->Image = static_cast <Bitmap^> (resources->GetObject ("MOVE"));
  moveButton->ImageTransparentColor = System::Drawing::Color::Silver;
  moveButton->CheckOnClick = true;
  moveButton->Name = "moveButton";
  moveButton->Size = Drawing::Size (23, 22);
  moveButton->Text = "Перемещение";
  moveButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);  
  // 
  // rotateButton
  // 
  rotateButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  rotateButton->Image = Bitmap::FromFile ("media/images/ROTATE.BMP");
  rotateButton->Image = static_cast <Bitmap^> (resources->GetObject ("ROTATE"));
  rotateButton->ImageTransparentColor = System::Drawing::Color::Silver;
  rotateButton->CheckOnClick = true;
  rotateButton->Name = "rotateButton";
  rotateButton->Size = Drawing::Size (23, 22);
  rotateButton->Text = "Вращение";
  rotateButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);  
  // 
  // scaleButton
  // 
  scaleButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  scaleButton->Image = Bitmap::FromFile ("media/images/SCALE.BMP");
  scaleButton->Image = static_cast <Bitmap^> (resources->GetObject ("SCALE"));
  scaleButton->ImageTransparentColor = System::Drawing::Color::Silver;
  scaleButton->CheckOnClick = true;
  scaleButton->Name = "scaleButton";
  scaleButton->Size = Drawing::Size (23, 22);
  scaleButton->Text = "Масштабирование";
  scaleButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);  
  // 
  // shearButton
  // 
  shearButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  shearButton->Image = Bitmap::FromFile ("media/images/SHEAR.BMP");
  shearButton->Image = static_cast <Bitmap^> (resources->GetObject ("SHEAR"));
  shearButton->ImageTransparentColor = System::Drawing::Color::Silver;
  shearButton->CheckOnClick = true;
  shearButton->Name = "shearButton";
  shearButton->Size = Drawing::Size (23, 22);
  shearButton->Text = "Shear";
  shearButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);  
  // 
  // bindButton
  // 
  bindButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  bindButton->Image = Bitmap::FromFile ("media/images/BIND.BMP");
  bindButton->Image = static_cast <Bitmap^> (resources->GetObject ("BIND"));
  bindButton->ImageTransparentColor = System::Drawing::Color::Silver;
  bindButton->CheckOnClick = true;
  bindButton->Name = "bindButton";
  bindButton->Size = Drawing::Size (23, 22);
  bindButton->Text = "bind";
  bindButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);  
  // 
  // pencilExtrudeButton
  // 
  pencilExtrudeButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
  pencilExtrudeButton->Image = static_cast <Bitmap^> (resources->GetObject ("PENCIL_EXTRUDE"));
  pencilExtrudeButton->ImageTransparentColor = System::Drawing::Color::Silver;
  pencilExtrudeButton->CheckOnClick = true;
  pencilExtrudeButton->Name = "pencilExtrudeButton";
  pencilExtrudeButton->Size = Drawing::Size (23, 22);
  pencilExtrudeButton->Text = "Pencil Extrusion";
  pencilExtrudeButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);  
  // 
  // brushExtrudeButton
  // 
  brushExtrudeButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
  brushExtrudeButton->Image = static_cast <Bitmap^> (resources->GetObject ("BRUSH_EXTRUDE"));
  brushExtrudeButton->ImageTransparentColor = System::Drawing::Color::Silver;
  brushExtrudeButton->CheckOnClick = true;
  brushExtrudeButton->Name = "brushExtrudeButton";
  brushExtrudeButton->Size = Drawing::Size (23, 22);
  brushExtrudeButton->Text = "Brush Extrusion";
  brushExtrudeButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);  
  // 
  // cameraMoveButton
  // 
  cameraMoveButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  cameraMoveButton->Image = Bitmap::FromFile ("media/images/CAM_MOV.BMP");
  cameraMoveButton->Image = static_cast <Bitmap^> (resources->GetObject ("CAM_MOV"));
  cameraMoveButton->ImageTransparentColor = System::Drawing::Color::Silver;
  cameraMoveButton->CheckOnClick = true;
  cameraMoveButton->Name = L"cameraMoveButton";
  cameraMoveButton->Text = "CamMove";
  cameraMoveButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);  
  // 
  // cameraRotateButton
  // 
  cameraRotateButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  cameraRotateButton->Image = Bitmap::FromFile ("media/images/CAM_ROT.BMP");
  cameraRotateButton->Image = static_cast <Bitmap^> (resources->GetObject ("CAM_ROT"));
  cameraRotateButton->ImageTransparentColor = System::Drawing::Color::Silver;
  cameraRotateButton->CheckOnClick = true;
  cameraRotateButton->Name = L"cameraRotateButton";
  cameraRotateButton->Text = "CamRotate";
  cameraRotateButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);  
  // 
  // cameraZoomButton
  // 
  cameraZoomButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  cameraZoomButton->Image = Bitmap::FromFile ("media/images/CAM_ZOOM.BMP");
  cameraZoomButton->Image = static_cast <Bitmap^> (resources->GetObject ("CAM_ZOOM"));
  cameraZoomButton->ImageTransparentColor = System::Drawing::Color::Silver;
  cameraZoomButton->CheckOnClick = true;
  cameraZoomButton->Name = L"cameraZoomButton";
  cameraZoomButton->Text = "CamZoom";
  cameraZoomButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);  
  // 
  // createObjStripMenu
  // 
  createObjStripMenu->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
  createObjStripMenu->DropDownItems->AddRange
  (gcnew cli::array <System::Windows::Forms::ToolStripItem^>(5) { boxToolStripMenuItem,
                                                                  sphereToolStripMenuItem,
                                                                  cylinderToolStripMenuItem,
                                                                  coneToolStripMenuItem,
                                                                  torusToolStripMenuItem });
  createObjStripMenu->Name = L"createObjStripMenu";
  createObjStripMenu->Size = System::Drawing::Size(32, 22);
  createObjStripMenu->Text = L"Создать";
  // 
  // boxToolStripMenuItem
  // 
  boxToolStripMenuItem->Name = L"boxToolStripMenuItem";
  boxToolStripMenuItem->Size = System::Drawing::Size(152, 22);
  boxToolStripMenuItem->Text = L"Параллелепипед";
  boxToolStripMenuItem->Click += gcnew System::EventHandler(this, &EditorForm::boxToolStripMenuItem_Click);
  // 
  // sphereToolStripMenuItem
  // 
  sphereToolStripMenuItem->Name = L"sphereToolStripMenuItem";
  sphereToolStripMenuItem->Size = System::Drawing::Size(152, 22);
  sphereToolStripMenuItem->Text = L"Сфера";
  sphereToolStripMenuItem->Click += gcnew System::EventHandler(this, &EditorForm::sphereToolStripMenuItem_Click);
  // 
  // cylinderToolStripMenuItem
  // 
  cylinderToolStripMenuItem->Name = L"cylinderToolStripMenuItem";
  cylinderToolStripMenuItem->Size = System::Drawing::Size(152, 22);
  cylinderToolStripMenuItem->Text = L"Цилиндр";
  cylinderToolStripMenuItem->Click += gcnew System::EventHandler(this, &EditorForm::cylinderToolStripMenuItem_Click);
  // 
  // coneToolStripMenuItem
  // 
  coneToolStripMenuItem->Name = L"coneToolStripMenuItem";
  coneToolStripMenuItem->Size = System::Drawing::Size(152, 22);
  coneToolStripMenuItem->Text = L"Конус";
  coneToolStripMenuItem->Click += gcnew System::EventHandler(this, &EditorForm::coneToolStripMenuItem_Click);
  // 
  // torusToolStripMenuItem
  // 
  torusToolStripMenuItem->Name = L"torusToolStripMenuItem";
  torusToolStripMenuItem->Size = System::Drawing::Size(152, 22);
  torusToolStripMenuItem->Text = L"Тор";
  torusToolStripMenuItem->Click += gcnew System::EventHandler (this, &EditorForm::torusToolStripMenuItem_Click);
  // 
  // helpButton
  // 
  helpButton->DisplayStyle = Windows::Forms::ToolStripItemDisplayStyle::Image;
//  helpButton->Image = Bitmap::FromFile ("media/images/HELP.BMP");
  helpButton->Image = static_cast <Bitmap^> (resources->GetObject ("HELP"));
  helpButton->ImageTransparentColor = Drawing::Color::Magenta;
  helpButton->Name = "helpButton";
  helpButton->Size = Drawing::Size (23, 22);
  helpButton->Text = "Помощь";
  helpButton->Click += gcnew System::EventHandler (this, &EditorForm::ToolbarHandler);

  toolStrip->ResumeLayout (false);
  toolStrip->PerformLayout ();
}

// Создание компонентов главного окна редактора
void EditorForm::InitializeComponent (void)
{
  this->components = gcnew System::ComponentModel::Container();

  Assembly^ assembly = Assembly::GetAssembly (this->GetType ());
  resources = gcnew ResourceManager ("gui", assembly);

  CreateMenu ();
  CreateToolbar ();

  statusStrip = gcnew Windows::Forms::StatusStrip ();
  statusStrip->SuspendLayout ();
  statusText   = gcnew Windows::Forms::ToolStripStatusLabel ();
  dockPanel    = gcnew WeifenLuo::WinFormsUI::DockPanel ();
  outputForm   = gcnew OutputForm ();
  propertyForm = gcnew PropertyForm ();
  wxfForm      = gcnew WXFForm ();
  helpProvider = gcnew System::Windows::Forms::HelpProvider;

  helpProvider->HelpNamespace = "coolwork.chm";
  helpProvider->SetShowHelp (this, true);

  propertyForm->Show (dockPanel);
  outputForm->Show (dockPanel);
  wxfForm->Show (dockPanel);

  SuspendLayout();

  // 
  // statusStrip
  // 
  array<Windows::Forms::ToolStripItem^>^ statusItems = gcnew array<Windows::Forms::ToolStripItem^> (1);
  statusItems [0] = this->statusText;
  statusStrip->Items->AddRange (statusItems);
  statusStrip->Location = System::Drawing::Point (0, 458);
  statusStrip->Name = "statusStrip";
  statusStrip->Size = System::Drawing::Size (655, 22);
  statusStrip->TabIndex = 4;
  statusStrip->Text = "Status";
  // 
  // statusText
  // 
  statusText->Name = "statusText";
  statusText->Size = System::Drawing::Size (38, 17);
  statusText->Text = "Ready";
  // 
  // dockPanel
  // 
  dockPanel->ActiveAutoHideContent = nullptr;
  dockPanel->Dock = System::Windows::Forms::DockStyle::Fill;
  dockPanel->Font = gcnew System::Drawing::Font ("Tahoma", 11.0f, System::Drawing::FontStyle::Regular,
                                                                        System::Drawing::GraphicsUnit::World);
  dockPanel->Location = System::Drawing::Point (0, 40);
  dockPanel->Name = "dockPanel";
  dockPanel->Size = System::Drawing::Size (912, 530);
  dockPanel->TabIndex = 8;

  // 
  // EditorForm
  // 
  Text = L"My3D";
  Name= "EditorForm";
  Padding = System::Windows::Forms::Padding (0);
  Size = System::Drawing::Size (800,600);
  MinimumSize = System::Drawing::Size (640, 480);
  StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
  AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
  AutoScaleDimensions = System::Drawing::SizeF (6.0f, 13.0f);
  IsMdiContainer = true;
  WindowState = System::Windows::Forms::FormWindowState::Maximized;
  Menu = mainMenu;

  Controls->Add (this->dockPanel);
  Controls->Add (this->toolStrip);
  Controls->Add (this->statusStrip);

  Shown     += gcnew System::EventHandler (this, &EditorForm::EditorForm_Shown);
  Closing   += gcnew System::ComponentModel::CancelEventHandler (this, &EditorForm::EditorForm_Closing);
  Activated += gcnew System::EventHandler (this, &EditorForm::EditorForm_GotFocus);

  statusStrip->ResumeLayout (false);
  statusStrip->PerformLayout ();

  ResumeLayout (false);
  PerformLayout ();
}

#pragma endregion

}