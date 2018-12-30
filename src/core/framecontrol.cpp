#include <time.h>
#include <scene.h>
#include <render.h>
#include <cursor.h>
#include <cmd.h>
#include <mesh.h>
#include <string>

#include "framecontrol.h"
#include "manipulators.h"
#include "misc.h"

using namespace std;

const float MIN_UPDATE_TIME = 0.01f;
const char* FRAME_DEFAULT_MANIPULATOR = "select";
const char* SCENES_DIR = "media/scenes";

class LogSceneListener: public SceneListener
{
  public:
//    void OnCreate (Entity* obj)       { logPrintf ("LogSceneListener: Create entity '%s'",obj->name ()); }
//    void OnCreate (Light* obj)        { logPrintf ("LogSceneListener: Create light  '%s'",obj->name ()); }
//    void OnCreate (Camera* obj)       { logPrintf ("LogSceneListener: Create camera '%s'",obj->name ()); }
//    void OnCreate (ParamModel* obj)   { logPrintf ("LogSceneListener: Create model  '%s'",obj->name ());}
//    void OnCreate (HelperEntity* obj) { logPrintf ("LogSceneListener: Create helper '%s'",obj->name ()); }
//    void OnDelete (Entity* obj)       { logPrintf ("LogSceneListener: Delete entity '%s'",obj->name ()); }
//    void OnDelete (Light* obj)        { logPrintf ("LogSceneListener: Delete light  '%s'",obj->name ()); }
//    void OnDelete (Camera* obj)       { logPrintf ("LogSceneListener: Delete camera '%s'",obj->name ()); }
//    void OnDelete (ParamModel* obj)   { logPrintf ("LogSceneListener: Delete model  '%s'",obj->name ()); }
//    void OnDelete (HelperEntity* obj) { logPrintf ("LogSceneListener: Delete helper '%s'",obj->name ()); }
//    void OnModify (Entity* obj)       { logPrintf ("LogSceneListener: Modify entity '%s'",obj->name ()); }
//    void OnModify (Light* obj)        { logPrintf ("LogSceneListener: Modify light  '%s'",obj->name ()); }
//    void OnModify (Camera* obj)       { logPrintf ("LogSceneListener: Modify camera '%s'",obj->name ()); }
//    void OnModify (ParamModel* obj)   { logPrintf ("LogSceneListener: Modify model  '%s'",obj->name ()); }
//    void OnModify (HelperEntity* obj) { logPrintf ("LogSceneListener: Modify helper '%s'",obj->name ()); }
};

