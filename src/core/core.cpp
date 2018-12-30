#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <core.h>
#include <render.h>
#include <cursor.h>
#include <cmd.h>
#include <list>
#include <set>

#include "misc.h"
#include "framecontrol.h"

using namespace std;

const char* WND_CLASS_NAME = "Core render window";

static CoreEntityHandle get_handle (Entity* entity)
{
  return (CoreEntityHandle)entity;  
}

static Entity* get_entity (CoreEntityHandle handle)
{
  return (Entity*)handle;
}

class CoreSceneListenerImpl;

typedef set<CoreEntityHandle>    EntityHandleSet;
typedef vector<CoreEntityHandle> EntityHandleVector;

static EntityHandleSet    update_map, already_updated;
static size_t             silent_mode = 0;
static bool               update_transaction = false;
static bool               update_selection = false;
static EntityHandleVector selection;

static FILE*             log_file      = NULL;
static core_log_func     log_callback  = NULL;
static HWND              render_window = NULL;
static HINSTANCE         instance      = NULL;
static Render*           render        = NULL;
static FrameControl*     frame_control = NULL;
static CoreSceneListenerImpl* internal_listener = NULL;
static EntityHandleSet   valid_handles;

static core_change_manipulator_func on_change_manipulator = NULL;

static void CoreShowHelp ();

static bool IsSilentMode ();
static void SetSilentMode (bool state);

static void RegisterClass   ();
static void UnregisterClass ();

static void PostRepaint ();

static void FatalErrorHandler ()
{
//  abort ();
}

void CoreInit ()
{
  if (frame_control)
    CoreShutdown ();

  log_file = fopen ("runlog","wt");  
  
  logPrintf ("Core init");
    
  int   argc = 1;
  char* argv = "core.dll";
  
  glutInit  (&argc,&argv);
  
  __try
  {
    RegisterClass ();
    
    render        = Render::create ();
    frame_control = CreateFrameControl (*render);        
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreInit");
    FatalErrorHandler ();
  }
}

void CoreShutdown ()
{
  if (!frame_control)
    return;

  logPrintf ("Core shutdown");
  
  __try
  {
    CoreSetSceneListener (NULL);
    CoreSetManipulatorCallback (NULL);
      
    delete frame_control;
    
    frame_control = NULL;
    
    if (render)
    {
      render->release ();  
    
      render = NULL;    
    }
    
    UnregisterClass ();
  }  
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreShutdown");
    FatalErrorHandler ();
  }
  
  if (log_file)
     fclose (log_file);      
}

void CoreResize (int left,int top,int width,int height)
{  
  if (render_window)
    SetWindowPos (render_window,HWND_TOP,left,top,width,height,SWP_FRAMECHANGED|SWP_DRAWFRAME);
}

/*
    Render routines
*/

static Key vk2key (int key)
{
  switch (key)
  {
    case VK_DELETE: return KEY_DEL;
    case VK_ESCAPE: return KEY_ESC;
    default:        return (Key)-1;
  }
}

static VOID CALLBACK RenderTimerProc (HWND wnd,UINT msg,UINT event,DWORD time)
{
  frame_control->idle ();

  PostMessage (wnd,WM_PAINT,0,0);
}

