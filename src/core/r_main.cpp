#include <algorithm>
#include "draw_stuff.h"
#include "misc.h"

using namespace std;

const size_t BORDER_SIZE = 6;

struct RenderImpl {                           //описание реализации рендера
  HWND         wnd;                           //дескриптор окна отрисовки
  HDC          dc;                            //дескриптор контекста отрисовки
  HGLRC        context;                       //контекст отрисовки
  size_t       ref_count;                     //счётчик ссылок
  Viewport     vp [Render::VIEWPORTS_COUNT];  //области вывода
  Rect         window_rect;                   //границы окна
  float        split_x, split_y;              //координаты точки разделения областей просмотра
  Viewport*    active_viewport;               //активная область вывода  
  ViewportMode viewport_mode;                 //режим вывода в один вьюпорт
};

Render::Render ()
{
  impl = new RenderImpl;
  
  impl->wnd     = NULL;
  impl->context = NULL;
  impl->dc      = NULL;
    
  impl->window_rect.set (0,0,2,2);
  
  impl->vp [0].resize (0,0,1,1);
  impl->vp [1].resize (1,0,2,1);
  impl->vp [2].resize (0,1,1,2);  
  impl->vp [2].resize (1,1,2,2);
  
  impl->split_x = impl->split_y = 0.5f;
  impl->viewport_mode = RENDER_SINGLE_VIEWPORT_MODE;

  for (int i=0;i<4;i++)
    impl->vp [i].SetRender (this);
    
  impl->active_viewport = NULL;
  
  SetActiveViewport (impl->vp+1);
}

Render::~Render ()
{
  SetWindow (NULL);

  delete impl;
}

void Render::addref ()
{
  impl->ref_count++;
}

void Render::release ()
{
  if (!--impl->ref_count)
    delete this;
}

bool Render::SetWindow (void* wnd_handle)
{
  if (impl->wnd)
  {
    if (impl->context == wglGetCurrentContext ())
      wglMakeCurrent (0,0);

    wglDeleteContext (impl->context);
    ReleaseDC        (impl->wnd,impl->dc);
    
    impl->wnd     = NULL;
    impl->context = NULL;
    impl->dc      = NULL;
  }
  
  if (!wnd_handle)
    return false;

  impl->wnd = (HWND)wnd_handle;
  
  if (!impl->wnd)
  {
    logPrintf ("Render::SetWindow: Null window");
    return false;
  }
  
  PIXELFORMATDESCRIPTOR pfd;
  int pFormat = -1;

  memset (&pfd,0,sizeof (PIXELFORMATDESCRIPTOR));

  pfd.nSize        = sizeof (PIXELFORMATDESCRIPTOR);
  pfd.nVersion     = 1;
  pfd.dwFlags      = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
  pfd.iPixelType   = PFD_TYPE_RGBA;
  pfd.cColorBits   = 16;
  pfd.cDepthBits   = 16;
  pfd.iLayerType   = PFD_MAIN_PLANE;
  
  impl->dc = GetDC (impl->wnd);
  
  if (!impl->dc)
  {
    logPrintf ("Render::SetWindow: Error at GetDC(%p)",impl->wnd);
    impl->wnd = NULL;
    return false;
  } 

  pFormat = ChoosePixelFormat (impl->dc,&pfd);        
 
  if (!pFormat) 
  {
    logPrintf ("Render::SetWindow: Error at ChoosePixelFormat");
    ReleaseDC (impl->wnd,impl->dc);    
    impl->wnd  = NULL;
    impl->dc   = NULL;
    return false;
  }
  
  if (!SetPixelFormat (impl->dc,pFormat,&pfd))
  {
    logPrintf ("RenderInit: Error at SetPixelFormat");
    ReleaseDC (impl->wnd,impl->dc);    
    impl->wnd  = NULL;
    impl->dc   = NULL;
    return false;
  }

  impl->context = wglCreateContext (impl->dc);
  
  if (!impl->context)
  {
    logPrintf ("RenderInit: Error at wglCreateContext");
    ReleaseDC (impl->wnd,impl->dc);    
    impl->wnd  = NULL;
    impl->dc   = NULL;    
    return false;
  }  
  
  activate ();
  
  glewInit ();
  
  return true;
}