class FrameControlImpl: public FrameControl
{
  public:          
    FrameControlImpl (Render& _render) 
      : scene (true), render (_render), cursor (_render), cmd (Command::instance ())
    {      
      RegisterManipulators ();      
      
      cmd.bind ("select",*this,&FrameControlImpl::cmdSelect);
      cmd.bind ("move",*this,&FrameControlImpl::cmdMove);
      cmd.bind ("rotate",*this,&FrameControlImpl::cmdRotate); 
      cmd.bind ("scale",*this,&FrameControlImpl::cmdScale); 
      cmd.bind ("shear",*this,&FrameControlImpl::cmdShear); 
      cmd.bind ("create",*this,&FrameControlImpl::cmdCreate);
      cmd.bind ("testBox",*this,&FrameControlImpl::cmdCreateTestBox);
      cmd.bind ("bind",*this,&FrameControlImpl::cmdBind);      
      cmd.bind ("pencil_extrude",*this,&FrameControlImpl::cmdPencilExtrude);      
      cmd.bind ("brush_extrude",*this,&FrameControlImpl::cmdBrushExtrude);      
      cmd.bind ("dump",*this,&FrameControlImpl::cmdDump);
      cmd.bind ("reset",*this,&FrameControlImpl::cmdReset);
      cmd.bind ("save",*this,&FrameControlImpl::cmdSave);
      cmd.bind ("saveas",*this,&FrameControlImpl::cmdSaveAs);
      cmd.bind ("load",*this,&FrameControlImpl::cmdLoad);
      cmd.bind ("import",*this,&FrameControlImpl::cmdImport);
      cmd.bind ("remove_deps",*this,&FrameControlImpl::cmdRemoveDeps);
      cmd.bind ("delete",*this,&FrameControlImpl::cmdDelete);
      cmd.bind ("mirror_xy",*this,&FrameControlImpl::cmdMirrorXY);
      cmd.bind ("mirror_yx",*this,&FrameControlImpl::cmdMirrorXY);
      cmd.bind ("mirror_xz",*this,&FrameControlImpl::cmdMirrorXZ);
      cmd.bind ("mirror_zx",*this,&FrameControlImpl::cmdMirrorXZ);
      cmd.bind ("mirror_yz",*this,&FrameControlImpl::cmdMirrorYZ);
      cmd.bind ("mirror_zy",*this,&FrameControlImpl::cmdMirrorYZ);
      cmd.bind ("mirror_x",*this,&FrameControlImpl::cmdMirrorX);
      cmd.bind ("mirror_y",*this,&FrameControlImpl::cmdMirrorY);
      cmd.bind ("mirror_z",*this,&FrameControlImpl::cmdMirrorZ);
      cmd.bind ("mirror_o",*this,&FrameControlImpl::cmdMirrorO);
      cmd.bind ("mirror",*this,&FrameControlImpl::cmdMirrorO);
      cmd.bind ("camera",*this,&FrameControlImpl::cmdCamera);
      
      last_update_time = 0;      

      cursor.SetManipulator (FRAME_DEFAULT_MANIPULATOR);
      scene.RegisterListener (&scene_listener);

      cmd.etc ().set ("ortX",vec3f (1.0f,0.0f,0.0f));
      cmd.etc ().set ("ortY",vec3f (0.0f,1.0f,0.0f));
      cmd.etc ().set ("ortZ",vec3f (0.0f,0.0f,1.0f));            
      
      InitScene ();
      
//      scene.load ("my.wxf");
    }
      
    ~FrameControlImpl ()
    {      
      ClearScene ();
    }
    
    void MouseMove (int x,int y) {
      cursor.SetPosition (x,y);      
    }
    
    void MouseDown (MouseButton button,int x,int y) {
      cursor.SetPosition (x,y);      
      
      switch (button)
      {
        case MOUSE_LBUTTON:  
          cursor.BeginAction ();  
          break;
        case MOUSE_RBUTTON:
          if (cursor.IsActive ())
            cursor.CancelAction ();
          break;
      }
      
      if (cursor.IsDefaultManipulator ())  
        cursor.SetManipulator (FRAME_DEFAULT_MANIPULATOR);      
    }
    
    void MouseUp (MouseButton button,int x,int y) {
      cursor.SetPosition (x,y);
      
      switch (button)
      {
        case MOUSE_LBUTTON:  
          cursor.AcceptAction ();
          break;
      }
      
      if (cursor.IsDefaultManipulator ())
        cursor.SetManipulator (FRAME_DEFAULT_MANIPULATOR);
    }

    void MouseWheel (int wheel) {
      Camera* camera = render.GetActiveViewport ()->GetCamera ();
      
      if (!camera)
        return;                          
                
      for (;wheel>0;wheel-=120) camera->translate (vec3f (0,0,-2));
      for (;wheel<0;wheel+=120) camera->translate (vec3f (0,0,2));
    } 
    
    void KeyDown (Key key) {
      switch (key)
      {
        case KEY_DEL: cmd.exec ("delete"); break;
      }
    }
    
    void idle () {
      cursor.IdleAction ();
    }
        
    Scene& GetScene () { return scene; }
    
