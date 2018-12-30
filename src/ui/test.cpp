#using <mscorlib.dll>
#using <System.dll>
#using <System.Windows.Forms.dll>

#include <windows.h>

#include <stdio.h>
#include "GUI.h"

using namespace System;
using namespace System::Windows::Forms;

namespace My3D {

static void OnEntityCreate (CoreEntityHandle handle)
{
//  GUI::LogMessage (String::Format ("GUI::OnCreateEntity: '{0}'", gcnew String (CoreGetEntityParams (handle,CORE_ENTITY_NAME))));

  if (!GUI::IsValidScene ())
    return;

  GUI::ActiveScene->OnCreateEntity (handle);
}

static void OnEntityDelete (CoreEntityHandle handle)
{
//  GUI::LogMessage (String::Format ("GUI::OnDeleteEntity: '{0}'", gcnew String (CoreGetEntityParams (handle,CORE_ENTITY_NAME))));

  if (!GUI::IsValidScene ())
    return;
    
  GUI::ActiveScene->OnDeleteEntity (handle);
}

static void OnEntityModify (CoreEntityHandle handle)
{
//  GUI::LogMessage (String::Format ("GUI::OnModifyEntity: '{0}'", gcnew String (CoreGetEntityParams (handle,CORE_ENTITY_NAME))));

  if (!GUI::IsValidScene ())
    return;

  GUI::ActiveScene->OnModifyEntity (handle);
}

static void OnSelect ()
{
  if (!GUI::IsValidScene ())
    return;

//  GUI::LogMessage (String::Format ("GUI::Select: {0} entities", CoreGetSelectedEntitiesCount ()));

  GUI::ActiveScene->OnSelect ();
}

static void ManipulatorChanged (const char* manipulator_name)
{
  if (!GUI::IsValidScene ())
    return;

  GUI::Editor->ManipulatorChanged (gcnew String (manipulator_name));
}

void OnSceneCreate ()
{
  CoreSceneListener listener;
  
  listener.onCreate = OnEntityCreate;
  listener.onDelete = OnEntityDelete;
  listener.onModify = OnEntityModify;
  listener.onSelect = OnSelect;

  CoreSetSceneListener (&listener);
}

void OnSceneClose ()
{
  CoreSetSceneListener (0);
}

void LogMessage (const char * msg)
{
  GUI::LogMessage (msg);
}

}

[STAThreadAttribute]
int main (/*int argc, char* argv[]*/array<System::String ^> ^args)
{
//  MessageBox (NULL,"Heye","Test",MB_OK);

  Console::WriteLine ("Starting at: '{0}'", Application::ExecutablePath);

  CoreLogCallback (My3D::LogMessage);
  CoreSetManipulatorCallback (My3D::ManipulatorChanged);

  CoreInit ();
   
  Application::Run (My3D::GUI::Editor);

  CoreShutdown ();

  return 0;
}
