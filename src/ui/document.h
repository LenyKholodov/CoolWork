#pragma once

#using <mscorlib.dll>
#using <System.dll>

#include "gui_scene.h"

namespace My3D
{
  ref class SceneObject;

  public ref class Document
  {
    public:
       Document ();
       ~Document ();

       void Save   ();
       void SaveAs ();
       void Load   ();
       void Close  ();
       void DeleteContents ();

       property bool NeedSave
       {
         bool get (void)
         {
           return scene.Modified;
         }
         void set (bool value)
         {
           scene.Modified = value;
         }
       }

    public:
       Scene scene;
  };
}