    void InitScene ()
    {
      logPrintf ("Init scene");
      
      Camera* camera = scene.CreateCamera ();
      Light*  light = scene.CreateLight ();
        
      camera->SetPerspective (90,1,1000);
      camera->lookat         (vec3f (20,20,20),vec3f (0,0,0));
      camera->rename         ("Perspective");
      light->bind            (camera);
      camera->enable         (ENTITY_TEMP);
      light->enable          (ENTITY_TEMP);    
      
      light->SetSpotLight   (0.0f,vec3f (0.0f,0.0f,1.0f),90.0f);
      light->SetLightColor  (LIGHT_AMBIENT_COLOR,1.0f);
      light->SetLightColor  (LIGHT_DIFFUSE_COLOR,1.0f);
      light->SetLightColor  (LIGHT_SPECULAR_COLOR,1.0f);
      light->SetInnerRadius (100.0f);        
  
      render.viewport (2)->SetView (&scene,VIEW_LEFT);  
      render.viewport (3)->SetView (&scene,VIEW_TOP);
      render.viewport (0)->SetView (&scene,VIEW_FRONT);
      render.viewport (3)->enable  (VIEWPORT_WIREFRAME);
      render.viewport (2)->enable  (VIEWPORT_WIREFRAME);
      render.viewport (0)->enable  (VIEWPORT_WIREFRAME);      

      if (!render.viewport (1)->GetCamera ())
        render.viewport (1)->SetView (camera);
        
//      LandscapeModel* landscape = scene.CreateLandscape ();
      
//      landscape->translate (vec3f (0.0f,0.0f,0.0f));      
//      landscape->scale     (vec3f (10.0f,1.0f,10.0f));
//      landscape->BrushExtrusion (40,64,64,-30);            
      
      CoreUnlockUpdate ();
    }
    
    void ClearScene ()
    {
      logPrintf ("Clear scene");
      
      CoreLockUpdate ();
      
      cursor.SetManipulator (FRAME_DEFAULT_MANIPULATOR);      
      
      for (int i=0;i<Render::VIEWPORTS_COUNT;i++)
        render.viewport (i)->SetView (NULL);
        
      CoreSetCurrentFilename ("");
    }
    
    void cmdCreate (CmdArgs& args) {
      char buf [128] = {0};      
      
      if (!args.argc ())
      {
        logPrintf ("create <object_type> [<param> ...]");
        return;
      }
      
      if (!strcmp (args.gets (0),"tank"))
      {        
        Command::instance ().execf ("import %s/tank.wxf",SCENES_DIR);
        return;
      }
      else if (!strcmp (args.gets (0),"crane"))
      {
        Command::instance ().execf ("import %s/crane.wxf",SCENES_DIR);
        return;        
      }
      else if (!strcmp (args.gets (0),"headphones"))
      {
        Command::instance ().execf ("import %s/headphones.wxf",SCENES_DIR);
        return;        
      }      
      else if (!strcmp (args.gets (0),"fdc"))
      {
        Command::instance ().execf ("import %s/fdc.wxf",SCENES_DIR);
        return;        
      }            
      else if (!strcmp (args.gets (0),"spaceship"))
      {
        Command::instance ().execf ("import %s/spaceship.wxf",SCENES_DIR);
        return;        
      }                  
      
      _snprintf (buf,sizeof (buf),"create_%s",args.gets (0));      

      cursor.SetManipulator (buf);      

    }        
    
    void cmdCreateTestBox (CmdArgs&) { 
      ParamModel* model = scene.CreateModel ();      
      
      model->SetModelType (MODEL_BOX);
      model->scale        (4.0f);
    }

    void cmdSelect (CmdArgs& args) { 
      if (!args.argc ())
      {
        cursor.SetManipulator ("select"); 
        return;
      }
      
      scene.ClearSelection ();
      
      for (size_t i=0;i<args.argc ();i++)
      {
        Entity* entity = scene.root ()->find (args.gets (i));
        
        if (!entity)
        {
          logPrintf ("Warning! Entity '%s' not found",args.gets (i));
          continue;
        }
        
        entity->select (ENTITY_GROUP_SELECT);
      }
    }
       
    void cmdMove (CmdArgs& args) { 
      if (!args.argc ())
      {
        cursor.SetManipulator ("move");         
        return;
      }            
      
      vec3f delta = 0;
      
      for (size_t i=0;i<min (args.argc (),3u);i++)
        delta [i] = args.getf (i);
      
      const EntityList& selection = scene.GetSelection ();
      
      for (size_t i=0;i<selection.size ();i++)
        selection.item (i)->translate (delta);
    }
    
