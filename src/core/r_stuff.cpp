#include "draw_stuff.h"

/*
    Вывод текста
*/

void rOutText (void* font,const char* format,...)
{
  va_list list;

  va_start  (list,format);  
  rVOutText (font,format,list);
}

void rVOutText (void* font,const char* format,va_list list)
{  
  char buf [128] = {0};

  _vsnprintf (buf,sizeof (buf),format,list);

  for (char* s=buf;*s;s++)
    glutBitmapCharacter (font,*s);
}

/*
    Вывод сетки
*/

void rDrawGrid (Viewport* viewport)
{
  glMatrixMode  (GL_PROJECTION);
  glLoadMatrixf (transpose (viewport->GetCamera ()->GetProjTM ())[0]);
  glMatrixMode  (GL_MODELVIEW);
//  glLoadMatrixf (transpose (viewport->GetCamera ()->GetWorldTM ())[0]);  
  glLoadMatrixf (transpose (invert (viewport->GetCamera ()->GetWorldTM ()))[0]);
  glDisable     (GL_DEPTH_TEST);
  glDisable     (GL_LIGHTING);  
  
  const vec3f  &pos   = viewport->GetGridPos (),
               &dir   = normalize (viewport->GetGridDir ()),
               &up    = equal (abs (dir),vec3f (0,1,0),0.1f) ? vec3f (0,0,1) : vec3f (0,1,0);
  float        width  = viewport->GetGridSize ().x / 2.0f,
               height = viewport->GetGridSize ().y / 2.0f;                                
                 
  gluLookAt (pos.x,pos.y,pos.z,pos.x+dir.x,pos.y+dir.y,pos.z+dir.z,up.x,up.y,up.z);
  
  glBegin (GL_LINES);
  
  glColor3f (0.6f,0.6f,0.6f);  
  
  float dy = 2.0f * height / float (viewport->GetGridSlices ()),
        dx = 2.0f * width  / float (viewport->GetGridSlices ());
  
  for (float y=-height;y<=height;y+=dy)
  {
    glVertex2f (-width,y);
    glVertex2f (width,y);
  }
  
  for (float x=-width;x<=width;x+=dx)
  {
    glVertex2f (x,-height);
    glVertex2f (x,height);
  }  
  
  glColor3f  (0,0,0);  
  glVertex2f (-width,0);
  glVertex2f (width,0);
  glVertex2f (0,-height);
  glVertex2f (0,height);  
     
  glEnd ();      
}

void rDrawPoint (const vec3f& pos,const vec3f& color)
{
  glPushAttrib (GL_ENABLE_BIT);
  glDisable    (GL_LIGHTING);
  glDisable    (GL_DEPTH_TEST);
  glColor3fv   (color);  
  glPointSize  (10);
  glBegin      (GL_POINTS);
  glVertex3fv  (pos);
  glEnd        ();
  glPopAttrib  ();
}

void rDrawArrow (const vec3f& pos,const vec3f& dir,const vec3f& color,const char* msg)
{
  if (!msg)
    msg = "";

  glPushAttrib (GL_ALL_ATTRIB_BITS);  
  glDisable    (GL_LIGHTING);  
  glDisable    (GL_DEPTH_TEST);
  glFrontFace  (GL_CW);
  glColor3fv   (color);  

  glBegin (GL_LINES);
    glVertex3fv (pos);
    glVertex3fv (pos+dir);
  glEnd ();
  
  vec3f ndir = normalize (dir),
        up   = equal (abs (ndir),vec3f (0,1,0),0.1f) ? vec3f (0,0,1) : vec3f (0,1,0);
  
  glPushMatrix  ();  
  glTranslatef  (pos.x+dir.x,pos.y+dir.y,pos.z+dir.z);
  gluLookAt     (0,0,0,ndir.x,ndir.y,-ndir.z,up.x,up.y,up.z);
  glutSolidCone (0.1,0.2,12,12);
  glRasterPos3f (0.1f,0,length (dir)*0.3f);
  rOutText      (GLUT_BITMAP_8_BY_13,msg);
  glPopMatrix   ();
  
  glPopAttrib ();
}