bool Render::activate ()
{
  if (wglGetCurrentContext () == impl->context && wglGetCurrentDC () == impl->dc && impl->dc && impl->context)
    return true;

  return wglMakeCurrent (impl->dc,impl->context) == TRUE;
}

Viewport* Render::viewport (size_t index)
{
  return index < VIEWPORTS_COUNT ? &impl->vp [index] : NULL;
}

Viewport* Render::FindViewport (const char* name)
{
  if (!name)
    return NULL;

  for (int i=0;i<VIEWPORTS_COUNT;i++)
    if (!strcmp (name,impl->vp [i].name ()))
      return impl->vp+i;

  return NULL;            
}
   
void Render::SetSplit (int x,int y)
{
  const Rect& wnd_rect = impl->window_rect;

  impl->split_x       = float (min (max (x,wnd_rect.left ()),wnd_rect.right ()))/float (wnd_rect.width ());
  impl->split_y       = float (min (max (y,wnd_rect.top ()),wnd_rect.bottom ()))/float (wnd_rect.height ());
  impl->viewport_mode = RENDER_MULTI_VIEWPORT_MODE;
  
  SetWindowRect (wnd_rect.left (),wnd_rect.top (),wnd_rect.width (),wnd_rect.height ());
}

void Render::SetWindowRect (int left,int top,int width,int height)
{  
  Rect& wnd_rect = impl->window_rect;

  wnd_rect.set (left,top,width,height);
  
  if (impl->viewport_mode == RENDER_MULTI_VIEWPORT_MODE)
  {
    const int HALF_BORDER_SIZE = BORDER_SIZE / 2;    
    
    int x = (int)(float (left)+impl->split_x*float (width)),
        y = (int)(float (top)+impl->split_y*float (height));

    impl->vp [0].resize (wnd_rect.left ()+BORDER_SIZE,wnd_rect.top ()+BORDER_SIZE,x-HALF_BORDER_SIZE,y-HALF_BORDER_SIZE);
    impl->vp [1].resize (x+HALF_BORDER_SIZE,wnd_rect.top ()+BORDER_SIZE,wnd_rect.right ()-BORDER_SIZE,y-HALF_BORDER_SIZE);
    impl->vp [2].resize (wnd_rect.left ()+BORDER_SIZE,y+HALF_BORDER_SIZE,x-HALF_BORDER_SIZE,wnd_rect.bottom ()-BORDER_SIZE);    
    impl->vp [3].resize (x+HALF_BORDER_SIZE,y+HALF_BORDER_SIZE,wnd_rect.right ()-BORDER_SIZE,wnd_rect.bottom ()-BORDER_SIZE);    
  }  
  else
  {
    Viewport* viewport = GetActiveViewport ();
    
    if (!viewport)
      return;
      
    for (size_t i=0;i<VIEWPORTS_COUNT;i++)
      impl->vp [i].resize (-10,-10,0,0);
      
    viewport->resize (wnd_rect.left ()+BORDER_SIZE,wnd_rect.top ()+BORDER_SIZE,wnd_rect.right ()-BORDER_SIZE,wnd_rect.bottom ()-BORDER_SIZE);  
  }    
} 

const Rect& Render::GetWindowRect ()
{
  return impl->window_rect;
}

Viewport* Render::FindViewport (int x,int y)
{
  for (int i=0;i<VIEWPORTS_COUNT;i++)
    if (impl->vp [i].IsInViewport (x,y))
      return impl->vp+i;
      
  return NULL;
}

void Render::SetActiveViewport (Viewport* vp)
{
  if (!vp || vp->GetRender () != this)
    return ;
    
  impl->active_viewport = vp;  
}