static LRESULT RenderWndProc (HWND wnd,UINT msg,WPARAM wp,LPARAM lp)
{
//  RenderWindow* context = (RenderWindow*)GetWindowLong (wnd,GWL_USERDATA);  

  SetSilentMode (true);
   
  switch (msg)
  {
    case WM_CREATE:
    {
      __try
      {
        if (!render->SetWindow (wnd))
        {
          logPrintf     ("CoreInitRender: Error at set render window");
          DestroyWindow (wnd);
          SetSilentMode (false);
          return 1;
        }
        
        SetTimer (wnd,0,10,RenderTimerProc);        
      }
      __except(EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at create render window");
        FatalErrorHandler ();        
      }
      
      break;
    }
    case WM_DESTROY:
    {        
      __try
      {
        render->SetWindow (NULL);                     
      }
      __except(EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at destroy render window");
        FatalErrorHandler ();
      }
      break;
    }
    case WM_SIZE:
    {
      __try
      {          
        RECT rect;
        
        GetClientRect (wnd,&rect);
        
        int width  = rect.right-rect.left,
            height = rect.bottom-rect.top;          
        
        if (width && height)
          render->SetWindowRect (rect.left,rect.top,width,height);                  
      }
      __except(EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at resize render window");
        FatalErrorHandler ();        
      }
      
      PostRepaint ();                  
      break;
    }
    case WM_PAINT:
    {              
      __try
      {
        PAINTSTRUCT ps;      
              
        BeginPaint (wnd,&ps);
        render->repaint ();
        EndPaint (wnd,&ps);              
      }
      __except(EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at repaint");
        FatalErrorHandler ();
      }

      break;
    }
    case WM_MOUSEMOVE:    
      __try
      {    
        frame_control->MouseMove (LOWORD (lp),HIWORD (lp));         
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at MouseMove");
        FatalErrorHandler ();
      }
      
      PostRepaint ();              
      break;
    case WM_LBUTTONDBLCLK:
    {
      __try
      {    
        render->SetViewportMode (render->GetViewportMode () == RENDER_MULTI_VIEWPORT_MODE ?
                                 RENDER_SINGLE_VIEWPORT_MODE : RENDER_MULTI_VIEWPORT_MODE);        
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at LeftButtonDoubleClick");
        FatalErrorHandler ();
      }      
      
      PostRepaint ();                               
      SetFocus (render_window);                  
      break;        
    }
    case WM_LBUTTONDOWN: 
    {
      __try
      {    
        frame_control->MouseDown (MOUSE_LBUTTON,LOWORD (lp),HIWORD (lp));         
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at LeftButtonDown");
        FatalErrorHandler ();
      }      
      
      PostRepaint ();      
      SetFocus (render_window);      
      break;
    }
    case WM_RBUTTONDOWN: 
    {
      __try
      {    
        frame_control->MouseDown (MOUSE_RBUTTON,LOWORD (lp),HIWORD (lp));         
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at RightButtonDown");
        FatalErrorHandler ();
      }      
      
      PostRepaint ();      
      SetFocus (render_window);      
      break;
    }
    case WM_MBUTTONDOWN: 
    {
      __try
      {    
        frame_control->MouseDown (MOUSE_MBUTTON,LOWORD (lp),HIWORD (lp));         
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at MiddleButtonDown");
        FatalErrorHandler ();
      }      

      PostRepaint ();      
      SetFocus (render_window);      
      break;
    }
    case WM_LBUTTONUP:   
    {
      __try
      {    
        frame_control->MouseUp (MOUSE_LBUTTON,LOWORD (lp),HIWORD (lp));         
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at LeftButtonUp");
        FatalErrorHandler ();
      }      

      PostRepaint ();
      break;
    }
    case WM_RBUTTONUP:   
    {
      __try
      {
        frame_control->MouseUp (MOUSE_RBUTTON,LOWORD (lp),HIWORD (lp));         
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at RightButtonUp");
        FatalErrorHandler ();
      }      

      PostRepaint ();      
      break;      
    }
    case WM_MBUTTONUP:   
    {
      __try
      {    
        frame_control->MouseUp (MOUSE_MBUTTON,LOWORD (lp),HIWORD (lp));         
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at MiddleButtonUp");
        FatalErrorHandler ();
      }      
      
      PostRepaint ();      
      break;
    }
    case WM_MOUSEWHEEL:
    {
      __try
      {    
        frame_control->MouseWheel ((short)HIWORD (wp));
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at MouseWheel");
        FatalErrorHandler ();
      }      
      
      PostRepaint ();
      break;      
    }   
    case WM_CHAR:        
    {
      __try
      {    
        frame_control->PutChar (wp);         
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at PutChar");
        FatalErrorHandler ();
      }    

      PostRepaint ();      
      break;
    }
    case WM_KEYDOWN:
    {
      __try
      {    
        if (wp == VK_F1)
        {
          CoreShowHelp ();
        }
        else
        {
          Key key = vk2key (wp);
          
          if (key == (Key)-1)
            break;        
            
          frame_control->KeyDown (key);                  
        }        
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at KeyDown");
        FatalErrorHandler ();
      }      

      PostRepaint ();
      break;
    }
    case WM_KEYUP:
    {
      __try
      {    
        Key key = vk2key (wp);      
        
        if (key == (Key)-1)
          break;              
          
        frame_control->KeyUp (key);        
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at KeyUp");
        FatalErrorHandler ();
      }      

      PostRepaint ();
      break;
    }
    default: 
      SetSilentMode (false);
      return DefWindowProc (wnd,msg,wp,lp);  
  }  
  
  SetSilentMode (false);

  return 0;
}