void rDrawAxes ()
{
  rDrawArrow (0.0f,vec3f (1,0,0),vec3f (1,0,0),"X");
  rDrawArrow (0.0f,vec3f (0,1,0),vec3f (0,1,0),"Y");
  rDrawArrow (0.0f,vec3f (0,0,1),vec3f (0,0,1),"Z");
}

void rDrawRect (const vec3f& pos,const vec3f& dir,const vec3f& color)
{
  glPushAttrib (GL_ALL_ATTRIB_BITS);
  glDisable    (GL_LIGHTING);
  glDisable    (GL_DEPTH_TEST);
  glEnable     (GL_BLEND);
  glDisable    (GL_CULL_FACE);
  glBlendFunc  (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glColor4f    (color.x,color.y,color.z,0.5f);
  glPushMatrix ();
  
  vec3f up = equal (normalize (abs (dir)),vec3f (0,1,0),0.01f) ? vec3f (0,0,1) : vec3f (0,1,0);
  
  gluLookAt    (pos.x,pos.y,pos.z,pos.x+dir.x,pos.y+dir.y,pos.z+dir.z,up.x,up.y,up.z);
  glRectf      (-0.5,-0.5,0.5,0.5);
  glPopMatrix  ();
  glPopAttrib  ();
}

void rDrawArc(const vec3f& pos,const vec3f& dir, const vec3f& color,int steps)
{
  glPushAttrib (GL_ENABLE_BIT);
  glDisable    (GL_LIGHTING);
  glDisable    (GL_DEPTH_TEST);
  glEnable     (GL_BLEND);
  glDisable    (GL_CULL_FACE);
  glFrontFace  (GL_CW);
  glBlendFunc  (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glColor4f    (color.x,color.y,color.z,0.5f);
	
  steps=steps>0?steps:1;
  float dif=(float)M_PI/steps,angle=0.0f;
  float sin0,cos0,sin1,cos1;

  glPushMatrix();

  vec3f up = equal (normalize (abs (dir)),vec3f (0,1,0),0.01f) ? vec3f (0,0,1) : vec3f (0,1,0);
  gluLookAt    (pos.x,pos.y,pos.z,pos.x+dir.x,pos.y+dir.y,pos.z+dir.z,up.x,up.y,up.z);
  
  glBegin (GL_LINES);
    for (int i=0;i<steps;angle+=dif,i++)
    {
      sin0=sin(angle);
      cos0=cos(angle);
      sin1=sin(angle+dif);
      cos1=cos(angle+dif);
      
      glVertex3f(cos0,0,sin0);
      glVertex3f(cos1,0,sin1);
      glVertex3f(-cos0,0,sin0);
      glVertex3f(-cos1,0,sin1);
    }
  glEnd ();
  
  glPopMatrix();
  glPopAttrib ();
}

void rDrawSector(const vec3f& pos,const vec3f& dir, const vec3f& color, int steps, float angle)
{
  glPushAttrib (GL_ENABLE_BIT);
  glDisable    (GL_LIGHTING);
  glDisable    (GL_DEPTH_TEST);
  glEnable     (GL_BLEND);
  glDisable    (GL_CULL_FACE);
  glFrontFace  (GL_CW);
  glBlendFunc  (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glColor4f    (color.x,color.y,color.z,0.5f);
	
  steps=steps>0?steps:1;
  float dif=deg2rad(angle)/steps;
  angle=0.0f;
  float sin0,cos0,sin1,cos1;

  glPushMatrix();

  vec3f up = equal (normalize (abs (dir)),vec3f (0,1,0),0.01f) ? vec3f (0,0,1) : vec3f (0,1,0);
  gluLookAt    (pos.x,pos.y,pos.z,pos.x+dir.x,pos.y+dir.y,pos.z+dir.z,up.x,up.y,up.z);
  
  glBegin (GL_TRIANGLES);
    for (int i=-steps/2;i<steps;angle+=dif,i++)
    {
      sin0=sin(angle);
      cos0=cos(angle);
      sin1=sin(angle+dif);
      cos1=cos(angle+dif);
      
      glVertex3f(pos.x, pos.y, pos.z);
      glVertex3f(cos0,0,sin0);
      glVertex3f(cos1,0,sin1);
    }
  glEnd ();
  
  glPopMatrix();
  glPopAttrib ();
}

void rDrawCircle(const vec3f& pos,const vec3f& dir, const vec3f& color,int steps) {
  glPushAttrib (GL_ENABLE_BIT);
  glDisable    (GL_LIGHTING);
  glDisable    (GL_DEPTH_TEST);
  glEnable     (GL_BLEND);
  glDisable    (GL_CULL_FACE);
  glFrontFace  (GL_CW);
  glBlendFunc  (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glColor4f    (color.x,color.y,color.z,0.5f);
	
  steps=steps>0?steps:1;
  float dif=(float)M_PI/steps,angle=0.0f;
  float sin0,cos0,sin1,cos1;

  glPushMatrix();

  vec3f up = equal (normalize (abs (dir)),vec3f (0,1,0),0.01f) ? vec3f (0,0,1) : vec3f (0,1,0);
  gluLookAt    (pos.x,pos.y,pos.z,pos.x+dir.x,pos.y+dir.y,pos.z+dir.z,up.x,up.y,up.z);
  
  glBegin (GL_LINES);
    for (int i=0;i<steps;angle+=dif,i++) 
    {
      sin0=sin(angle);
      cos0=cos(angle);
      sin1=sin(angle+dif);
      cos1=cos(angle+dif);

      glVertex3f(cos0,0,sin0);
      glVertex3f(cos1,0,sin1);
      glVertex3f(-cos0,0,sin0);
      glVertex3f(-cos1,0,sin1);
      glVertex3f(-cos0,0,-sin0);
      glVertex3f(-cos1,0,-sin1);
      glVertex3f(cos0,0,-sin0);
      glVertex3f(cos1,0,-sin1);
    }
  glEnd ();
  
  glPopMatrix();
  glPopAttrib ();
}

void rDrawLine (const vec3f& from,const vec3f& to,const vec3f& color)
{
  glPushAttrib (GL_ENABLE_BIT);
  glDisable    (GL_LIGHTING);
  glDisable    (GL_DEPTH_TEST);
  glColor3fv   (color);  

  glBegin (GL_LINES);
    glVertex3fv (from);
    glVertex3fv (to);
  glEnd ();  
    
  glPopAttrib ();
}

void rDrawBoundBox(const vec3f& from,const vec3f& to,const vec3f& color) {
  glPushAttrib (GL_ENABLE_BIT);
  glDisable    (GL_LIGHTING);
  //glDisable    (GL_DEPTH_TEST);
  glColor3fv   (color);
  const float part=0.8f;
  
  glBegin (GL_LINES);
  	glVertex3f(from.x,from.y,from.z);
  	glVertex3f(from.x*part+to.x*(1.0f-part),from.y,from.z);
  	glVertex3f(from.x*(1.0f-part)+to.x*part,from.y,from.z);
  	glVertex3f(to.x,from.y,from.z);

  	glVertex3f(from.x,from.y,from.z);
  	glVertex3f(from.x,from.y*part+to.y*(1.0f-part),from.z);
  	glVertex3f(from.x,from.y*(1.0f-part)+to.y*part,from.z);
  	glVertex3f(from.x,to.y,from.z);

  	glVertex3f(from.x,from.y,from.z);
  	glVertex3f(from.x,from.y,from.z*part+to.z*(1.0f-part));
  	glVertex3f(from.x,from.y,from.z*(1.0f-part)+to.z*part);
  	glVertex3f(from.x,from.y,to.z);
  	
  	glVertex3f(from.x,to.y,to.z);
  	glVertex3f(from.x*part+to.x*(1.0f-part),to.y,to.z);
  	glVertex3f(from.x*(1.0f-part)+to.x*part,to.y,to.z);
  	glVertex3f(to.x,to.y,to.z);

  	glVertex3f(to.x,from.y,to.z);
  	glVertex3f(to.x,from.y*part+to.y*(1.0f-part),to.z);
  	glVertex3f(to.x,from.y*(1.0f-part)+to.y*part,to.z);
  	glVertex3f(to.x,to.y,to.z);

  	glVertex3f(to.x,to.y,from.z);
  	glVertex3f(to.x,to.y,from.z*part+to.z*(1.0f-part));
  	glVertex3f(to.x,to.y,from.z*(1.0f-part)+to.z*part);
  	glVertex3f(to.x,to.y,to.z);
  	
  	glVertex3f(from.x,from.y,to.z);
  	glVertex3f(from.x,from.y*part+to.y*(1.0f-part),to.z);
  	glVertex3f(from.x,from.y*(1.0f-part)+to.y*part,to.z);
  	glVertex3f(from.x,to.y,to.z);

  	glVertex3f(from.x,from.y,to.z);
  	glVertex3f(from.x*part+to.x*(1.0f-part),from.y,to.z);
  	glVertex3f(from.x*(1.0f-part)+to.x*part,from.y,to.z);
  	glVertex3f(to.x,from.y,to.z);

  	glVertex3f(from.x,to.y,from.z);
  	glVertex3f(from.x,to.y,from.z*part+to.z*(1.0f-part));
  	glVertex3f(from.x,to.y,from.z*(1.0f-part)+to.z*part);
  	glVertex3f(from.x,to.y,to.z);

  	glVertex3f(from.x,to.y,from.z);
  	glVertex3f(from.x*part+to.x*(1.0f-part),to.y,from.z);
  	glVertex3f(from.x*(1.0f-part)+to.x*part,to.y,from.z);
  	glVertex3f(to.x,to.y,from.z);

  	glVertex3f(to.x,from.y,from.z);
  	glVertex3f(to.x,from.y*part+to.y*(1.0f-part),from.z);
  	glVertex3f(to.x,from.y*(1.0f-part)+to.y*part,from.z);
  	glVertex3f(to.x,to.y,from.z);

  	glVertex3f(to.x,from.y,from.z);
  	glVertex3f(to.x,from.y,from.z*part+to.z*(1.0f-part));
  	glVertex3f(to.x,from.y,from.z*(1.0f-part)+to.z*part);
  	glVertex3f(to.x,from.y,to.z);
  glEnd ();
    
  glPopAttrib ();
}

void rDrawBoundSphere(const vec3f& pos,float radius,const vec3f& color,int steps) {
  glPushAttrib (GL_ENABLE_BIT);
  glDisable    (GL_LIGHTING);
  glDisable    (GL_DEPTH_TEST);
  glColor3fv   (color);
	
  steps=steps>0?steps:1;
  float dif=(float)M_PI/steps,angle=0.0f;
  float sin0,cos0,sin1,cos1;

  glPushMatrix();
  glTranslatef(pos.x,pos.y,pos.z);
  glScalef(radius,radius,radius);
  
  glBegin (GL_LINES);
  	for (int i=0;i<steps;angle+=dif,i++) {
  		sin0=sin(angle);
  		cos0=cos(angle);
  		sin1=sin(angle+dif);
  		cos1=cos(angle+dif);
  		
  		glVertex3f(sin0,cos0,0);
  		glVertex3f(sin1,cos1,0);
  		glVertex3f(sin0,-cos0,0);
  		glVertex3f(sin1,-cos1,0);
  		glVertex3f(-sin0,-cos0,0);
  		glVertex3f(-sin1,-cos1,0);
  		glVertex3f(-sin0,cos0,0);
  		glVertex3f(-sin1,cos1,0);

  		glVertex3f(cos0,0,sin0);
  		glVertex3f(cos1,0,sin1);
  		glVertex3f(-cos0,0,sin0);
  		glVertex3f(-cos1,0,sin1);
  		glVertex3f(-cos0,0,-sin0);
  		glVertex3f(-cos1,0,-sin1);
  		glVertex3f(cos0,0,-sin0);
  		glVertex3f(cos1,0,-sin1);

  		glVertex3f(0,sin0,cos0);
  		glVertex3f(0,sin1,cos1);
  		glVertex3f(0,sin0,-cos0);
  		glVertex3f(0,sin1,-cos1);
  		glVertex3f(0,-sin0,-cos0);
  		glVertex3f(0,-sin1,-cos1);
  		glVertex3f(0,-sin0,cos0);
  		glVertex3f(0,-sin1,cos1);
  	}
  glEnd ();
  
  glPopMatrix();

  glPopAttrib ();
}