void Render::SetActiveViewport (int x,int y)
{
  Viewport* viewport = FindViewport (x,y);
  
  if (viewport)
    viewport->activate ();
}

Viewport* Render::GetActiveViewport ()
{
  return impl->active_viewport;
}

void Render::SetViewportMode (ViewportMode mode)
{
  if (mode == impl->viewport_mode)
    return;
        
  switch (mode)
  {
    default:
      mode = RENDER_MULTI_VIEWPORT_MODE;
    case RENDER_SINGLE_VIEWPORT_MODE:                      
    case RENDER_MULTI_VIEWPORT_MODE:   
      break;
  }
  
  impl->viewport_mode = mode;
  
  const Rect& wnd_rect = impl->window_rect;  
  
  SetWindowRect (wnd_rect.left (),wnd_rect.top (),wnd_rect.width (),wnd_rect.height ());
}

ViewportMode Render::GetViewportMode ()
{
  return impl->viewport_mode;
}

static void draw_wire_rect (float left,float top,float right,float bottom)
{
  glBegin (GL_LINE_LOOP);
    glVertex2f (left,top);
    glVertex2f (right,top);
    glVertex2f (right,bottom);
    glVertex2f (left,bottom);
  glEnd ();  
}
    
void Render::repaint ()
{
  if (!activate ())
    return;                
  
  const Rect& wnd_rect = impl->window_rect;
    
  glClearColor (0.5f,0.5f,0.5f,0.5f);  
  glClear      (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glHint       (GL_LINE_SMOOTH,GL_NICEST);
  glHint       (GL_POLYGON_SMOOTH,GL_NICEST);  
  glHint       (GL_POINT_SMOOTH,GL_NICEST);
  
  if (impl->viewport_mode == RENDER_MULTI_VIEWPORT_MODE)
  {
    for (int i=0;i<VIEWPORTS_COUNT;i++)
    {
      glPushAttrib   (GL_ALL_ATTRIB_BITS);
      glMatrixMode   (GL_PROJECTION);
      glLoadIdentity ();    
      glMatrixMode   (GL_MODELVIEW);  
      glLoadIdentity ();
      
      impl->vp [i].repaint ();
      
      glPopAttrib ();
    }    
  }
  else if (GetActiveViewport ())
  {
    glPushAttrib   (GL_ALL_ATTRIB_BITS);
    glMatrixMode   (GL_PROJECTION);
    glLoadIdentity ();    
    glMatrixMode   (GL_MODELVIEW);  
    glLoadIdentity ();
    
    GetActiveViewport ()->repaint ();
    
    glPopAttrib ();    
  }

  glEnable       (GL_POINT_SMOOTH);
  glEnable       (GL_LINE_SMOOTH);
  glDisable      (GL_LIGHTING);
  glDisable      (GL_BLEND);
  glDisable      (GL_ALPHA_TEST);
  glDisable      (GL_CULL_FACE);  
  glDisable      (GL_DEPTH_TEST);
  glPointSize    (5);  
  glMatrixMode   (GL_PROJECTION);
  glLoadIdentity ();    
  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();    
  glViewport     (wnd_rect.left (),wnd_rect.top (),wnd_rect.width (),wnd_rect.height ());
  glOrtho        (0,10,0,10,-1,1);
  glColor3f      (0,0,0);
  draw_wire_rect (0,0,10,10);

  glFinish ();    
  SwapBuffers (impl->dc);
}

/*
    Rect
*/

Rect::Rect ()
{
  set (0,0,0,0);
}

Rect::Rect (int x,int y,int width,int height)
{
  set (x,y,width,height);
}

void Rect::set (int x,int y,int width,int height)
{
  rect_left   = x;
  rect_top    = y;
  rect_right  = x + width;
  rect_bottom = y + height;
}

bool Rect::test (int x,int y) const
{
  return x >= rect_left && x < rect_right && y >= rect_top && y < rect_bottom;
}

/*
    Render::Create
*/

Render* Render::create ()
{
  return new Render;
}
