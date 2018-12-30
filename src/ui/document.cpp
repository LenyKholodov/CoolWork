#include "editorform.h"
#include "document.h"
#include "GUI.h"
#include "utils.h"

#include <core.h>

using namespace System;

namespace My3D {

Document::Document ()
{
}

Document::~Document ()
{
}

void Document::DeleteContents ()
{
  GUI::PropertyControl->SceneTree->Nodes->Clear ();
  CoreDoCommand ("reset");
  NeedSave = true;
}

void Document::Save ()
{
  AutoStr save_cmd ("save");

  CoreDoCommand (save_cmd.c_str ());

  NeedSave = false;
}

void Document::SaveAs ()
{
  AutoStr save_cmd ("saveas");

  CoreDoCommand (save_cmd.c_str ());

  NeedSave = false;
}

void Document::Load ()
{
  AutoStr load_cmd ("load");

  CoreDoCommand (load_cmd.c_str ());

  NeedSave = false;
}

void Document::Close ()
{
}

}
