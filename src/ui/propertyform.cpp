#include "editorform.h"
#include "gui_scene.h"
#include "document.h"
#include "propertyform.h"
#include "GUI.h"
#include "utils.h"

using namespace System;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace WeifenLuo::WinFormsUI;
using namespace Silver::UI;

namespace My3D {

public ref class EntityTreeNode : public System::Windows::Forms::TreeNode
{
  public:
    EntityTreeNode (Entity^ _entity)
    {
      entity = _entity;
      Name = entity->Name;
      Text = entity->Name;
    }

    CoreEntityHandle GetHandle ()
    {
      return entity->GetHandle ();
    }

    Entity^ entity;
};

/*
===============================================================================

  Свойства панели

===============================================================================
*/

Object^ PropertyForm::ActiveEntity::get ()
{
  return propertyGrid->SelectedObject;
}

void PropertyForm::ActiveEntity::set (Object^ value)
{
  Entity^ active_entity = dynamic_cast <Entity^> (value);

  if (value == nullptr)
    entityList->SelectedIndex = -1;
  else
    entityList->SelectedItem = value;

  propertyGrid->SelectedObject = value;
}

PropertyGrid^ PropertyForm::EntityProperties::get ()
{
  return propertyGrid;
}

array<Object^>^ PropertyForm::SelectedEntities::get ()
{
  return propertyGrid->SelectedObjects;
}

void PropertyForm::SelectedEntities::set (array<Object^>^ value)
{
  entityList->SelectedIndex = -1;
  propertyGrid->SelectedObjects = value;
}

TreeView^ PropertyForm::SceneTree::get ()
{
  return scene_tree;
};

bool PropertyForm::RefreshEnabled::get ()
{
  return refresh_enabled;
}

void PropertyForm::RefreshEnabled::set (bool value)
{
  refresh_enabled = value;
}

/*
===============================================================================

  Конструктор / деструктор

===============================================================================
*/

PropertyForm::PropertyForm ()
{
  InitializeEntityMap ();
  InitializeComponent ();
}

PropertyForm::~PropertyForm ()
{
  if (components)
  {
    delete components;
  }
}

/*
===============================================================================

  Создание и установка свойств компонентов интерфеса

===============================================================================
*/

#pragma region Windows Form Designer generated code

/*
===============================================================================

  ToolBox

===============================================================================
*/

void PropertyForm::InitializeEntityMap ()
{
  entity_map.Add ("Указатель",      "select");
  entity_map.Add ("Параллелепипед", "create box");
  entity_map.Add ("Сфера",          "create sphere");
  entity_map.Add ("Цилиндр",        "create cylinder");
  entity_map.Add ("Конус",          "create cone");
  entity_map.Add ("Тор",            "create torus");
  entity_map.Add ("Полусфера",      "create halfsphere");
  entity_map.Add ("Полуцилиндр",    "create halfcylinder");
  entity_map.Add ("Полуконус",      "create halfcone");
  entity_map.Add ("Полутор",        "create halftorus");
  entity_map.Add ("Тетраэдр",       "create tetrahedron");
  entity_map.Add ("Гексаэдр",       "create hexahedron");
  entity_map.Add ("Октаэдр",        "create octahedron");
  entity_map.Add ("Додекаэдр",      "create dodecahedron");
  entity_map.Add ("Икосаэдр",       "create icosahedron");

  entity_map.Add ("Point",         "create point");
  entity_map.Add ("Spot free",     "create spot_free");
  entity_map.Add ("Spot target",   "create spot_target");
  entity_map.Add ("Direct free",   "create direct_free");
  entity_map.Add ("Direct target", "create direct_target");
}

void PropertyForm::ToolBox_TabSelectionChanged (ToolBoxTab^ sender, EventArgs^ e)
{
  sender->SelectedItemIndex = 0;
}

void PropertyForm::EnableToolBox ()
{
  toolBox->Enabled = true;
}

void PropertyForm::DisableToolBox ()
{
  toolBox->Enabled = false;
}

// Создание панели создания объектов
void PropertyForm::CreateToolBox ()
{
  toolBox = gcnew ToolBox ();
  toolBox->SuspendLayout ();
  // 
  // toolBox
  // 
  toolBox->Name     = "toolBox";
  toolBox->Size     = Drawing::Size (208, 405);
  toolBox->Location = Drawing::Point (0, 0);

  Image^ image_list = nullptr;

  try {
    image_list = gcnew Drawing::Bitmap ("media/images/TOOLBOX.BMP");
  }
  catch (Exception^ ex)
  {
    Console::WriteLine (ex->ToString ());
  }

  toolBox->SetImageList (image_list, Drawing::Size (24,24), Color::FromArgb (185,185,185), false);

  toolBox->BackColor   = Drawing::SystemColors::Control;
  toolBox->Dock        = Windows::Forms::DockStyle::Fill;
  toolBox->Font        = gcnew Drawing::Font ("Verdana", 8.0f, FontStyle::Regular, GraphicsUnit::Point, 0);
  toolBox->TabHeight   = 25;
  toolBox->ItemHeight  = 20;
  toolBox->ItemSpacing = 1;

  toolBox->ItemHoverColor    = Drawing::Color::BurlyWood;
  toolBox->ItemNormalColor   = Drawing::SystemColors::Control;
  toolBox->ItemSelectedColor = Drawing::Color::Linen;

  toolBox->ItemSelectionChanged += gcnew ItemSelectionChangedHandler (this,&PropertyForm::ToolBox_ItemSelectionChanged);
  toolBox->TabSelectionChanged += gcnew TabSelectionChangedHandler (this,&PropertyForm::ToolBox_TabSelectionChanged);

  toolBox->AddTab ("Геометрические примтивы",-1);
  toolBox->AddTab ("Источники света",-1);

  toolBox [0]->View = ViewMode::LargeIcons;

  toolBox [0]->AddItem ("Указатель",0,0,false,nullptr);
  toolBox [0]->AddItem ("Параллелепипед",1,1,true,1);
  toolBox [0]->AddItem ("Сфера",2,2,true,2);
  toolBox [0]->AddItem ("Цилиндр",3,3,true,3);
  toolBox [0]->AddItem ("Тор",4,4,true,4);
  toolBox [0]->AddItem ("Конус",5,5,true,5);
  toolBox [0]->AddItem ("Полусфера");
  toolBox [0]->AddItem ("Полуцилиндр");
  toolBox [0]->AddItem ("Полуконус");
  toolBox [0]->AddItem ("Полутор");
  toolBox [0]->AddItem ("Тетраэдр");
  toolBox [0]->AddItem ("Гексаэдр");
  toolBox [0]->AddItem ("Октаэдр");
  toolBox [0]->AddItem ("Додекаэдр");
  toolBox [0]->AddItem ("Икосаэдр");

  toolBox [1]->View = ViewMode::LargeIcons;

  toolBox [1]->AddItem ("Указатель",0,0,false,nullptr);
  toolBox [1]->AddItem ("Point");
  toolBox [1]->AddItem ("Spot free");
  toolBox [1]->AddItem ("Spot target");
  toolBox [1]->AddItem ("Direct free");
  toolBox [1]->AddItem ("Direct target");

  toolBox [0]->Selected = true;
  toolBox [0]->SelectedItemIndex = 0;
}

// Создание компонентов панели
void PropertyForm::InitializeComponent ()
{
  this->propertiesPage = gcnew Windows::Forms::TabPage ();
  this->objectsPage    = gcnew Windows::Forms::TabPage ();
  this->scenePage      = gcnew Windows::Forms::TabPage ();
  this->scene_tree     = gcnew Windows::Forms::TreeView ();
  this->entityList     = gcnew Windows::Forms::ComboBox ();
  this->propertyGrid   = gcnew Windows::Forms::PropertyGrid ();
  this->tabControl     = gcnew Windows::Forms::TabControl ();

  CreateToolBox ();

  this->tabControl->SuspendLayout ();
  this->propertiesPage->SuspendLayout ();
  this->SuspendLayout();
  // 
  // tabControl
  // 
  this->tabControl->Controls->Add (this->propertiesPage);
  this->tabControl->Controls->Add (this->objectsPage);
  this->tabControl->Controls->Add (this->scenePage);
  this->tabControl->Dock = System::Windows::Forms::DockStyle::Fill;
  this->tabControl->Location = System::Drawing::Point (513, 24);
  this->tabControl->Name = "tabControl";
  this->tabControl->SelectedIndex = 0;
  this->tabControl->Size = System::Drawing::Size (198, 444);
  this->tabControl->TabIndex = 0;
  // 
  // propertiesPage
  // 
  this->propertiesPage->Controls->Add (this->propertyGrid);
  this->propertiesPage->Controls->Add (this->entityList);
  this->propertiesPage->Location = System::Drawing::Point (4, 22);
  this->propertiesPage->Name = "propertiesPage";
  this->propertiesPage->Padding = System::Windows::Forms::Padding (3);
  this->propertiesPage->Size = System::Drawing::Size (190, 418);
  this->propertiesPage->TabIndex = 1;
  this->propertiesPage->Text = "Свойства";
  this->propertiesPage->UseVisualStyleBackColor = true;
  // 
  // objectsPage
  // 
  this->objectsPage->Controls->Add (toolBox);
  this->objectsPage->Location = System::Drawing::Point (4, 22);
  this->objectsPage->Name = "objectsPage";
  this->objectsPage->Padding = System::Windows::Forms::Padding (3);
  this->objectsPage->Size = System::Drawing::Size (193, 418);
  this->objectsPage->TabIndex = 2;
  this->objectsPage->Text = "Объекты";
  this->objectsPage->UseVisualStyleBackColor = true;
  // 
  // scenePage
  // 
  this->scenePage->Controls->Add (scene_tree);
  this->scenePage->Location = System::Drawing::Point (4, 22);
  this->scenePage->Name = "scenePage";
  this->scenePage->Padding = System::Windows::Forms::Padding (3);
  this->scenePage->Size = System::Drawing::Size (193, 418);
  this->scenePage->TabIndex = 3;
  this->scenePage->Text = "Сцена";
  this->scenePage->UseVisualStyleBackColor = true;
  // 
  // scene_tree
  // 
  this->scene_tree->Dock = System::Windows::Forms::DockStyle::Fill;
	this->scene_tree->Name = L"scene_tree";
  this->scene_tree->NodeMouseClick += gcnew Windows::Forms::TreeNodeMouseClickEventHandler (this, &PropertyForm::NodeMouseClick);
  this->scene_tree->NodeMouseDoubleClick += gcnew Windows::Forms::TreeNodeMouseClickEventHandler (this, &PropertyForm::NodeMouseDoubleClick);
  // 
  // entityList
  // 
  this->entityList->Dock = System::Windows::Forms::DockStyle::Top;
  this->entityList->FormattingEnabled = true;
  this->entityList->Location = System::Drawing::Point (3, 3);
  this->entityList->Name = "entityList";
  this->entityList->Size = System::Drawing::Size (184, 21);
  this->entityList->Sorted = true;
  this->entityList->TabIndex = 3;
  this->entityList->SelectedIndexChanged += gcnew System::EventHandler (this, &PropertyForm::CurrentEntityChanged);
  // 
  // propertyGrid
  // 
  this->propertyGrid->Dock = System::Windows::Forms::DockStyle::Fill;
  this->propertyGrid->Location = System::Drawing::Point (3, 24);
  this->propertyGrid->Name = "propertyGrid";
  this->propertyGrid->Size = System::Drawing::Size (184, 391);
  this->propertyGrid->TabIndex = 4;
  this->propertyGrid->PropertyValueChanged += gcnew Windows::Forms::PropertyValueChangedEventHandler (this, &PropertyForm::PropertyValueChanged);
  // 
  // PropertyForm
  // 
  this->AutoScaleBaseSize = System::Drawing::Size (6, 14);
  this->ClientSize = System::Drawing::Size (360, 373);
  this->Controls->Add (tabControl);
  this->Font = gcnew System::Drawing::Font ("Verdana", 8.25F, System::Drawing::FontStyle::Regular,
                                                              System::Drawing::GraphicsUnit::Point, ((System::Byte)(0)));
  this->Name = "PropertyForm";
  this->ShowHint = WeifenLuo::WinFormsUI::DockState::DockRight/*AutoHide*/;
  this->TabText = "Менеджер";
  this->Text = "Менеджер";
  this->HideOnClose = true;
  
  this->propertiesPage->ResumeLayout (false);
  this->propertiesPage->PerformLayout ();
  this->tabControl->ResumeLayout (false);
  this->tabControl->PerformLayout ();
  this->ResumeLayout (false);
}

#pragma endregion

/*
===============================================================================

  Helpers

===============================================================================
*/

void PropertyForm::NodeMouseClick (System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
{
  EntityTreeNode^ node = safe_cast <EntityTreeNode^> (e->Node);

  CoreEntityXSelect (node->GetHandle ());
}

void PropertyForm::NodeMouseDoubleClick (System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
{
  EntityTreeNode^ node = safe_cast <EntityTreeNode^> (e->Node);

  CoreEntityXSelect (node->GetHandle ());

  tabControl->SelectedIndex = 0;
}

EntityTreeNode^ PropertyForm::FindSceneNode (Entity^ entity)
{
  array<TreeNode^>^ result = scene_tree->Nodes->Find (entity->Name, true);

  if (result != nullptr)
  {
    if (result->Length == 1)
      return safe_cast <EntityTreeNode^> (result [0]);
    else
      GUI::LogMessage ("Parent collision.");
  }

  return nullptr;
}

void PropertyForm::AddEntity (Entity^ entity)
{
  entityList->Items->Add (entity);

  if (refresh_enabled)
    ActiveEntity = entity;

  EntityTreeNode^ new_node = gcnew EntityTreeNode (entity);
  Entity^ parent;

  if ( !GUI::ActiveScene->Entities->TryGetValue (CoreGetParentEntity (entity->GetHandle ()), parent))
  {
    scene_tree->Nodes->Add (new_node);
  }
  else
  {
    EntityTreeNode^ parent_node = FindSceneNode (parent);
    if (parent_node != nullptr)
      parent_node->Nodes->Add (new_node);
  }
}

void PropertyForm::DelEntity (Entity^ entity)
{
  if ( !entityList->Items->Contains (entity) )
    return;

  entityList->Items->Remove (entity);

  ActiveEntity = nullptr;

  EntityTreeNode^ node = FindSceneNode (entity);

  if (node != nullptr)
    scene_tree->Nodes->Remove (node);
}

void PropertyForm::CurrentEntityChanged (System::Object^ sender, System::EventArgs^ e)
{
  if (entityList->SelectedIndex == -1)
  {
    ActiveEntity = nullptr;
  }
  else
  {
    Entity^ entity = dynamic_cast <Entity^> (entityList->SelectedItem);

    ActiveEntity = entity;

    CoreEntityXSelect (entity->GetHandle ());

//    GUI::LogMessage ("Property Control select entity: '" + entity->Name + "'");
  }
}

// Обновление параметров после их изменения на стороне рендера
void PropertyForm::Update ()
{
}

void PropertyForm::UpdateEntityName (Entity^ entity)
{
  if (entity == nullptr)
    return;

  entityList->Items->Remove (entity);
  entityList->Items->Add (entity);
  entityList->SelectedItem = entity;
}

void PropertyForm::PropertyValueChanged (System::Object^ s, System::Windows::Forms::PropertyValueChangedEventArgs^ e)
{
  if (e->ChangedItem->Label == "ProjectionType")
  {
    CoreEntityHandle handle = ( dynamic_cast <Entity^> (entityList->SelectedItem) )->GetHandle ();
    GUI::ActiveScene->OnDeleteEntity (handle);
    GUI::ActiveScene->OnCreateEntity (handle);
  }
}

void PropertyForm::ToolBox_ItemSelectionChanged (ToolBoxItem^ sender, EventArgs^ e)
{
  AutoStr cmd (entity_map [sender->Caption]);
  GUI::LogMessage (entity_map [sender->Caption]);

  if (CoreIsRenderInit ()) 
    CoreDoCommand (cmd.c_str ());
}

}