static void RegisterClass ()
{
  WNDCLASS wc;

//  wc.style         = CS_HREDRAW|CS_VREDRAW;
//  wc.style         = CS_OWNDC;
  wc.style         = CS_DBLCLKS;
  wc.lpfnWndProc   = (WNDPROC)RenderWndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = instance;
  wc.hIcon         = LoadIcon (instance,IDI_APPLICATION);
  wc.hCursor       = LoadCursor(instance,IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = WND_CLASS_NAME;

  if (!RegisterClass (&wc))
    logPrintf ("CoreInit: Error at register window class '%s'",WND_CLASS_NAME);
}

static void UnregisterClass ()
{
  if (!UnregisterClass (WND_CLASS_NAME,instance))
    logPrintf ("CoreShutdown: Error at unregister window class '%s'",WND_CLASS_NAME);
}

void PostRepaint ()
{
  PostMessage (render_window,WM_PAINT,0,0);
}

void CoreInitRender (void* wnd_handle)
{
  logPrintf ("Core init render at window %p",wnd_handle);
  
  __try
  {
    SetSilentMode (true);
    
    if (render_window)
      CoreShutdownRender ();
    
    HWND wnd = CreateWindow (WND_CLASS_NAME,WND_CLASS_NAME,WS_CHILD,
                             0,0,CW_USEDEFAULT,CW_USEDEFAULT,(HWND)wnd_handle,NULL,instance,NULL);

    if (!wnd)
    {
      logPrintf ("CoreInit render: Error at create internal render window with class '%s'",WND_CLASS_NAME);
      SetSilentMode (false); 
      return;
    }  
    
    render_window = wnd;
                                                        
    ShowWindow (wnd,SW_MAXIMIZE);
    RECT rect;
        
    GetClientRect (wnd,&rect);
    
    render->SetSplit ((rect.right+rect.left)/2,(rect.bottom+rect.top)/2);    
    
    UpdateWindow (wnd);
    
    SetSilentMode (false);    
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreRenderInit");
    FatalErrorHandler ();
  }
}

CoreAPI void CoreShutdownRender ()
{
  __try
  {
        if (!render_window)
    {
      logPrintf ("CoreShutdownRender: Error. Render not created");
      return;    
    }
    
    SetSilentMode (true);  
    
    logPrintf ("Core shutdown render");  
    
    DestroyWindow (render_window);  
    
    render_window = NULL;
    
    SetSilentMode (false);
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreShutdownRender");
    FatalErrorHandler ();
  }
}

CoreAPI bool CoreIsRenderInit ()
{
  return render_window != NULL;
}

/*
    Log routines
*/

void logPrintf (const char* format,...)
{
  if (!log_file)
    return;
     
  va_list list;

  va_start (list,format);
  
  static char buf [1024];
    
  _vsnprintf (buf,sizeof (buf),format,list);  

  fprintf (log_file,"[%8u] ",clock ());       
  fprintf (log_file,"%s\n",buf);
  fflush  (log_file);  
  printf  ("[%8u] ",clock ());
  printf  ("%s\n",buf);
  fflush  (stdout);
  
  if (log_callback)
    log_callback (buf);
}

void CoreLogMessage (const char* msg)
{
  __try
  {
    if (msg)
      logPrintf ("%s",msg);      
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
  }
}

void CoreLogCallback (core_log_func _callback)
{
  log_callback = _callback;
}

/*
    Command routines
*/

void CoreDoCommand (const char* command)
{
  SetSilentMode (true);
  
  __try
  {
    Command::instance ().exec (command);    
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreDoCommand '%s'",command);
    FatalErrorHandler ();
  }
  
  SetSilentMode (false);
  PostRepaint ();
}

/*
    Scnene routines
*/

class CoreSceneListenerImpl: public SceneListener
{
  public:
    CoreSceneListenerImpl (Scene& _scene,const CoreSceneListener& _listener) 
      : scene (_scene), listener (_listener), is_registered (false)
    {      
      RegisterListener ();
    }
    
    ~CoreSceneListenerImpl () {
      UnregisterListener ();
    }
    
    void RegisterListener () {
      if (is_registered)
        return;               
        
      SetSilentMode          (true);
      scene.RegisterListener (this);      
      SetSilentMode          (false);
      is_registered = true;  
    }
    
    void UnregisterListener () { 
      if (!is_registered)
        return;
        
      SetSilentMode            (true);
      scene.UnregisterListener (this);       
      SetSilentMode            (false);
      is_registered = false;        
    }
  
    void OnCreate (Entity* obj)       { _OnCreate (obj); }
    void OnCreate (Light* obj)        { _OnCreate (obj);   }
    void OnCreate (Camera* obj)       { _OnCreate (obj);  }
    void OnCreate (ParamModel* obj)   { _OnCreate (obj);   }
    void OnCreate (HelperEntity* obj) { _OnCreate (obj);  }
    void OnDelete (Entity* obj)       { _OnDelete (obj); }
    void OnDelete (Light* obj)        { _OnDelete (obj);   }
    void OnDelete (Camera* obj)       { _OnDelete (obj);  }
    void OnDelete (ParamModel* obj)   { _OnDelete (obj);   }
    void OnDelete (HelperEntity* obj) { _OnDelete (obj);  }
    void OnModify (Entity* obj)       { _OnModify (obj); } 
    void OnModify (Light* obj)        { _OnModify (obj);   } 
    void OnModify (Camera* obj)       { _OnModify (obj);  } 
    void OnModify (ParamModel* obj)   { _OnModify (obj);   } 
    void OnModify (HelperEntity* obj) { _OnModify (obj);  }
    
    void OnSelect (Entity*) {
      if (IsSilentMode ())       
        update_selection = true;
      else
        _CallOnSelect ();
    }
    
    void _CallOnSelect () {
      __try
      {      
        if (listener.onSelect)
          listener.onSelect ();
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at Core::OnSelect");
        FatalErrorHandler ();
      }      
    }
            
    void _OnModify (Entity* obj) {      
      if (IsSilentMode ())
        update_map.insert (get_handle (obj));
      else
        _CallOnModify (obj);
    }    
    
    void _OnCreate (Entity* obj) {      
      const char* name = "__unknown__";
      
      __try
      {
        if (!obj)
        {
          logPrintf ("Warning! Null object passed to Core::OnCreate notify");          
          return;
        }
        
        valid_handles.insert (get_handle (obj));              
        update_map.erase (get_handle (obj));
          
        name = obj->name ();                
        
        if (listener.onCreate)
          listener.onCreate (get_handle (obj));              
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at Core::OnCreate '%s'",name);
        FatalErrorHandler ();
      }
    }
    
    void _OnDelete (Entity* obj) {      
      const char* name = "__unknown__";      

      __try
      {
        if (!obj)
        {
          logPrintf ("Warning! Null object passed to Core::OnDelete notify");
          return;
        }
          
        name = obj->name ();        
        
        if (listener.onDelete)
          listener.onDelete (get_handle (obj));
          
        update_map.erase (get_handle (obj));
        valid_handles.erase (get_handle (obj));
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at Core::OnDelete '%s'",name);
        FatalErrorHandler ();
      }
    }    
    
    void _CallOnModify (Entity* obj) {            
      const char* name = "__unknown__";      

      __try      
      {      
        if (!obj)
        {
          logPrintf ("Warning! Null object passed to Core::OnModify notify");
          return;
        }
        
        if (!valid_handles.count (get_handle (obj))) //modify before create
          return;          
          
        if (update_transaction)
        {
          if (already_updated.find (get_handle (obj)) != already_updated.end ())
             return;
             
          already_updated.insert (get_handle (obj));
        }
        
//        logPrintf ("OnModify '%s' (width='%s')",obj->name (),Command::instance ().etc ().gets (obj->property ("width")));
        
        name = obj->name ();        
                      
        if (listener.onModify)
          listener.onModify (get_handle (obj));      
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        logPrintf ("Fatal error at Core::OnModify '%s'",name);
        FatalErrorHandler ();
      }
    }
        
    Scene&            scene;
    CoreSceneListener listener;
    bool              is_registered;
};

bool IsSilentMode ()
{
  return !update_transaction && silent_mode > 0;
}

void SetSilentMode (bool state)
{    
  if (state) silent_mode++;
  else       silent_mode--;
    
  if (!silent_mode && !update_transaction)
  {        
    update_transaction = true;
    
    already_updated.clear ();
    
    if (internal_listener)
    {                                    
      for (EntityHandleSet::iterator i=update_map.begin ();i!=update_map.end ();i++)
        internal_listener->_CallOnModify (get_entity (*i));
        
      if (update_map.size ())
        PostRepaint ();
        
      if (update_selection)
      {
        selection.clear ();
        
        if (frame_control)
        {
          const EntityList& list = frame_control->GetScene ().GetSelection ();
          
          for (size_t i=0;i<list.size ();i++)
            if (valid_handles.count (get_handle (list.item (i))))
              selection.push_back (get_handle (list.item (i)));
        }        
        
        internal_listener->_CallOnSelect ();  
      }
    }

    update_map.clear ();
    already_updated.clear ();
    
    update_transaction = false;
    update_selection = false;
  }
}

void CoreLockUpdate ()
{
  if (internal_listener)
    internal_listener->UnregisterListener ();
}

void CoreUnlockUpdate ()
{
  if (internal_listener)
    internal_listener->RegisterListener ();
}

void CoreSetSceneListener (CoreSceneListener* listener)
{
  if (internal_listener)
  {
    delete internal_listener;
    internal_listener = NULL;
  }

  if (listener)
    internal_listener = new CoreSceneListenerImpl (frame_control->GetScene (),*listener);
}

/*
    Entity params
*/

bool CoreIsValidEntity (CoreEntityHandle handle)
{
  return valid_handles.count (handle) != 0;
}

CoreEntityHandle CoreGetParentEntity (CoreEntityHandle handle)
{
  if (!CoreIsValidEntity (handle))
  {
    logPrintf ("Try to get parent from invalid entity (entity_handle=%p)",handle);
    return 0;
  }  
  
  CoreEntityHandle parent = get_handle (get_entity (handle)->parent ());
  
  return CoreIsValidEntity (parent) ? parent : 0;
}

static const char* GetParamName (CoreEntityParam param)
{
  switch (param)
  {
    case CORE_ENTITY_NAME:             return "name";
    case CORE_ENTITY_PARENT_NAME:      return "parent";
    case CORE_ENTITY_TYPE:             return "type";
    case CORE_ENTITY_VISIBLE:          return "visible";
    case CORE_ENTITY_SELECT:           return "select";
    case CORE_ENTITY_WIRE_COLOR:       return "wire_color";
    case CORE_ENTITY_LOCAL_POSITION:   return "local_position";
    case CORE_ENTITY_LOCAL_ROTATION:   return "local_rotation";
    case CORE_ENTITY_LOCAL_SCALE:      return "local_scale";
    case CORE_ENTITY_LOCAL_DIR:        return "local_dir";
    case CORE_ENTITY_LOCAL_UP:         return "local_up";
    case CORE_ENTITY_WORLD_POSITION:   return "world_position";
    case CORE_ENTITY_WORLD_ROTATION:   return "world_rotation";
    case CORE_ENTITY_WORLD_SCALE:      return "world_scale";
    case CORE_ENTITY_WORLD_DIR:        return "world_dir";
    case CORE_ENTITY_WORLD_UP:         return "world_up";    
    case CORE_LIGHT_ENABLE:            return "enable";
    case CORE_LIGHT_TYPE:              return "light_type";
    case CORE_LIGHT_AMBIENT_COLOR:     return "ambient_color";
    case CORE_LIGHT_DIFFUSE_COLOR:     return "diffuse_color";
    case CORE_LIGHT_SPECULAR_COLOR:    return "specular_color";
    case CORE_LIGHT_SPOT_ANGLE:        return "spot_angle";
    case CORE_LIGHT_SPOT_EXPONENT:     return "spot_exponent";
    case CORE_LIGHT_RADIUS:            return "radius";
    case CORE_MODEL_WIDTH:             return "width";
    case CORE_MODEL_HEIGHT:            return "height";
    case CORE_MODEL_DEPTH:             return "depth";
    case CORE_MODEL_RADIUS:            return "radius";
    case CORE_MODEL_SECOND_RADIUS:     return "radius2";
    case CORE_MODEL_SLICES:            return "slices";
    case CORE_MODEL_PARALLELS:         return "parallels";
    case CORE_MODEL_MERIDIANS:         return "meridians";
    case CORE_MODEL_SHEAR_XY:          return "shear_x";
    case CORE_MODEL_SHEAR_ZY:          return "shear_z";
    case CORE_MATERIAL_AMBIENT_COLOR:  return "mtl.ambient_color";      
    case CORE_MATERIAL_DIFFUSE_COLOR:  return "mtl.diffuse_color";
    case CORE_MATERIAL_SPECULAR_COLOR: return "mtl.specular_color";
    case CORE_MATERIAL_EMISSION_COLOR: return "mtl.emission_color";
    case CORE_MATERIAL_SHININESS:      return "mtl.shininess";
    case CORE_MATERIAL_TRANSPARENCY:   return "mtl.transparency";
    case CORE_MATERIAL_BLEND_MODE:     return "mtl.blend";
    case CORE_MATERIAL_WIREFRAME:      return "mtl.wireframe";
    case CORE_MATERIAL_TWO_SIDED:      return "mtl.two_sided";
    case CORE_CAMERA_PROJECTION:       return "projection";
    case CORE_CAMERA_LEFT:             return "left";
    case CORE_CAMERA_RIGHT:            return "right";
    case CORE_CAMERA_TOP:              return "top";
    case CORE_CAMERA_BOTTOM:           return "bottom";
    case CORE_CAMERA_ZNEAR:            return "znear";
    case CORE_CAMERA_ZFAR:             return "zfar";
    case CORE_CAMERA_DISTANCE:         return "distance";
    case CORE_CAMERA_ALPHA:            return "alpha";
    case CORE_CAMERA_BETA:             return "beta";
    case CORE_CAMERA_POINT1:           return "point1";
    case CORE_CAMERA_POINT2:           return "point2";
    case CORE_CAMERA_POINT3:           return "point3";
    default:                           return NULL;
  }
}

template <class T>
static void set_param (CoreEntityHandle handle,CoreEntityParam param,const T& value)
{
  const char* pname = GetParamName (param);  
  
  if (!CoreIsValidEntity (handle))
  {
    logPrintf ("Try to set param '%s' to invalid entity (entity_handle=%p)",pname?pname:"__unknown__",handle);
    return;
  }
  
  if (!pname)
  {
    logPrintf ("Try to set invalid param %d to entity '%s'",param,get_entity (handle)->name ());
    return ;
  }  
  
  char param_name [256] = "__unknown__";  
  
  SetSilentMode (true);    

  __try
  {        
    Entity* entity = get_entity (handle);        

    _snprintf (param_name,sizeof (param_name),"%s.%s",entity->name (),pname);
        
    Command::instance ().etc ().set (param_name,value);    
//    logPrintf ("CoreSetEntityParam: '%s'='%s'",param_name,Command::instance ().etc ().gets (param_name));
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreSetEntityParam '%s' (entity_handle=%p)",param_name,handle);
    FatalErrorHandler ();
  }

  SetSilentMode (false);     
  PostRepaint ();
}

template <class T>
static T _get_param (const char* name,T*)
{
  return T (0);
}

static const char* _get_param (const char* name,const char**)
{
  return Command::instance ().etc ().gets (name);
}

static int _get_param (const char* name,int*)
{
  return Command::instance ().etc ().geti (name);
}

static float _get_param (const char* name,float*)
{
  return Command::instance ().etc ().getf (name);
}

static vec3f _get_param (const char* name,vec3f*)
{
  return Command::instance ().etc ().get3f (name);
}

template <class T>
static T get_param (CoreEntityHandle handle,CoreEntityParam param)
{
  const char* pname = GetParamName (param);  

  if (!CoreIsValidEntity (handle))
  {
    logPrintf ("Try to get param '%s' from invalid entity (entity_handle=%p)",pname?pname:"__unknown__",handle);
    return T (0);
  }
  
  if (!pname)
  {
    logPrintf ("Try to get invalid param %d from entity '%s'",param,get_entity (handle)->name ());
    return T (0);
  }  

  Entity* entity = get_entity (handle);
      
  char param_name [256] = "__unknown__";
  
  SetSilentMode (true);      
  
  __try
  {
    _snprintf (param_name,sizeof (param_name),"%s.%s",entity->name (),pname);
      
    T value = _get_param (param_name,(T*)NULL);    
    
//    logPrintf ("CoreGetEntityParam: '%s'='%s'",param_name,Command::instance ().etc ().gets (param_name));
    
    SetSilentMode (false);        
    
    return value;
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreGetEntityParam '%s' (entity_handle=%p)",param_name,handle);
    FatalErrorHandler ();
    SetSilentMode (false);        
    return T (0);
  }
  
  SetSilentMode (false);
  
//  PostRepaint ();
}

void CoreSetEntityParams (CoreEntityHandle handle,CoreEntityParam param,const char* value)
{
  set_param (handle,param,value);
}

void CoreSetEntityParamf (CoreEntityHandle handle,CoreEntityParam param,float value)
{
  set_param (handle,param,value);
}

void CoreSetEntityParami (CoreEntityHandle handle,CoreEntityParam param,int value)
{
  set_param (handle,param,value);
}

void CoreSetEntityParam3fv (CoreEntityHandle handle,CoreEntityParam param,const float* value)
{
  vec3f v (value [0],value [1],value [2]);
  
  set_param (handle,param,v);
}

const char* CoreGetEntityParams (CoreEntityHandle handle,CoreEntityParam param)
{
  return get_param<const char*> (handle,param);
}

float CoreGetEntityParamf (CoreEntityHandle handle,CoreEntityParam param)
{
  return get_param<float> (handle,param);
}

int CoreGetEntityParami (CoreEntityHandle handle,CoreEntityParam param)
{
  return get_param<int> (handle,param);
}

void CoreGetEntityParam3fv (CoreEntityHandle handle,CoreEntityParam param,float* res)
{ 
  vec3f v = get_param<vec3f> (handle,param);
  
  for (size_t i=0;i<3;i++)
    res [i] = v [i];
}

void CoreEntityXSelect (CoreEntityHandle handle)
{
  if (!CoreIsValidEntity (handle))
  {
    logPrintf ("Try to select invalid entity (entity_handle=%p)",handle);
    return;
  }
  
  char name_buf [256] = "__unknown__";
  
  SetSilentMode (true);      

  __try
  {
    Entity* entity = get_entity (handle);
    
    strncpy (name_buf,entity->name (),sizeof (name_buf));
    
//    logPrintf ("CoreEntityXSelect for entity '%s' doesn't work!",name_buf);
    
    entity->select (ENTITY_EXCLUSIVE_SELECT);    
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreEntityXSelect '%s' (entity_handle=%p)",name_buf,handle);
    FatalErrorHandler ();
  }
  
  SetSilentMode (false);
  
  PostRepaint ();  
} 

size_t CoreGetSelectedEntitiesCount ()
{
  __try
  {
    return selection.size ();
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreGetSelectedEntitiesCount");
    FatalErrorHandler ();    
  }  
  
  return 0;
}

CoreEntityHandle CoreGetSelectedEntity (size_t index)
{
  __try
  {
    if (!frame_control || index >= selection.size ())
      return 0;
      
    return selection [index];
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreGetSelectedEntity %u",index);
    FatalErrorHandler ();        
  }  
  
  return 0;
}

void CoreChangeManipulator (const char* name)
{
  if (!name || !on_change_manipulator)
    return;

  on_change_manipulator (name);      
}

void CoreSetManipulatorCallback (core_change_manipulator_func onChange)
{
  on_change_manipulator = onChange;
}

/*
    Вспомогательные функции загрузки/сохранения
*/

static char cur_filename [256] = "";

string CoreGetOpenFilename (const char* dir)
{
  if (!render_window)
    return "";
    
  if (!dir || !*dir)
    dir = ".";        
  
  static const TCHAR* filter = _T("Scene file\0*.wxf\0");

  OPENFILENAME ofn;

  memset (&ofn,0,sizeof (ofn));

  ofn.lStructSize = sizeof (OPENFILENAME);
  ofn.hwndOwner   = render_window;
  ofn.hInstance   = instance;
  ofn.lpstrFilter = filter;
  ofn.lpstrFile   = cur_filename;
  ofn.nMaxFile    = sizeof (cur_filename);
  ofn.lpstrInitialDir = dir;
  ofn.lpstrDefExt = "wxf";

  return GetOpenFileName (&ofn) ? cur_filename : "";
}

string CoreGetSaveAsFilename (const char* dir)
{
  if (!render_window)
    return "";
    
  if (!dir || !*dir)
    dir = ".";    
    
  static const TCHAR* filter = _T("Scene file\0*.wxf\0");

  OPENFILENAME ofn;

  memset (&ofn,0,sizeof (ofn));

  ofn.lStructSize = sizeof (OPENFILENAME);
  ofn.hwndOwner   = render_window;
  ofn.hInstance   = instance;
  ofn.lpstrFilter = filter;
  ofn.lpstrFile   = cur_filename;
  ofn.nMaxFile    = sizeof (cur_filename);
  ofn.Flags       = OFN_OVERWRITEPROMPT;
  ofn.lpstrInitialDir = dir;
  ofn.lpstrDefExt = "wxf";

  return GetSaveFileName (&ofn) ? cur_filename : "";
}

string CoreGetSaveFilename ()
{
  return cur_filename;
}

void CoreSetCurrentFilename (const char* name)
{
  if (!name)
    name = "";
    
  __try
  {
    strncpy (cur_filename,name,sizeof (cur_filename));    
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    logPrintf ("Fatal error at CoreSetCurrentFilename");
    FatalErrorHandler ();
    *cur_filename = 0;
  }
}

const char* CoreGetCurrentSceneFileName ()
{
  return cur_filename;
}

void CoreShowHelp ()
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

  CreateProcess (NULL,"hh coolwork.chm",NULL,NULL,FALSE,0,NULL,".",&si,&pi);
}