    void cmdRotate (CmdArgs& args) { 
      if (!args.argc ())
      {
        cursor.SetManipulator ("rotate");
        return;
      }
      
      vec3f rot = 0;
      
      for (size_t i=0;i<min (args.argc (),3u);i++)
        rot [i] = args.getf (i);
        
      const EntityList& selection = scene.GetSelection ();
      
      for (size_t i=0;i<selection.size ();i++)
        selection.item (i)->rotate (rot.x,rot.y,rot.z);
    }
    
    void cmdScale  (CmdArgs& args) { 
      if (!args.argc ())
      {
        cursor.SetManipulator ("scale");         
        return;
      }
      
      vec3f scale = 1;
      
      for (size_t i=0;i<min (args.argc (),3u);i++)
        scale [i] = args.getf (i);
        
      const EntityList& selection = scene.GetSelection ();
      
      for (size_t i=0;i<selection.size ();i++)
        selection.item (i)->scale (scale);
    }
    
    void cmdMirrorXY (CmdArgs&) { cmd.exec ("scale 1 1 -1"); }
    void cmdMirrorXZ (CmdArgs&) { cmd.exec ("scale 1 -1 1"); }    
    void cmdMirrorYZ (CmdArgs&) { cmd.exec ("scale -1 1 1"); }
    void cmdMirrorX  (CmdArgs&) { cmd.exec ("scale 1 -1 -1"); }
    void cmdMirrorY  (CmdArgs&) { cmd.exec ("scale -1 1 -1"); }
    void cmdMirrorZ  (CmdArgs&) { cmd.exec ("scale -1 -1 1"); }
    void cmdMirrorO  (CmdArgs&) { cmd.exec ("scale -1 -1 -1"); }
    
    void cmdShear     (CmdArgs&) { cursor.SetManipulator ("shear"); }        
    void cmdBind          (CmdArgs&) { cursor.SetManipulator ("bind"); }
    
    void cmdPencilExtrude (CmdArgs& args) 
    {
      cursor.SetManipulator ("pencil_extrude"); 

      if (args.argc ())
      {
        cmd.etc().set ("manipulator.radius", int (args.geti (0)));
        if (args.argc () > 1)
        {
          cmd.etc().set ("manipulator.flow", float (args.getf (1)));
          if (args.argc () > 2)         
            cmd.etc().set ("manipulator.mode", int (args.geti (2)));
        }

      }
    }

    void cmdBrushExtrude  (CmdArgs& args) 
    {
      cursor.SetManipulator ("brush_extrude"); 

      if (args.argc ())
      {
        cmd.etc().set ("manipulator.radius", int (args.geti (0)));
        if (args.argc () > 1)
        {
          cmd.etc().set ("manipulator.flow", float (args.getf (1)));
          if (args.argc () > 2)         
            cmd.etc().set ("manipulator.mode", int (args.geti (2)));
        }

      }
    }
    
    void cmdDump (CmdArgs& args) {      
      cmd.etc ().dump (args.argc () ? args.gets (0) : "*");
    }
    
    void cmdRemoveDeps (CmdArgs& args) {
      const char* name = args.argc () ? args.gets (0) : scene.root ()->name ();
      
      Entity* entity = scene.root ()->find (name);
      
      if (!entity)
      {
        logPrintf ("There is no entity with name '%s'",name);
        return;
      }
      
      entity->RemoveDependencies ();
    }
    
    void cmdReset (CmdArgs&)  
    {
      ClearScene  ();
      logPrintf   ("Reset scene");
      scene.reset ();
      InitScene   ();
    }
    
    void cmdSave (CmdArgs& args)
    {            
      string filename = args.argc () && *args.gets (0) ? args.gets (0) : CoreGetSaveFilename ();
      
      if (!filename.empty ())      
        scene.save (filename.c_str ());
      else 
        Command::instance ().exec ("saveas");
    }   
    
