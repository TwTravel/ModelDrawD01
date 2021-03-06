#include "zgl.h"
#include <stdio.h>
/* glVertex */

void glVertex4f(float x,float y,float z,float w)
{
  GLParam p[5];

  p[0].op=OP_Vertex;
  p[1].f=x;
  p[2].f=y;
  p[3].f=z;
  p[4].f=w;

  glRunFunc(p);
 // glopVertex(GLContext * c, GLParam * p)
}

void glVertex2f(float x,float y) 
{
  glVertex4f(x,y,0,1);
}

void glVertex3f(float x,float y,float z) 
{
  glVertex4f(x,y,z,1);
}

void glVertex3fv(float *v) 
{
  glVertex4f(v[0],v[1],v[2],1);
}

/* glNormal */

void glNormal3f(float x,float y,float z)
{
  GLParam p[4];

  p[0].op=OP_Normal;
  p[1].f=x;
  p[2].f=y;
  p[3].f=z;

  glRunFunc(p);
}

void glNormal3fv(float *v) 
{
  glNormal3f(v[0],v[1],v[2]);
}

/* glColor */

void glColor4f(float r,float g,float b,float a)
{
  GLParam p[8];

  p[0].op=OP_Color;
  p[1].f=r;
  p[2].f=g;
  p[3].f=b;
  p[4].f=a;
  /* direct convertion to integer to go faster if no shading */
  p[5].ui = (unsigned int) (r * (ZB_POINT_RED_MAX - ZB_POINT_RED_MIN) + 
                            ZB_POINT_RED_MIN);
  p[6].ui = (unsigned int) (g * (ZB_POINT_GREEN_MAX - ZB_POINT_GREEN_MIN) + 
                            ZB_POINT_GREEN_MIN);
  p[7].ui = (unsigned int) (b * (ZB_POINT_BLUE_MAX - ZB_POINT_BLUE_MIN) + 
                            ZB_POINT_BLUE_MIN);
  glRunFunc(p);
}

void glColor4fv(float *v)
{
  GLParam p[8];

  p[0].op=OP_Color;
  p[1].f=v[0];
  p[2].f=v[1];
  p[3].f=v[2];
  p[4].f=v[3];
  /* direct convertion to integer to go faster if no shading */
  p[5].ui = (unsigned int) (v[0] * (ZB_POINT_RED_MAX - ZB_POINT_RED_MIN) + 
                            ZB_POINT_RED_MIN);
  p[6].ui = (unsigned int) (v[1] * (ZB_POINT_GREEN_MAX - ZB_POINT_GREEN_MIN) + 
                            ZB_POINT_GREEN_MIN);
  p[7].ui = (unsigned int) (v[2] * (ZB_POINT_BLUE_MAX - ZB_POINT_BLUE_MIN) + 
                            ZB_POINT_BLUE_MIN);
  glRunFunc(p);
}

void glColor3f(float x,float y,float z) 
{
  glColor4f(x,y,z,1);
}

void glColor3fv(float *v) 
{
  glColor4f(v[0],v[1],v[2],1);
}


/* TexCoord */

void glTexCoord4f(float s,float t,float r,float q)
{
  GLParam p[5];

  p[0].op=OP_TexCoord;
  p[1].f=s;
  p[2].f=t;
  p[3].f=r;
  p[4].f=q;

  glRunFunc(p);
}

void glTexCoord2f(float s,float t)
{
  glTexCoord4f(s,t,0,1);
}

void glTexCoord2fv(float *v)
{
  glTexCoord4f(v[0],v[1],0,1);
}

void glEdgeFlag(int flag)
{
  GLParam p[2];

  p[0].op=OP_EdgeFlag;
  p[1].i=flag;

  glRunFunc(p);
}

/* misc */

   

/* glEnable / glDisable */

void glEnable(int cap)
{
  GLParam p[3];

  p[0].op=OP_EnableDisable;
  p[1].i=cap;
  p[2].i=1;

  glRunFunc(p);
}

void glDisable(int cap)
{
  GLParam p[3];

  p[0].op=OP_EnableDisable;
  p[1].i=cap;
  p[2].i=0;

  glRunFunc(p);
}

/* glBegin / glEnd */

void glBegin(int mode)
{
  GLParam p[2];

  p[0].op=OP_Begin;
  p[1].i=mode;

  glRunFunc(p);
}

void glEnd(void)
{
  GLParam p[1];

  p[0].op=OP_End;

  glRunFunc(p);
}

/* matrix */

void glMatrixMode(int mode)
{
  GLParam p[2];

  p[0].op=OP_MatrixMode;
  p[1].i=mode;

  glRunFunc(p);
}

void glLoadMatrixf(const float *m)
{
  GLParam p[17];
  int i;

  p[0].op=OP_LoadMatrix;
  for(i=0;i<16;i++) p[i+1].f=m[i];

  glRunFunc(p);
}

void glLoadIdentity(void)
{
  GLParam p[1];

  p[0].op=OP_LoadIdentity;

  glRunFunc(p);
}

void glMultMatrixf(const float *m)
{
  GLParam p[17];
  int i;

  p[0].op=OP_MultMatrix;
  for(i=0;i<16;i++) p[i+1].f=m[i];

  glRunFunc(p);
}

void glPushMatrix(void)
{
  GLParam p[1];

  p[0].op=OP_PushMatrix;

  glRunFunc(p);
}

void glPopMatrix(void)
{
  GLParam p[1];

  p[0].op=OP_PopMatrix;

  glRunFunc(p);
}

