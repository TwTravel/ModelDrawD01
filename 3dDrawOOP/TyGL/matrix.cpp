#include "zgl.h"

void gl_print_matrix( const float *m)
{
   int i;

   for (i=0;i<4;i++) {
      fprintf(stderr,"%f %f %f %f\n", m[i], m[4+i], m[8+i], m[12+i] );
   }
}

  void gl_matrix_update(GLContext *c)
{
  c->matrix_model_projection_updated=(c->matrix_mode<=1);
}


void glopMatrixMode(GLContext *c,GLParam *p)
{
 
}

void glopLoadMatrix(GLContext *c,GLParam *p)
{
 
}

void glopLoadIdentity(GLContext *c,GLParam *p)
{

  
}

void glopMultMatrix(GLContext *c,GLParam *p)
{
 
}


void glopPushMatrix(GLContext *c,GLParam *p)
{
  
}

void glopPopMatrix(GLContext *c,GLParam *p)
{
  
}


void glopRotate(GLContext *c,GLParam *p)
{
 
}

void glopScale(GLContext *c,GLParam *p)
{
  
}

void glopTranslate(GLContext *c,GLParam *p)
{
 
}


void glopFrustum(GLContext *c,GLParam *p)
{
  
}
  
