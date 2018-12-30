#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
#using <WeifenLuo.WinFormsUI.Docking.dll>
#using <ToolBox.dll>

#include <core.h>

namespace My3D
{
  ref class EditorForm;
  ref class Entity;
  ref class EntityTreeNode;

	public ref class PropertyForm : public WeifenLuo::WinFormsUI::DockContent
	{
    typedef System::Collections::Generic::Dictionary <System::String^, System::String^> EntityMap;

    public :
      PropertyForm ();

      void AddEntity (Entity^);
      void DelEntity (Entity^);
      void EnableToolBox ();
      void DisableToolBox ();

      void UpdateEntityName (Entity^ entity);
      void Update ();
      
      EntityTreeNode^ FindSceneNode (Entity^);

    protected :
      ~PropertyForm ();

    private:
      void CurrentEntityChanged (System::Object^ sender, System::EventArgs^ e);
      void PropertyValueChanged (System::Object^ sender, System::Windows::Forms::PropertyValueChangedEventArgs^ e);
      void ToolBox_ItemSelectionChanged (Silver::UI::ToolBoxItem^ sender, System::EventArgs^ e);
      void ToolBox_TabSelectionChanged  (Silver::UI::ToolBoxTab^ sender, System::EventArgs^ e);

      void NodeMouseClick (System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e);
      void NodeMouseDoubleClick (System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e);


      void InitializeComponent ();
      void InitializeEntityMap ();
      void CreateToolBox ();

    private:
      System::ComponentModel::Container ^components;

      System::Windows::Forms::TabControl^        tabControl;
      System::Windows::Forms::TabPage^           propertiesPage;
      System::Windows::Forms::TabPage^           objectsPage;
      System::Windows::Forms::TabPage^           scenePage;
      System::Windows::Forms::TreeView^          scene_tree;
      System::Windows::Forms::ComboBox^          entityList;
      System::Windows::Forms::PropertyGrid^      propertyGrid;

      Silver::UI::ToolBox^ toolBox;

      EntityMap  entity_map;

      bool refresh_enabled;

    public:

      property bool RefreshEnabled
      {
        bool get ();
        void set (bool);
      }

      property System::Object^ ActiveEntity
      {
        System::Object^ get ();
        void            set (System::Object^);
      };

      property array <System::Object^>^ SelectedEntities
      {
        array <System::Object^>^ get ();
        void set (array <System::Object^>^);
      }

      property System::Windows::Forms::PropertyGrid^ EntityProperties
      {
        System::Windows::Forms::PropertyGrid^ get ();
      };

      property System::Windows::Forms::TreeView^ SceneTree
      {
        System::Windows::Forms::TreeView^ get ();
      };
  };
}