void glRotatef(float angle,float x,float y,float z)
{
  GLParam p[5];

  p[0].op=OP_Rotate;
  p[1].f=angle;
  p[2].f=x;
  p[3].f=y;
  p[4].f=z;

  glRunFunc(p);
}

void glTranslatef(float x,float y,float z)
{
  GLParam p[4];

  p[0].op=OP_Translate;
  p[1].f=x;
  p[2].f=y;
  p[3].f=z;

  glRunFunc(p);
}

void glScalef(float x,float y,float z)
{
  GLParam p[4];

  p[0].op=OP_Scale;
  p[1].f=x;
  p[2].f=y;
  p[3].f=z;

  glRunFunc(p);
}


void glViewport(int x,int y,int width,int height)
{
  GLParam p[5];

  p[0].op=OP_Viewport;
  p[1].i=x;
  p[2].i=y;
  p[3].i=width;
  p[4].i=height;

  glRunFunc(p);
}

void glFrustum(double left,double right,double bottom,double top,
               double near,double farv)
{
  GLParam p[7];

  p[0].op=OP_Frustum;
  p[1].f=left;
  p[2].f=right;
  p[3].f=bottom;
  p[4].f=top;
  p[5].f=near;
  p[6].f=farv;

  glRunFunc(p);
}

/* lightening */

void glMaterialfv(int mode,int type,float *v)
{
  GLParam p[7];
  int i,n;

  assert(mode == GL_FRONT  || mode == GL_BACK || mode==GL_FRONT_AND_BACK);

  p[0].op=OP_Material;
  p[1].i=mode;
  p[2].i=type;
  n=4;
  if (type == GL_SHININESS) n=1;
  for(i=0;i<4;i++) p[3+i].f=v[i];
  for(i=n;i<4;i++) p[3+i].f=0;

  glRunFunc(p);
}

void glMaterialf(int mode,int type,float v)
{
  GLParam p[7];
  int i;

  p[0].op=OP_Material;
  p[1].i=mode;
  p[2].i=type;
  p[3].f=v;
  for(i=0;i<3;i++) p[4+i].f=0;

  glRunFunc(p);
}

void glColorMaterial(int mode,int type)
{
  GLParam p[3];

  p[0].op=OP_ColorMaterial;
  p[1].i=mode;
  p[2].i=type;

  glRunFunc(p);
}

void glLightfv(int light,int type,float *v)
{
  GLParam p[7];
  int i;

  p[0].op=OP_Light;
  p[1].i=light;
  p[2].i=type;
  /* TODO: 3 composants ? */
  for(i=0;i<4;i++) p[3+i].f=v[i];

  glRunFunc(p);
}


void glLightf(int light,int type,float v)
{
  GLParam p[7];
  int i;

  p[0].op=OP_Light;
  p[1].i=light;
  p[2].i=type;
  p[3].f=v;
  for(i=0;i<3;i++) p[4+i].f=0;

  glRunFunc(p);
}

void glLightModeli(int pname,int param)
{
  GLParam p[6];
  int i;

  p[0].op=OP_LightModel;
  p[1].i=pname;
  p[2].f=(float)param;
  for(i=0;i<4;i++) p[3+i].f=0;

  glRunFunc(p);
}

void glLightModelfv(int pname,float *param)
{
  GLParam p[6];
  int i;

  p[0].op=OP_LightModel;
  p[1].i=pname;
  for(i=0;i<4;i++) p[2+i].f=param[i];

  glRunFunc(p);
}

/* clear */

 

 
/* textures */

void glTexImage2D( int target, int level, int components,
                   int width, int height, int border,
                   int format, int type, void *pixels)
{
  GLParam p[10];

  p[0].op=OP_TexImage2D;
  p[1].i=target;
  p[2].i=level;
  p[3].i=components;
  p[4].i=width;
  p[5].i=height;
  p[6].i=border;
  p[7].i=format;
  p[8].i=type;
  p[9].p=pixels;

  glRunFunc(p);
}


void glBindTexture(int target,int texture)
{
  GLParam p[3];

  p[0].op=OP_BindTexture;
  p[1].i=target;
  p[2].i=texture;

  glRunFunc(p);
}

void glTexEnvi(int target,int pname,int param)
{
  GLParam p[8];
  
  p[0].op=OP_TexEnv;
  p[1].i=target;
  p[2].i=pname;
  p[3].i=param;
  p[4].f=0;
  p[5].f=0;
  p[6].f=0;
  p[7].f=0;

  glRunFunc(p);
}

void glTexParameteri(int target,int pname,int param)
{
  GLParam p[8];
  
  p[0].op=OP_TexParameter;
  p[1].i=target;
  p[2].i=pname;
  p[3].i=param;
  p[4].f=0;
  p[5].f=0;
  p[6].f=0;
  p[7].f=0;

  glRunFunc(p);
}

void glPixelStorei(int pname,int param)
{
  GLParam p[3];

  p[0].op=OP_PixelStore;
  p[1].i=pname;
  p[2].i=param;

  glRunFunc(p);
}

/* selection */



/* Special Functions */

void glCallList(unsigned int list)
{
  GLParam p[2];

  p[0].op=OP_CallList;
  p[1].i=list;

  glRunFunc(p);
}

void glFlush(void)
{
  /* nothing to do */
}

 

/* Non standard functions */

void glDebug(int mode)
{
  GLContext *c=gl_get_context();
  c->print_flag=mode;
}

