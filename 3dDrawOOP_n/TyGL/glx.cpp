/* simple glx driver for TinyGL */
#include <glx.h>
//#include <sys/ipc.h>
//#include <sys/shm.h>
//#include <X11/extensions/XShm.h>
#include "zgl.h"

#include "c24bitmap.h"
#include "c256bitmap.h"


  


void glXSwapBuffers( )
{
  GLContext *gl_context;
 // TinyGLXContext *ctx;

  /* retrieve the current GLXContext */
  gl_context=gl_get_context();
 //ctx=(TinyGLXContext *)gl_context->opaque;
  //  GLContext *gl_context;


  C24BitMap  CPic;
  CPic.FormatF(gl_context->image_w, gl_context->image_h);
  ZBuffer * zb =  gl_context->zb;
  for(int i=0;i< gl_context->image_h ;i++)
     {
      char* ptr = (char*)zb->pbuf;
      ptr = &ptr[zb->linesize*i]; 
      for(int j=0; j<gl_context->image_w;j++)
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


 
