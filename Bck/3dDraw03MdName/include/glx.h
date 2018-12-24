#ifndef GLX_H
#define GLX_H

//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#include "gl.h"


/* the following comes from Mesa */
 


#define GLX_VERSION_1_1		1


/*
 * Tokens for glXChooseVisual and glXGetConfig:
 */
enum _GLX_CONFIGS {
	GLX_USE_GL		= 1,
	GLX_BUFFER_SIZE		= 2,
	GLX_LEVEL		= 3,
	GLX_RGBA		= 4,
	GLX_DOUBLEBUFFER	= 5, 
	GLX_STEREO		= 6,
	GLX_AUX_BUFFERS		= 7,
	GLX_RED_SIZE		= 8,
	GLX_GREEN_SIZE		= 9,
	GLX_BLUE_SIZE		= 10,
	GLX_ALPHA_SIZE		= 11,
	GLX_DEPTH_SIZE		= 12,
	GLX_STENCIL_SIZE	= 13,
	GLX_ACCUM_RED_SIZE	= 14,
	GLX_ACCUM_GREEN_SIZE	= 15,
	GLX_ACCUM_BLUE_SIZE	= 16,
	GLX_ACCUM_ALPHA_SIZE	= 17,

	/* GLX_EXT_visual_info extension */
	GLX_X_VISUAL_TYPE_EXT		= 0x22,
	GLX_TRANSPARENT_TYPE_EXT	= 0x23,
	GLX_TRANSPARENT_INDEX_VALUE_EXT	= 0x24,
	GLX_TRANSPARENT_RED_VALUE_EXT	= 0x25,
	GLX_TRANSPARENT_GREEN_VALUE_EXT	= 0x26,
	GLX_TRANSPARENT_BLUE_VALUE_EXT	= 0x27,
	GLX_TRANSPARENT_ALPHA_VALUE_EXT	= 0x28
};


/*
 * Error codes returned by glXGetConfig:
 */
#define GLX_BAD_SCREEN		1
#define GLX_BAD_ATTRIBUTE	2
#define GLX_NO_EXTENSION	3
#define GLX_BAD_VISUAL		4
#define GLX_BAD_CONTEXT		5
#define GLX_BAD_VALUE       	6
#define GLX_BAD_ENUM		7


/*
 * GLX 1.1 and later:
 */
#define GLX_VENDOR		1
#define GLX_VERSION		2
#define GLX_EXTENSIONS 		3


/*
 * GLX_visual_info extension
 */
#define GLX_TRUE_COLOR_EXT		0x8002
#define GLX_DIRECT_COLOR_EXT		0x8003
#define GLX_PSEUDO_COLOR_EXT		0x8004
#define GLX_STATIC_COLOR_EXT		0x8005
#define GLX_GRAY_SCALE_EXT		0x8006
#define GLX_STATIC_GRAY_EXT		0x8007
#define GLX_NONE_EXT			0x8000
#define GLX_TRANSPARENT_RGB_EXT		0x8008
#define GLX_TRANSPARENT_INDEX_EXT	0x8009


typedef void *GLXContext;
 

#endif