    void cmdSaveAs (CmdArgs& args)
    {
      string filename = args.argc () && *args.gets (0) ? args.gets (0) : CoreGetSaveAsFilename (SCENES_DIR);
      
      scene.save (filename.c_str ());
    }           
    
    void cmdLoad (CmdArgs& args)
    {
      ClearScene ();
      scene.reset ();      
      render.viewport(1)->activate ();      
      
      string filename = args.argc () && *args.gets (0) ? args.gets (0) : CoreGetOpenFilename (SCENES_DIR);
      
      if (!filename.empty ())      
        scene.load (filename.c_str ());

      InitScene  ();            
    }
    
    void cmdImport (CmdArgs& args)
    {
      render.viewport(1)->activate ();      
            
      if (!args.argc ())
      {
        logPrintf ("import <filename>");
        return;
      }
    
      scene.load (args.gets (0));
    }    
    
    void _delete (Entity* entity)
    {
      if (entity == scene.root ())
      {
        logPrintf ("There is no ways for delete root object. User 'reset'");
        return;
      }
            
      for (int i=0;i<Render::VIEWPORTS_COUNT;i++)
        if (entity == render.viewport (i)->GetCamera ())
        {
          render.viewport (i)->SetView (NULL);
          return;
        }
        
      entity->unbind ();
    }
    
    void cmdDelete (CmdArgs& args)
    {      
      if (!args.argc ())
      {
        while (scene.GetSelection ().size ())
          _delete (scene.GetSelection ().item (0));  
        
        return;
      }
      
      for (size_t i=0;i<args.argc ();i++)
      {
        Entity* entity = scene.root ()->find (args.gets (i));
        
        if (!entity)
        {
          logPrintf ("Warning! No entity with name '%s' found in scene",args.gets (0));
          continue;
        }
        
        _delete (entity);        
      }      
    }
    
    void cmdCameraMove (Camera* camera,CmdArgs& args) {      
      if (!args.argc ())
      {
        cursor.SetManipulator ("camera_move_xy");
        return;
      }            
      
      vec3f delta = 0;
      
      for (size_t i=0;i<min (args.argc (),3u);i++)
        delta [i] = args.getf (i);
        
      camera->translate (delta);
    }    
    
    void cmdCameraRotate (Camera* camera,CmdArgs& args) {
      if (!args.argc ())
      {
        cursor.SetManipulator ("camera_self_rotate");
        return;
      }
      
      vec3f rot = 0;
      
      for (size_t i=0;i<min (args.argc (),3u);i++)
        rot [i] = -args.getf (i);                        
        
      camera->rotate (rot.x,rot.y,rot.z);
    }
    
    void cmdCameraZoom (Camera* camera,CmdArgs& args) {
      if (!args.argc ())
      {
        cursor.SetManipulator ("camera_zoom");
        return;
      }
      
      float zoom_x = 1.0f/args.getf (0),
            zoom_y = args.argc () > 1 ? 1.0f/args.getf (1) : zoom_x;
      
      camera->scale (vec3f (zoom_x,zoom_y,1.0f));
    }        
    
    void cmdCameraSet (Camera*,CmdArgs& args) {
       const char* name = args.gets (0);
       
       if (!name)
       {
         logPrintf ("camera set <name>");
         return;
       }
       
       Entity* obj = scene.root ()->find (name);
       
       if (!obj)
       {
         logPrintf ("Camera '%s' not found",name);
         return;
       }
       
       Camera* camera = dynamic_cast<Camera*>(obj);
       
       if (!camera)
       {
         logPrintf ("Entity '%s' isn't a camera",name);
         return;
       }

       if (!render.GetActiveViewport ())
       {
         logPrintf ("No activate viewport selected");
         return;
       }
       
       render.GetActiveViewport ()->SetView (camera);
    }
    
