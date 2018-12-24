#include "zgl.h"



#define FRAC_BITS 16

/* resizing with no interlating nor nearest pixel */

void gl_resizeImageNoInterpolate(unsigned char *dest,int xsize_dest,int ysize_dest,
                                 unsigned char *src,int xsize_src,int ysize_src)
{
  unsigned char *pix,*pix_src,*pix1;
  int x1,y1,x1inc,y1inc;
  int xi,yi,x,y;

  pix=dest;
  pix_src=src;
  
  x1inc=(int)((float) ((xsize_src)<<FRAC_BITS) / (float) (xsize_dest));
  y1inc=(int)((float) ((ysize_src)<<FRAC_BITS) / (float) (ysize_dest));

  y1=0;
  for(y=0;y<ysize_dest;y++) {
    x1=0;
    for(x=0;x<xsize_dest;x++) {
      xi=x1 >> FRAC_BITS;
      yi=y1 >> FRAC_BITS;
      pix1=pix_src+(yi*xsize_src+xi)*3;

      pix[0]=pix1[0];
      pix[1]=pix1[1];
      pix[2]=pix1[2];

      pix+=3;
      x1+=x1inc;
    }
    y1+=y1inc;
  }
}

