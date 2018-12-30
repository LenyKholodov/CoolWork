#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Data.dll>
#using <System.Windows.Forms.dll>
#using <WeifenLuo.WinFormsUI.Docking.dll>

namespace My3D
{
  ref class OutputForm;
  ref class WXFForm;
  ref class PropertyForm;
  ref class RenderForm;
  ref class Document;

  public ref class EditorForm : public System::Windows::Forms::Form
  {
    public:
      EditorForm ();

    protected:
      ~EditorForm ();

    private: 
      void InitializeComponent ();
      void CreateMenu ();
      void CreateToolbar ();
      void ActivateDocumentMode (bool enable);
      void MenuItemHandler (System::Object^ sender, System::EventArgs^ e);
      void ToolbarHandler (System::Object^ sender, System::EventArgs^ e);

      void EditorForm_GotFocus (System::Object^, System::EventArgs^);
      void EditorForm_Shown    (System::Object^, System::EventArgs^);
      void EditorForm_Closing  (System::Object^, System::ComponentModel::CancelEventArgs^);

      void SetToolButtonsState (int cursor, int move, int rotate, int scale, int shear, int bind, int pencil_extrude, int brush_extrude,
                                int cam_zoom, int cam_move, int cam_rotate);

      void boxToolStripMenuItem_Click (System::Object^ sender, System::EventArgs^ e);
      void sphereToolStripMenuItem_Click (System::Object^ sender, System::EventArgs^ e);
      void cylinderToolStripMenuItem_Click (System::Object^ sender, System::EventArgs^ e);
      void coneToolStripMenuItem_Click (System::Object^ sender, System::EventArgs^ e);
      void torusToolStripMenuItem_Click (System::Object^ sender, System::EventArgs^ e);

    public:
      void About ();
      void HelpTopics ();
      void Exit ();
      void ClearAll ();

      void NewDocument    ();
      void OpenDocument   ();
      void SaveDocument   ();
      void SaveAsDocument ();
      void CloseDocument  ();

      void EnableRefresh ();
      void DisableRefresh ();

      void ActivateCursor ();
      void ManipulatorChanged (System::String^ manipulator_name);

      void ShowRender ();
      void CloseRender ();
      void LogMessage (const char* msg);
      void LogMessage (System::String^ msg);

    private:
      void CreateDoc ();
      void CreateShape (const char* create_cmd);

    private:
      enum class EditorState {
        DocumentProcessing,
        Idle
      };

    private:
      System::ComponentModel::Container ^components;

      System::Windows::Forms::MainMenu^ mainMenu;
      System::Windows::Forms::MenuItem^ fileMenuItem;
      System::Windows::Forms::MenuItem^ editMenuItem;


      System::Windows::Forms::MenuItem^ toolsMenuItem;

      System::Windows::Forms::MenuItem^ primitiveMenuItem;
      System::Windows::Forms::MenuItem^ boxMenuItem;
      System::Windows::Forms::MenuItem^ sphereMenuItem;
      System::Windows::Forms::MenuItem^ cylinderMenuItem;
      System::Windows::Forms::MenuItem^ coneMenuItem;
      System::Windows::Forms::MenuItem^ torusMenuItem;
      System::Windows::Forms::MenuItem^ landscapeMenuItem;
      System::Windows::Forms::MenuItem^ halfsphereMenuItem;
      System::Windows::Forms::MenuItem^ halfcylinderMenuItem;
      System::Windows::Forms::MenuItem^ halfconeMenuItem;
      System::Windows::Forms::MenuItem^ halftorusMenuItem;

      System::Windows::Forms::MenuItem^ platonicsMenuItem;
      System::Windows::Forms::MenuItem^ dodecahedronMenuItem;
      System::Windows::Forms::MenuItem^ icosahedronMenuItem;
      System::Windows::Forms::MenuItem^ octahedronMenuItem;
      System::Windows::Forms::MenuItem^ tetrahedronMenuItem;
      System::Windows::Forms::MenuItem^ hexahedronMenuItem;

      System::Windows::Forms::MenuItem^ shapesMenuItem;
      System::Windows::Forms::MenuItem^ spaceshipMenuItem;
      System::Windows::Forms::MenuItem^ headphonesMenuItem;
      System::Windows::Forms::MenuItem^ craneMenuItem;
      System::Windows::Forms::MenuItem^ fdcMenuItem;
      System::Windows::Forms::MenuItem^ tankMenuItem;

      System::Windows::Forms::MenuItem^ lightMenuItem;
      System::Windows::Forms::MenuItem^ pointLightMenuItem;
      System::Windows::Forms::MenuItem^ spotFreeLightMenuItem;
      System::Windows::Forms::MenuItem^ spotTargetLightMenuItem;
      System::Windows::Forms::MenuItem^ directFreeLightMenuItem;
      System::Windows::Forms::MenuItem^ directTargetLightMenuItem;

      System::Windows::Forms::MenuItem^ manipulatorMenuItem;
      System::Windows::Forms::MenuItem^ accPlusBrushMenuItem;
      System::Windows::Forms::MenuItem^ accMinusBrushMenuItem;
      System::Windows::Forms::MenuItem^ accPlusPencilMenuItem;
      System::Windows::Forms::MenuItem^ accMinusPencilMenuItem;
      System::Windows::Forms::MenuItem^ noaccMinusBrushMenuItem;
      System::Windows::Forms::MenuItem^ noaccMinusPencilMenuItem;

      System::Windows::Forms::MenuItem^ viewMenuItem;
      System::Windows::Forms::MenuItem^ windowMenuItem;
      System::Windows::Forms::MenuItem^ helpMenuItem;
      System::Windows::Forms::MenuItem^ newMenuItem;
      System::Windows::Forms::MenuItem^ openMenuItem;
      System::Windows::Forms::MenuItem^ closeMenuItem;
      System::Windows::Forms::MenuItem^ saveMenuItem;
      System::Windows::Forms::MenuItem^ saveAsMenuItem;
      System::Windows::Forms::MenuItem^ exitMenuItem;
      System::Windows::Forms::MenuItem^ testMenuItem;
      System::Windows::Forms::MenuItem^ removeDepsMenuItem;
      System::Windows::Forms::MenuItem^ clearAllMenuItem;
      System::Windows::Forms::MenuItem^ toolbarMenuItem;
      System::Windows::Forms::MenuItem^ statusbarMenuItem;
      System::Windows::Forms::MenuItem^ propertyMenuItem;
      System::Windows::Forms::MenuItem^ outputMenuItem;
      System::Windows::Forms::MenuItem^ renderMenuItem;
      System::Windows::Forms::MenuItem^ WXFMenuItem;
      System::Windows::Forms::MenuItem^ helpAboutMenuItem;
      System::Windows::Forms::MenuItem^ helpTopicsMenuItem;

      System::Windows::Forms::ToolStrip^          toolStrip;
      System::Windows::Forms::ToolStripButton^    newButton;
      System::Windows::Forms::ToolStripButton^    openButton;
      System::Windows::Forms::ToolStripButton^    saveButton;
      System::Windows::Forms::ToolStripButton^    closeButton;

      System::Windows::Forms::ToolStripButton^    cursorButton;
      System::Windows::Forms::ToolStripButton^    moveButton;
      System::Windows::Forms::ToolStripButton^    rotateButton;
      System::Windows::Forms::ToolStripButton^    scaleButton;
      System::Windows::Forms::ToolStripButton^    shearButton;
      System::Windows::Forms::ToolStripButton^    bindButton;
      System::Windows::Forms::ToolStripButton^    pencilExtrudeButton;
      System::Windows::Forms::ToolStripButton^    brushExtrudeButton;

      System::Windows::Forms::ToolStripButton^    cameraZoomButton;
      System::Windows::Forms::ToolStripButton^    cameraRotateButton;
      System::Windows::Forms::ToolStripButton^    cameraMoveButton;

      System::Windows::Forms::ToolStripSplitButton^  createObjStripMenu;
      System::Windows::Forms::ToolStripMenuItem^     boxToolStripMenuItem;
      System::Windows::Forms::ToolStripMenuItem^     sphereToolStripMenuItem;
      System::Windows::Forms::ToolStripMenuItem^     cylinderToolStripMenuItem;
      System::Windows::Forms::ToolStripMenuItem^     coneToolStripMenuItem;
      System::Windows::Forms::ToolStripMenuItem^     torusToolStripMenuItem;

      System::Windows::Forms::ToolStripButton^       helpButton;
 
      System::Windows::Forms::StatusStrip^          statusStrip;
      System::Windows::Forms::ToolStripStatusLabel^ statusText;

      WeifenLuo::WinFormsUI::DockPanel^  dockPanel;

      System::Windows::Forms::HelpProvider^ helpProvider;

      WXFForm^       wxfForm;
      OutputForm^    outputForm;
      RenderForm^    renderForm;

    public:

      System::Resources::ResourceManager^ resources;

      EditorState   state;
      Document^     curDoc;
      PropertyForm^ propertyForm;

    public:

      property System::Object^ ActiveEntity
      {
        System::Object^ get ();
        void            set (System::Object^);
      };

      property PropertyForm^ PropertyPanel
      {
        PropertyForm^ get ();
      };

      property WXFForm^ WXF
      {
        WXFForm^ get ();
      };

      property System::Resources::ResourceManager^ Resources
      {
        System::Resources::ResourceManager^ get ();
      };

  };
}