    void cmdCamera (CmdArgs& args) {      
      if (!args.argc ())
      {
        logPrintf ("camera <command:move|rotate|zoom> [param ...]");
        return;
      }
            
      if (!render.GetActiveViewport ())
      {
        logPrintf ("No active viewport selected");
        return;
      }
      
      Camera* camera = render.GetActiveViewport ()->GetCamera ();
      
      if (!camera && strcmp (args.gets (0),"set"))
      {
        logPrintf ("No active camera in viewport selected");
        return;
      }
      
      const char* command = args.gets (0);
      
      args.shift (1);
      
      if      (!strcmp (command,"move"))   cmdCameraMove   (camera,args);
      else if (!strcmp (command,"rotate")) cmdCameraRotate (camera,args);
      else if (!strcmp (command,"zoom"))   cmdCameraZoom   (camera,args);
      else if (!strcmp (command,"set"))    cmdCameraSet    (camera,args);
      else                                 logPrintf ("No camera action with name '%s'",command);
    }
    
    void RegisterManipulators () {
      cursor.RegisterManipulator ("select",manipCreateSelect);
      cursor.RegisterManipulator ("move",manipCreateMove);
      cursor.RegisterManipulator ("rotate",manipCreateRotate);
      cursor.RegisterManipulator ("scale",manipCreateScale);
      cursor.RegisterManipulator ("shear",manipCreateShear);
      cursor.RegisterManipulator ("pencil_extrude",manipCreatePencilExtrude);
      cursor.RegisterManipulator ("brush_extrude",manipCreateBrushExtrude);
      cursor.RegisterManipulator ("create_box",manipCreateBox);
      cursor.RegisterManipulator ("create_sphere",manipCreateSphere);
      cursor.RegisterManipulator ("create_halfsphere",manipCreateHalfSphere);
      cursor.RegisterManipulator ("create_cone",manipCreateCone);
      cursor.RegisterManipulator ("create_halfcone",manipCreateHalfCone);
      cursor.RegisterManipulator ("create_cylinder",manipCreateCylinder);
      cursor.RegisterManipulator ("create_halfcylinder",manipCreateHalfCylinder);
      cursor.RegisterManipulator ("create_tetrahedron",manipCreateTetrahedron);
      cursor.RegisterManipulator ("create_hexahedron",manipCreateHexahedron);
      cursor.RegisterManipulator ("create_octahedron",manipCreateOctahedron);
      cursor.RegisterManipulator ("create_dodecahedron",manipCreateDodecahedron);
      cursor.RegisterManipulator ("create_icosahedron",manipCreateIcosahedron);
      cursor.RegisterManipulator ("create_halftorus",manipCreateHalfTorus);
      cursor.RegisterManipulator ("create_torus",manipCreateTorus);
      cursor.RegisterManipulator ("create_halftorus",manipCreateHalfTorus);
      cursor.RegisterManipulator ("create_landscape",manipCreateLandscape);
      cursor.RegisterManipulator ("create_point",manipCreatePointLight);
      cursor.RegisterManipulator ("create_spot_free",manipCreateSpotFreeLight);
      cursor.RegisterManipulator ("create_spot_target",manipCreateSpotTargetLight);
      cursor.RegisterManipulator ("create_direct_free",manipCreateDirectFreeLight);
      cursor.RegisterManipulator ("create_direct_target",manipCreateDirectTargetLight);
      cursor.RegisterManipulator ("bind",manipCreateBind);
      cursor.RegisterManipulator ("camera_move_xy",manipCameraMoveRL_UD);
      cursor.RegisterManipulator ("camera_move_xz",manipCameraMoveRL_FB);
      cursor.RegisterManipulator ("camera_move_zy",manipCameraMoveFB_UD);
      cursor.RegisterManipulator ("camera_self_rotate",manipCameraSelfRotate);
      cursor.RegisterManipulator ("camera_zoom",manipCameraZoom);
    }                
    
  private:
    Command&     cmd;
    Render&      render;
    Scene        scene;
    RenderCursor cursor;
    clock_t      last_update_time;
    LogSceneListener scene_listener;
};

FrameControl* CreateFrameControl (Render& render)
{
  return new FrameControlImpl (render);
}
