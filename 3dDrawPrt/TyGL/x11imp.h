#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

 
#include "gl.h"
#include "ui.h"

 

#include "zgl.h"


#ifndef M_PI
#  define M_PI 3.14159265
#endif

 
void glXSwapBuffers( );

void tkSwapBuffers()
{
    glXSwapBuffers();
}

 bool glXMakeDrawOk(  int width, int height)
{
  
  
 GLContext * gl_context=gl_get_context();
   
  ZBuffer *zb;
  
  
  if ( gl_context == NULL) {
    int mode(0);
   // int mode(0),bpp;
        // RGB 16/24/32 
    // bpp = 24;//bits_per_pixel(ctx->display,&ctx->visual_info);
 
        zb=ZB_open(width,height,mode,0,NULL,NULL,NULL);
        if (zb == NULL) {
            fprintf(stderr, "Error while initializing Z buffer\n");
            exit(1);
        }
      
    glInit(zb);

	gl_context=gl_get_context();
    gl_context->image_w = width;
	gl_context->image_h = height;
    gl_context->gl_resize_viewport= NULL;  
    gl_context->viewport.xsize=-1;
    gl_context->viewport.ysize=-1;

    glViewport(0, 0, width,height);
  }

  return true;
}

 
int ui_loop(int argc, char **argv, const char *name)
{
 
  int k, width, height;
  char buf[80];
   
   
  width  = 1600;
  height = 1600;
   
  
   glXMakeDrawOk(   width,  height);

   init();
   reshape(width, height);
  
  return 1;
  
   
}


