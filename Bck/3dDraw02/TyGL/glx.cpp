/* simple glx driver for TinyGL */
#include <glx.h>
//#include <sys/ipc.h>
//#include <sys/shm.h>
//#include <X11/extensions/XShm.h>
#include "zgl.h"

#include "c24bitmap.h"
#include "c256bitmap.h"


 

//Bool glXQueryExtension( Display *dpy, int *errorb, int *event )
//{
//    return True;
//}





GLXContext glXCreateContext( //Display *dpy, XVisualInfo *vis,
                             GLXContext shareList, bool direct )
{
  TinyGLXContext *ctx;

  if (shareList != NULL) {
    gl_fatal_error("No sharing available in TinyGL");
  }
  ctx=(TinyGLXContext *)gl_malloc(sizeof(TinyGLXContext));
  ctx->gl_context=NULL;
  //khdev ctx->visual_info=*vis;
  return (GLXContext) ctx;
}




void glXSwapBuffers( )
{
  GLContext *gl_context;
  TinyGLXContext *ctx;

  /* retrieve the current GLXContext */
  gl_context=gl_get_context();
  ctx=(TinyGLXContext *)gl_context->opaque;

  C24BitMap  CPic;
  CPic.FormatF(ctx->image_w, ctx->image_h);
  ZBuffer * zb = ctx->gl_context->zb;
  for(int i=0;i< ctx->image_h ;i++)
     {
      char* ptr = (char*)zb->pbuf;
      ptr = &ptr[zb->linesize*i]; 
      for(int j=0; j<ctx->image_w;j++)
      {
        C24PixVal Pix = get_pix_color(CPic, j, i);
		//ptr++;
        *Pix.r = *ptr; ptr++;      
        *Pix.g = *ptr; ptr++;
		*Pix.b = *ptr; ptr++;  		
      }
     }
  CPic.Save("img.bmp");
  return;
  /* for non 16 bits visuals, a conversion is required */
  
}


 
