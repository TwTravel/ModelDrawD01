#include <stdlib.h>
#include "zbuffer.h"

#define ZCMP(z,zpix) ((z) >= (zpix))

 

/*
 * Smooth filled triangle.
 * The code below is very tricky :)
 */
void inline PUT_PIXELK(int _a, double &z, double &zz, register PIXEL *pp,double *pz,
                       register unsigned int&or1,register unsigned int&og1,register unsigned int&ob1,
					   int &dgdx,int&drdx,int&dbdx,int&dzdx)	
{
	 						 
    zz = z / 16384 ;//z >> ZB_POINT_Z_FRAC_BITS;		 
    if (ZCMP(zz,pz[_a])) {				 
      pp[3 * _a]=or1 >> 8; 
      pp[3 * _a + 1]=og1 >> 8; 
      pp[3 * _a + 2]=ob1 >> 8; 
      pz[_a]=zz;				 
    } 
    z+=dzdx;					 
    og1+=dgdx;					 
    or1+=drdx;					 
    ob1+=dbdx;					 
}
void ZB_fillTriangleSmooth(ZBuffer *zb,
			   ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{



ZBufferPoint *t,*pr1,*pr2,*l1,*l2;
  float fdx1, fdx2, fdy1, fdy2, fz, d1, d2;
  double *pz1;
  PIXEL *pp1;
  int part,update_left,update_right;

  int nb_lines,dx1,dy1,tmp,dx2,dy2;

  int error,derror;
  int x1,dxdy_min,dxdy_max;
/* warning: x2 is multiplied by 2^16 */
  int x2,dx2dy2;  

// INTERP_RGB INTERP_Z
  int z1,dzdx,dzdy,dzdl_min,dzdl_max;
 
  int r1,drdx,drdy,drdl_min,drdl_max;
  int g1,dgdx,dgdy,dgdl_min,dgdl_max;
  int b1,dbdx,dbdy,dbdl_min,dbdl_max;
 
 

  /* we sort the vertex with increasing y */
  if (p1->y < p0->y) {
    t = p0;
    p0 = p1;
    p1 = t;
  }
  if (p2->y < p0->y) {
    t = p2;
    p2 = p1;
    p1 = p0;
    p0 = t;
  } else if (p2->y < p1->y) {
    t = p1;
    p1 = p2;
    p2 = t;
  }

  /* we compute dXdx and dXdy for all interpolated values */
  
  fdx1 = p1->x - p0->x;
  fdy1 = p1->y - p0->y;

  fdx2 = p2->x - p0->x;
  fdy2 = p2->y - p0->y;

  fz = fdx1 * fdy2 - fdx2 * fdy1;
  if (fz == 0)
    return;
  fz = 1.0 / fz;

  fdx1 *= fz;
  fdy1 *= fz;
  fdx2 *= fz;
  fdy2 *= fz;

// INTERP_RGB INTERP_Z
  d1 = p1->z - p0->z;
  d2 = p2->z - p0->z;
  dzdx = (int) (fdy2 * d1 - fdy1 * d2);
  dzdy = (int) (fdx1 * d2 - fdx2 * d1);
 
  d1 = p1->r - p0->r;
  d2 = p2->r - p0->r;
  drdx = (int) (fdy2 * d1 - fdy1 * d2);
  drdy = (int) (fdx1 * d2 - fdx2 * d1);

  d1 = p1->g - p0->g;
  d2 = p2->g - p0->g;
  dgdx = (int) (fdy2 * d1 - fdy1 * d2);
  dgdy = (int) (fdx1 * d2 - fdx2 * d1);

  d1 = p1->b - p0->b;
  d2 = p2->b - p0->b;
  dbdx = (int) (fdy2 * d1 - fdy1 * d2);
  dbdy = (int) (fdx1 * d2 - fdx2 * d1);


  /* screen coordinates */

  pp1 = (PIXEL *) ((char *) zb->pbuf + zb->linesize * p0->y);
  pz1 = zb->zbuf + p0->y * zb->xsize;

  //DRAW_INIT();

  for(part=0;part<2;part++) {
    if (part == 0) {
      if (fz > 0) {
	update_left=1;
	update_right=1;
	l1=p0;
	l2=p2;
	pr1=p0;
	pr2=p1;
      } else {
	update_left=1;
	update_right=1;
	l1=p0;
	l2=p1;
	pr1=p0;
	pr2=p2;
      }
      nb_lines = p1->y - p0->y;
    } else {
      /* second part */
      if (fz > 0) {
	update_left=0;
	update_right=1;
	pr1=p1;
	pr2=p2;
      } else {
	update_left=1;
	update_right=0;
	l1=p1; 
	l2=p2;
      }
      nb_lines = p2->y - p1->y + 1;
    }

    /* compute the values for the left edge */

    if (update_left) {
      dy1 = l2->y - l1->y;
      dx1 = l2->x - l1->x;
      if (dy1 > 0) 
	tmp = (dx1 << 16) / dy1;
      else
	tmp = 0;
      x1 = l1->x;
      error = 0;
      derror = tmp & 0x0000ffff;
      dxdy_min = tmp >> 16;
      dxdy_max = dxdy_min + 1; 
// INTERP_RGB INTERP_Z
      z1=l1->z;
      dzdl_min=(dzdy + dzdx * dxdy_min); 
      dzdl_max=dzdl_min + dzdx;
 
      r1=l1->r;
      drdl_min=(drdy + drdx * dxdy_min);
      drdl_max=drdl_min + drdx;
      
      g1=l1->g;
      dgdl_min=(dgdy + dgdx * dxdy_min);
      dgdl_max=dgdl_min + dgdx;
      
      b1=l1->b;
      dbdl_min=(dbdy + dbdx * dxdy_min);
      dbdl_max=dbdl_min + dbdx;
 
 
    }

    /* compute values for the right edge */

    if (update_right) {
      dx2 = (pr2->x - pr1->x);
      dy2 = (pr2->y - pr1->y);
      if (dy2>0) 
	dx2dy2 = ( dx2 << 16) / dy2;
      else
	dx2dy2 = 0;
      x2 = pr1->x << 16;
    }

    /* we draw all the scan line of the part */

    while (nb_lines>0) {
      nb_lines--;
 
      {
          register PIXEL *pp;
          register int n;
 
          double *pz;
          double z,zz;
          register unsigned int or1,og1,ob1;

          n=(x2 >> 16) - x1;
          pp=(PIXEL *)((char *)pp1 + x1 * PSZB);
// INTERP_RGB INTERP_Z
          pz=pz1+x1;
          z=z1;
 
          or1 = r1;
          og1 = g1;
          ob1 = b1;
 
          
          while (n>=0) {
              PUT_PIXELK(0,z, zz, pp, pz, or1, og1, ob1,  dgdx, drdx, dbdx, dzdx);
//#ifdef INTERP_Z
              pz++;//=1;
			  
              pp=(PIXEL *)((char *)pp + PSZB);
              n-=1;
          }
      }
 
      error+=derror;
      if (error > 0) {
	error-=0x10000;
	x1+=dxdy_max;
// INTERP_RGB INTERP_Z
	z1+=dzdl_max;
   
	r1+=drdl_max;
	g1+=dgdl_max;
	b1+=dbdl_max;
 
 
      } else {
	x1+=dxdy_min;
// INTERP_RGB INTERP_Z
	z1+=dzdl_min;
 
	r1+=drdl_min;
	g1+=dgdl_min;
	b1+=dbdl_min;
 
 
      } 
      
      /* right edge */
      x2+=dx2dy2;

      /* screen coordinates */
      pp1=(PIXEL *)((char *)pp1 + zb->linesize);
      pz1+=zb->xsize;
    }
  }
  
 
}

void ZB_setTexture(ZBuffer *zb,PIXEL *texture)
{
    zb->current_texture=texture;
}

 

/*
 * Texture mapping with perspective correction.
 * We use the gradient method to make less divisions.
 * TODO: pipeline the division
 */
 

 void inline PUT_PIXEL_TEX(int _a, PIXEL *texture, double &z,
 double  &zz, register PIXEL *pp,double  *pz,
   register unsigned int&s, register unsigned int&t,
   int &dsdx,int&dtdx,int&dzdx)
{						 
   unsigned char *ptr; 
   zz = z /16384;//>> ZB_POINT_Z_FRAC_BITS;		 
     if (ZCMP(zz,pz[_a])) {				 
       ptr = texture + (((t & 0x3FC00000) | (s & 0x003FC000)) >> 14) * 3; 
       pp[3 * _a]= ptr[0]; 
       pp[3 * _a + 1]= ptr[1]; 
       pp[3 * _a + 2]= ptr[2]; 
       pz[_a]=zz;				 
    }						 
    z+=dzdx;					 
    s+=dsdx;					 
    t+=dtdx;					 
}


void ZB_fillTriangleMappingPerspective(ZBuffer *zb,
                            ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
    PIXEL *texture;
    float fdzdx,fndzdx,ndszdx,ndtzdx;

  

#define NB_INTERP 8


 
  
ZBufferPoint *t,*pr1,*pr2,*l1,*l2;
  float fdx1, fdx2, fdy1, fdy2, fz, d1, d2;
  double *pz1;
  PIXEL *pp1;
  int part,update_left,update_right;

  int nb_lines,dx1,dy1,tmp,dx2,dy2;

  int error,derror;
  int x1,dxdy_min,dxdy_max;
/* warning: x2 is multiplied by 2^16 */
  int x2,dx2dy2;  

// INTERP_Z

  int z1,dzdx,dzdy,dzdl_min,dzdl_max;
 
// INTERP_RGB
  int r1,drdx,drdy,drdl_min,drdl_max;
  int g1,dgdx,dgdy,dgdl_min,dgdl_max;
  int b1,dbdx,dbdy,dbdl_min,dbdl_max;
 
// INTERP_ST
  int s1,dsdx,dsdy,dsdl_min,dsdl_max;
  int t1,dtdx,dtdy,dtdl_min,dtdl_max;
 
//#ifdef INTERP_STZ
  float sz1,dszdx,dszdy,dszdl_min,dszdl_max;
  float tz1,dtzdx,dtzdy,dtzdl_min,dtzdl_max;
 

  /* we sort the vertex with increasing y */
  if (p1->y < p0->y) {
    t = p0;
    p0 = p1;
    p1 = t;
  }
  if (p2->y < p0->y) {
    t = p2;
    p2 = p1;
    p1 = p0;
    p0 = t;
  } else if (p2->y < p1->y) {
    t = p1;
    p1 = p2;
    p2 = t;
  }

  /* we compute dXdx and dXdy for all interpolated values */
  
  fdx1 = p1->x - p0->x;
  fdy1 = p1->y - p0->y;

  fdx2 = p2->x - p0->x;
  fdy2 = p2->y - p0->y;

  fz = fdx1 * fdy2 - fdx2 * fdy1;
  if (fz == 0)
    return;
  fz = 1.0 / fz;

  fdx1 *= fz;
  fdy1 *= fz;
  fdx2 *= fz;
  fdy2 *= fz;

 
  d1 = p1->z - p0->z;
  d2 = p2->z - p0->z;
  dzdx = (int) (fdy2 * d1 - fdy1 * d2);
  dzdy = (int) (fdx1 * d2 - fdx2 * d1);
 

 

//  INTERP_STZ
  {
    float zz;
    zz=(float) p0->z;
    p0->sz= (float) p0->s * zz;
    p0->tz= (float) p0->t * zz;
    zz=(float) p1->z;
    p1->sz= (float) p1->s * zz;
    p1->tz= (float) p1->t * zz;
    zz=(float) p2->z;
    p2->sz= (float) p2->s * zz;
    p2->tz= (float) p2->t * zz;

    d1 = p1->sz - p0->sz;
    d2 = p2->sz - p0->sz;
    dszdx = (fdy2 * d1 - fdy1 * d2);
    dszdy = (fdx1 * d2 - fdx2 * d1);
    
    d1 = p1->tz - p0->tz;
    d2 = p2->tz - p0->tz;
    dtzdx = (fdy2 * d1 - fdy1 * d2);
    dtzdy = (fdx1 * d2 - fdx2 * d1);
  }

  /* screen coordinates */

  pp1 = (PIXEL *) ((char *) zb->pbuf + zb->linesize * p0->y);
  pz1 = zb->zbuf + p0->y * zb->xsize;

  //DRAW_INIT();
 
 {						 
    texture=zb->current_texture; 
    fdzdx=(float)dzdx; 
    fndzdx=NB_INTERP * fdzdx; 
    ndszdx=NB_INTERP * dszdx; 
    ndtzdx=NB_INTERP * dtzdx; 
  }
  
  
  //===================================
  for(part=0;part<2;part++) {
    if (part == 0) {
      if (fz > 0) {
	update_left=1;
	update_right=1;
	l1=p0;
	l2=p2;
	pr1=p0;
	pr2=p1;
      } else {
	update_left=1;
	update_right=1;
	l1=p0;
	l2=p1;
	pr1=p0;
	pr2=p2;
      }
      nb_lines = p1->y - p0->y;
    } else {
      /* second part */
      if (fz > 0) {
	update_left=0;
	update_right=1;
	pr1=p1;
	pr2=p2;
      } else {
	update_left=1;
	update_right=0;
	l1=p1; 
	l2=p2;
      }
      nb_lines = p2->y - p1->y + 1;
    }

    /* compute the values for the left edge */

    if (update_left) {
      dy1 = l2->y - l1->y;
      dx1 = l2->x - l1->x;
      if (dy1 > 0) 
	tmp = (dx1 << 16) / dy1;
      else
	tmp = 0;
      x1 = l1->x;
      error = 0;
      derror = tmp & 0x0000ffff;
      dxdy_min = tmp >> 16;
      dxdy_max = dxdy_min + 1;
      
//#ifdef INTERP_Z
      z1=l1->z;
      dzdl_min=(dzdy + dzdx * dxdy_min); 
      dzdl_max=dzdl_min + dzdx;
 
 
// INTERP_STZ
      sz1=l1->sz;
      dszdl_min=(dszdy + dszdx * dxdy_min);
      dszdl_max=dszdl_min + dszdx;
      
      tz1=l1->tz;
      dtzdl_min=(dtzdy + dtzdx * dxdy_min);
      dtzdl_max=dtzdl_min + dtzdx;
 
    }

    /* compute values for the right edge */

    if (update_right) {
      dx2 = (pr2->x - pr1->x);
      dy2 = (pr2->y - pr1->y);
      if (dy2>0) 
	dx2dy2 = ( dx2 << 16) / dy2;
      else
	dx2dy2 = 0;
      x2 = pr1->x << 16;
    }

    /* we draw all the scan line of the part */

    while (nb_lines>0) {
      nb_lines--;
 
      //////////////////////DRAW_LINE();
	  double *pz;		 
  register PIXEL *pp;		 
  double  z,zz;	 
  unsigned int s,t;
  register int n,dsdx,dtdx;		 
  float sz,tz,fz,zinv;  
  n=(x2>>16)-x1;                              
  fz=(float)z1; 
  zinv=1.0 / fz; 
  pp=(PIXEL *)((char *)pp1 + x1 * PSZB);  
  pz=pz1+x1;					 
  z=z1;						 
  sz=sz1; 
  tz=tz1; 
  while (n>=(NB_INTERP-1)) {						    
    { 
      float ss,tt; 
      ss=(sz * zinv); 
      tt=(tz * zinv); 
      s=(int) ss; 
      t=(int) tt; 
      dsdx= (int)( (dszdx - ss*fdzdx)*zinv ); 
      dtdx= (int)( (dtzdx - tt*fdzdx)*zinv ); 
      fz+=fndzdx; 
      zinv=1.0 / fz; 
    } 
    PUT_PIXEL_TEX(0, texture,  z, zz, pp, pz, s,t, dsdx, dtdx, dzdx	);						    
    PUT_PIXEL_TEX(1, texture,  z, zz, pp, pz, s,t, dsdx, dtdx, dzdx	);								    
    PUT_PIXEL_TEX(2, texture,  z, zz, pp, pz, s,t, dsdx, dtdx, dzdx	);								    
    PUT_PIXEL_TEX(3, texture,  z, zz, pp, pz, s,t, dsdx, dtdx, dzdx	);								    
    PUT_PIXEL_TEX(4, texture,  z, zz, pp, pz, s,t, dsdx, dtdx, dzdx	);								    
    PUT_PIXEL_TEX(5, texture,  z, zz, pp, pz, s,t, dsdx, dtdx, dzdx	);								    
    PUT_PIXEL_TEX(6, texture,  z, zz, pp, pz, s,t, dsdx, dtdx, dzdx	);								    
    PUT_PIXEL_TEX(7, texture,  z, zz, pp, pz, s,t, dsdx, dtdx, dzdx	);								    
    pz+=NB_INTERP;							    
    pp=(PIXEL *)((char *)pp + NB_INTERP * PSZB); 
    n-=NB_INTERP;							    
    sz+=ndszdx; 
    tz+=ndtzdx; 
  }									    
    { 
      float ss,tt; 
      ss=(sz * zinv); 
      tt=(tz * zinv); 
      s=(int) ss;
      t=(int) tt;
      dsdx= (int)( (dszdx - ss*fdzdx)*zinv );
      dtdx= (int)( (dtzdx - tt*fdzdx)*zinv );
    } 
  while (n>=0) {							   
    PUT_PIXEL_TEX(0, texture,  z, zz, pp, pz, s,t, dsdx, dtdx, dzdx	);							   
    pz+=1;								   
    pp=(PIXEL *)((char *)pp + PSZB);
    n-=1;								   
  }									   
 
      //////////////////////////////////
      /* left edge */
      error+=derror;
      if (error > 0) {
	error-=0x10000;
	x1+=dxdy_max;
//  INTERP_Z
	z1+=dzdl_max;
      
 
//#ifdef INTERP_STZ
	sz1+=dszdl_max;
	tz1+=dtzdl_max;
 
      } else {
	x1+=dxdy_min;
//#ifdef INTERP_Z
	z1+=dzdl_min;
       
 
//#ifdef INTERP_STZ
	sz1+=dszdl_min;
	tz1+=dtzdl_min;
 
      } 
      
      /* right edge */
      x2+=dx2dy2;

      /* screen coordinates */
      pp1=(PIXEL *)((char *)pp1 + zb->linesize);
      pz1+=zb->xsize;
    }
  }
  
 
}

 

 
