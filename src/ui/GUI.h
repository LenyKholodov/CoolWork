#pragma once

#using <mscorlib.dll>
#using <System.dll>

#include <core.h>
#include "document.h"
#include "editorform.h"
#include "propertyform.h"
#include "wxfform.h"

namespace My3D {

public ref class GUI
{
  private: 
    static EditorForm^ editor = gcnew My3D::EditorForm ();

  public:
    static bool IsValidScene ()  { return editor->curDoc != nullptr; }

    static void LogMessage (System::String^ msg)
    {
      editor->LogMessage (msg);
    }

    static void LogMessage (const char* msg)
    {
      editor->LogMessage (msg);
    }

    static property EditorForm^ Editor
    {
      EditorForm^ get () { return editor; }
    }

    static property Scene^ ActiveScene
    {
      Scene^ get () { return %editor->curDoc->scene; }
    }

    static property PropertyForm^ PropertyControl
    {
      PropertyForm^ get () { return editor->propertyForm; }
    }
};

}