#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <X11/keysym.h>

#include "glx.h" 
#include "gl.h"
#include "ui.h"


//#include <sys/ipc.h>
//#include <sys/shm.h>
//#include <X11/extensions/XShm.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>

#include "zgl.h"


#ifndef M_PI
#  define M_PI 3.14159265
#endif

//#include "c24bitmap.h"
//#include "c256bitmap.h"



static int attributeList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, NULL };


//Display *dpy;
//Window win;
void glXSwapBuffers( );

void tkSwapBuffers()
{
    glXSwapBuffers();
}

 bool glXMakeDrawOk(    GLXContext ctx1, int width, int height)
{
  
  TinyGLXContext *ctx = (TinyGLXContext *) ctx1;
   
  int i,xsize,ysize;
  unsigned int palette[ZB_NB_COLORS];
  unsigned char color_indexes[ZB_NB_COLORS];
  ZBuffer *zb;
  //XColor xcolor;
  unsigned long pixel[ZB_NB_COLORS],tmp_plane;
  
  if (ctx->gl_context == NULL) {
    // create the TinyGL context 

   // ctx->display=dpy;
    //ctx->drawable=drawable;

    //XGetWindowAttributes(ctx->display,drawable,&attr);
    
	
    xsize= width;
    ysize= height;

    //if (attr.depth != ctx->visual_info.depth) return False;

    // ximage structure 
    //ctx->ximage=NULL;
    ctx->shm_use=1; 

     {
        int mode,bpp;
        // RGB 16/24/32 
        bpp = 24;//bits_per_pixel(ctx->display,&ctx->visual_info);
        switch(bpp) {
        case 24:
            mode = ZB_MODE_RGB24;
            ctx->do_convert = (TGL_FEATURE_RENDER_BITS != 16);
            break;
         
        }
        zb=ZB_open(xsize,ysize,mode,0,NULL,NULL,NULL);
        if (zb == NULL) {
            fprintf(stderr, "Error while initializing Z buffer\n");
            exit(1);
        }
    }

    // create a gc 
    //ctx->gc = XCreateGC(ctx->display, drawable, 0, 0);
    ctx->image_w = width;
	ctx->image_h = height;
    // initialisation of the TinyGL interpreter 
    glInit(zb);
    ctx->gl_context=gl_get_context();
    ctx->gl_context->opaque=(void *) ctx;
    ctx->gl_context->gl_resize_viewport= NULL;//glX_resize_viewport;

    // set the viewport : we force a call to glX_resize_viewport 
    ctx->gl_context->viewport.xsize=-1;
    ctx->gl_context->viewport.ysize=-1;

    glViewport(0, 0, xsize, ysize);
  }

  return true;
}

/* we assume here that drawable is a window */
/*Bool glXMakeDrawOk( Display *dpy, GLXDrawable drawable,
                     GLXContext ctx1, int width, int height)
{
  
  TinyGLXContext *ctx = (TinyGLXContext *) ctx1;
  XWindowAttributes attr;
  int i,xsize,ysize;
  unsigned int palette[ZB_NB_COLORS];
  unsigned char color_indexes[ZB_NB_COLORS];
  ZBuffer *zb;
  XColor xcolor;
  unsigned long pixel[ZB_NB_COLORS],tmp_plane;
  
  if (ctx->gl_context == NULL) {
    // create the TinyGL context 

    ctx->display=dpy;
    ctx->drawable=drawable;

    //XGetWindowAttributes(ctx->display,drawable,&attr);
    attr.width  = width ;
	attr.height = height;
	
    xsize=attr.width;
    ysize=attr.height;

    //if (attr.depth != ctx->visual_info.depth) return False;

    // ximage structure 
    ctx->ximage=NULL;
    ctx->shm_use=1; 

     {
        int mode,bpp;
        // RGB 16/24/32 
        bpp = 24;//bits_per_pixel(ctx->display,&ctx->visual_info);
        switch(bpp) {
        case 24:
            mode = ZB_MODE_RGB24;
            ctx->do_convert = (TGL_FEATURE_RENDER_BITS != 16);
            break;
        case 32:
            mode = ZB_MODE_RGBA;
            ctx->do_convert = (TGL_FEATURE_RENDER_BITS != 16);
            break;
        default:
            mode = ZB_MODE_5R6G5B;
            ctx->do_convert = (TGL_FEATURE_RENDER_BITS != 16);
            break;
        }
        zb=ZB_open(xsize,ysize,mode,0,NULL,NULL,NULL);
        if (zb == NULL) {
            fprintf(stderr, "Error while initializing Z buffer\n");
            exit(1);
        }
    }

    // create a gc 
    //ctx->gc = XCreateGC(ctx->display, drawable, 0, 0);
    ctx->image_w = width;
	ctx->image_h = height;
    // initialisation of the TinyGL interpreter 
    glInit(zb);
    ctx->gl_context=gl_get_context();
    ctx->gl_context->opaque=(void *) ctx;
    ctx->gl_context->gl_resize_viewport= NULL;//glX_resize_viewport;

    // set the viewport : we force a call to glX_resize_viewport 
    ctx->gl_context->viewport.xsize=-1;
    ctx->gl_context->viewport.ysize=-1;

    glViewport(0, 0, xsize, ysize);
  }

  return True;
}*/

GLXContext glXCreateContext(  GLXContext shareList, bool direct );

int ui_loop(int argc, char **argv, const char *name)
{
 // XVisualInfo *vi;
  //Colormap cmap;
  //XSetWindowAttributes swa;
  //XSizeHints hint;
  GLXContext cx;
  //XEvent event;
  int k, width, height;
  char buf[80];
  //XEvent xev;
  //KeySym keysym;
  //XComposeStatus status;
  
   
 // dpy = NULL;//XOpenDisplay(NULL);
 // vi = NULL;//glXChooseVisual(dpy, DefaultScreen(dpy), attributeList);
      
   
  cx = glXCreateContext( 0, GL_TRUE);

  
  //cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),
  //			 vi->visual, AllocNone);

   
  width  = 1600;
  height = 1600;
  /*hint.x = 0;
  hint.y = 0;
  hint.width = width;
  hint.height = height;
  hint.flags = PPosition | PSize;
  swa.colormap = cmap;
  swa.border_pixel = 0;
  swa.event_mask = StructureNotifyMask;
  //win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, width, height,
//		      0, vi->depth, InputOutput, vi->visual,
	//	      CWBorderPixel|CWColormap|CWEventMask, &swa);
  //XSetStandardProperties (dpy, win, name, name, None, NULL, 0, &hint);

  //XMapWindow(dpy, win);
  //XIfEvent(dpy, &event, WaitForNotify, (char*)win);
  //XSelectInput(dpy, win, KeyPressMask | StructureNotifyMask | ExposureMask);

  /* connect the context to the window */
  //glXMakeCurrent(dpy, win, cx);
  
   glXMakeDrawOk(   cx,  width,  height);

   init();
   reshape(width, height);
  
  return 1;
  
   
}


