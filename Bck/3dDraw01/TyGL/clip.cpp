#include "zgl.h"

/* fill triangle profile */
/* #define PROFILE */

#define CLIP_XMIN   (1<<0)
#define CLIP_XMAX   (1<<1)
#define CLIP_YMIN   (1<<2)
#define CLIP_YMAX   (1<<3)
#define CLIP_ZMIN   (1<<4)
#define CLIP_ZMAX   (1<<5)

void gl_transform_to_viewport(GLContext *c,GLVertex *v)
{
  float winv;

  /* coordinates */
  winv=1.0/v->pc.W;
  v->zp.x= (int) ( v->pc.X * winv * c->viewport.scale.X 
                   + c->viewport.trans.X );
  v->zp.y= (int) ( v->pc.Y * winv * c->viewport.scale.Y 
                   + c->viewport.trans.Y );
  v->zp.z= (int) ( v->pc.Z * winv * c->viewport.scale.Z 
                   + c->viewport.trans.Z );
  /* color */
  if (c->lighting_enabled) {
      v->zp.r=(int)(v->color.v[0] * (ZB_POINT_RED_MAX - ZB_POINT_RED_MIN) 
                    + ZB_POINT_RED_MIN);
      v->zp.g=(int)(v->color.v[1] * (ZB_POINT_GREEN_MAX - ZB_POINT_GREEN_MIN) 
                    + ZB_POINT_GREEN_MIN);
      v->zp.b=(int)(v->color.v[2] * (ZB_POINT_BLUE_MAX - ZB_POINT_BLUE_MIN) 
                    + ZB_POINT_BLUE_MIN);
  } else {
      /* no need to convert to integer if no lighting : take current color */
      v->zp.r = c->longcurrent_color[0];
      v->zp.g = c->longcurrent_color[1];
      v->zp.b = c->longcurrent_color[2];
  }
  
  /* texture */

  if (c->texture_2d_enabled) {
    v->zp.s=(int)(v->tex_coord.X * (ZB_POINT_S_MAX - ZB_POINT_S_MIN) 
                  + ZB_POINT_S_MIN);
    v->zp.t=(int)(v->tex_coord.Y * (ZB_POINT_T_MAX - ZB_POINT_T_MIN) 
                  + ZB_POINT_T_MIN);
  }
}


 /* triangle */

/*
 * Clipping
 */

/* We clip the segment [a,b] against the 6 planes of the normal volume.
 * We compute the point 'c' of intersection and the value of the parameter 't'
 * of the intersection if x=a+t(b-a). 
 */
	 
static float clip_xmin(V4 *c,V4 *a,V4 *b) 
{ float t,dX,dY,dZ,dW,den;
 dX = (b->v[0] - a->v[0]);
 dY = (b->v[1] - a->v[1]);
 dZ = (b->v[2] - a->v[2]);
 dW = (b->v[3] - a->v[3]);
 den = -(- dX) + dW;
 if (den == 0)
	 t=0;
 else 
	 t = ( - a->v[0] - a->v[3]) / den;
 c->v[1] = a->v[1] + t * dY;
 c->v[2] = a->v[2] + t * dZ;
 c->v[3] = a->v[3] + t * dW;
 c->v[0] = - c->v[3];
 return t;
}

static float clip_xmax(V4 *c,V4 *a,V4 *b) { float t,dX,dY,dZ,dW,den;
 dX = (b->v[0] - a->v[0]);
 dY = (b->v[1] - a->v[1]);
 dZ = (b->v[2] - a->v[2]);
 dW = (b->v[3] - a->v[3]);
 den = -(+ dX) + dW;
 if (den == 0) 
	 t=0;
 else 
	 t = ( + a->v[0] - a->v[3]) / den;
 c->v[1] = a->v[1] + t * dY;
 c->v[2] = a->v[2] + t * dZ;
 c->v[3] = a->v[3] + t * dW;
 c->v[0] = + c->v[3];
 return t;
}

static float clip_ymin(V4 *c,V4 *a,V4 *b) { float t,dX,dY,dZ,dW,den;
 dX = (b->v[0] - a->v[0]);
 dY = (b->v[1] - a->v[1]);
 dZ = (b->v[2] - a->v[2]);
 dW = (b->v[3] - a->v[3]);
 den = -(- dY) + dW;
 if (den == 0) 
	 t=0;
 else 
	 t = ( - a->v[1] - a->v[3]) / den;
 c->v[0] = a->v[0] + t * dX;
 c->v[2] = a->v[2] + t * dZ;
 c->v[3] = a->v[3] + t * dW;
 c->v[1] = - c->v[3];
 return t;
}

static float clip_ymax(V4 *c,V4 *a,V4 *b) { float t,dX,dY,dZ,dW,den;
 dX = (b->v[0] - a->v[0]);
 dY = (b->v[1] - a->v[1]);
 dZ = (b->v[2] - a->v[2]);
 dW = (b->v[3] - a->v[3]);
 den = -(+ dY) + dW;
 if (den == 0) t=0;
 else t = ( + a->v[1] - a->v[3]) / den;
 c->v[0] = a->v[0] + t * dX;
 c->v[2] = a->v[2] + t * dZ;
 c->v[3] = a->v[3] + t * dW;
 c->v[1] = + c->v[3];
 return t;
}

static float clip_zmin(V4 *c,V4 *a,V4 *b) { float t,dX,dY,dZ,dW,den;
 dX = (b->v[0] - a->v[0]);
 dY = (b->v[1] - a->v[1]);
 dZ = (b->v[2] - a->v[2]);
 dW = (b->v[3] - a->v[3]);
 den = -(- dZ) + dW;
 if (den == 0) t=0;
 else t = ( - a->v[2] - a->v[3]) / den;
 c->v[0] = a->v[0] + t * dX;
 c->v[1] = a->v[1] + t * dY;
 c->v[3] = a->v[3] + t * dW;
 c->v[2] = - c->v[3];
 return t;
}
static float clip_zmax(V4 *c,V4 *a,V4 *b) { float t,dX,dY,dZ,dW,den;
 dX = (b->v[0] - a->v[0]);
 dY = (b->v[1] - a->v[1]);
 dZ = (b->v[2] - a->v[2]);
 dW = (b->v[3] - a->v[3]);
 den = -(+ dZ) + dW;
 if (den == 0) t=0;
 else t = ( + a->v[2] - a->v[3]) / den;
 c->v[0] = a->v[0] + t * dX;
 c->v[1] = a->v[1] + t * dY;
 c->v[3] = a->v[3] + t * dW;
 c->v[2] = + c->v[3];
 return t;
}




float (*clip_proc[6])(V4 *,V4 *,V4 *)=  {
    clip_xmin,clip_xmax,
    clip_ymin,clip_ymax,
    clip_zmin,clip_zmax
};

static inline void updateTmp(GLContext *c,
			     GLVertex *q,GLVertex *p0,GLVertex *p1,float t)
{
  if (c->current_shade_model == GL_SMOOTH) {
    q->color.v[0]=p0->color.v[0] + (p1->color.v[0]-p0->color.v[0])*t;
    q->color.v[1]=p0->color.v[1] + (p1->color.v[1]-p0->color.v[1])*t;
    q->color.v[2]=p0->color.v[2] + (p1->color.v[2]-p0->color.v[2])*t;
  } else {
    q->color.v[0]=p0->color.v[0];
    q->color.v[1]=p0->color.v[1];
    q->color.v[2]=p0->color.v[2];
  }

  if (c->texture_2d_enabled) {
    q->tex_coord.X=p0->tex_coord.X + (p1->tex_coord.X-p0->tex_coord.X)*t;
    q->tex_coord.Y=p0->tex_coord.Y + (p1->tex_coord.Y-p0->tex_coord.Y)*t;
  }

  q->clip_code=gl_clipcode(q->pc.X,q->pc.Y,q->pc.Z,q->pc.W);
  if (q->clip_code==0)
    gl_transform_to_viewport(c,q);
}

static void gl_draw_triangle_clip(GLContext *c,
                                  GLVertex *p0,GLVertex *p1,GLVertex *p2,int clip_bit);

void gl_draw_triangle(GLContext *c,
                      GLVertex *p0,GLVertex *p1,GLVertex *p2)
{
  int co,c_and,cc[3],front;
  float norm;
  
  cc[0]=p0->clip_code;
  cc[1]=p1->clip_code;
  cc[2]=p2->clip_code;
  
  co=cc[0] | cc[1] | cc[2];

  /* we handle the non clipped case here to go faster */
  if (co==0) {
    
      norm=(float)(p1->zp.x-p0->zp.x)*(float)(p2->zp.y-p0->zp.y)-
        (float)(p2->zp.x-p0->zp.x)*(float)(p1->zp.y-p0->zp.y);
      
      if (norm == 0) return;

      front = norm < 0.0;
      front = front ^ c->current_front_face;
  
      /* back face culling */
      if (c->cull_face_enabled) {
        /* most used case first */
        if (c->current_cull_face == GL_BACK) {
          if (front == 0) return;
          c->draw_triangle_front(c,p0,p1,p2);
        } else if (c->current_cull_face == GL_FRONT) {
          if (front != 0) return;
          c->draw_triangle_back(c,p0,p1,p2);
        } else {
          return;
        }
      } else {
        /* no culling */
        if (front) {
          c->draw_triangle_front(c,p0,p1,p2);
        } else {
          c->draw_triangle_back(c,p0,p1,p2);
        }
      }
  } else {
    c_and=cc[0] & cc[1] & cc[2];
    if (c_and==0) {
      gl_draw_triangle_clip(c,p0,p1,p2,0);
    }
  }
}

static void gl_draw_triangle_clip(GLContext *c,
                                  GLVertex *p0,GLVertex *p1,GLVertex *p2,int clip_bit)
{
  int co,c_and,co1,cc[3],edge_flag_tmp,clip_mask;
  GLVertex tmp1,tmp2,*q[3];
  float tt;
  
  cc[0]=p0->clip_code;
  cc[1]=p1->clip_code;
  cc[2]=p2->clip_code;
  
  co=cc[0] | cc[1] | cc[2];
  if (co == 0) {
    gl_draw_triangle(c,p0,p1,p2);
  } else {
    c_and=cc[0] & cc[1] & cc[2];
    /* the triangle is completely outside */
    if (c_and!=0) return;

    /* find the next direction to clip */
    while (clip_bit < 6 && (co & (1 << clip_bit)) == 0)  {
      clip_bit++;
    }

    /* this test can be true only in case of rounding errors */
    if (clip_bit == 6) {
#if 0
      printf("Error:\n");
      printf("%f %f %f %f\n",p0->pc.X,p0->pc.Y,p0->pc.Z,p0->pc.W);
      printf("%f %f %f %f\n",p1->pc.X,p1->pc.Y,p1->pc.Z,p1->pc.W);
      printf("%f %f %f %f\n",p2->pc.X,p2->pc.Y,p2->pc.Z,p2->pc.W);
#endif
      return;
    }
  
    clip_mask = 1 << clip_bit;
    co1=(cc[0] ^ cc[1] ^ cc[2]) & clip_mask;
    
    if (co1)  { 
      /* one point outside */

      if (cc[0] & clip_mask) { q[0]=p0; q[1]=p1; q[2]=p2; }
      else if (cc[1] & clip_mask) { q[0]=p1; q[1]=p2; q[2]=p0; }
      else { q[0]=p2; q[1]=p0; q[2]=p1; }
      
      tt=clip_proc[clip_bit](&tmp1.pc,&q[0]->pc,&q[1]->pc);
      updateTmp(c,&tmp1,q[0],q[1],tt);

      tt=clip_proc[clip_bit](&tmp2.pc,&q[0]->pc,&q[2]->pc);
      updateTmp(c,&tmp2,q[0],q[2],tt);

      tmp1.edge_flag=q[0]->edge_flag;
      edge_flag_tmp=q[2]->edge_flag;
      q[2]->edge_flag=0;
      gl_draw_triangle_clip(c,&tmp1,q[1],q[2],clip_bit+1);

      tmp2.edge_flag=1;
      tmp1.edge_flag=0;
      q[2]->edge_flag=edge_flag_tmp;
      gl_draw_triangle_clip(c,&tmp2,&tmp1,q[2],clip_bit+1);
    } else {
      /* two points outside */

      if ((cc[0] & clip_mask)==0) { q[0]=p0; q[1]=p1; q[2]=p2; }
      else if ((cc[1] & clip_mask)==0) { q[0]=p1; q[1]=p2; q[2]=p0; } 
      else { q[0]=p2; q[1]=p0; q[2]=p1; }
      
      tt=clip_proc[clip_bit](&tmp1.pc,&q[0]->pc,&q[1]->pc);
      updateTmp(c,&tmp1,q[0],q[1],tt);

      tt=clip_proc[clip_bit](&tmp2.pc,&q[0]->pc,&q[2]->pc);
      updateTmp(c,&tmp2,q[0],q[2],tt);
      
      tmp1.edge_flag=1;
      tmp2.edge_flag=q[2]->edge_flag;
      gl_draw_triangle_clip(c,q[0],&tmp1,&tmp2,clip_bit+1);
    }
  }
}


 
 

void gl_draw_triangle_fill(GLContext *c,
                           GLVertex *p0,GLVertex *p1,GLVertex *p2)
{
 
    
  if (c->texture_2d_enabled) {
 
    ZB_setTexture(c->zb,(PIXEL *)c->current_texture->images[0].pixmap);
    ZB_fillTriangleMappingPerspective(c->zb,&p0->zp,&p1->zp,&p2->zp);
  } else if (c->current_shade_model == GL_SMOOTH) {
    ZB_fillTriangleSmooth(c->zb,&p0->zp,&p1->zp,&p2->zp);
  } 
}

 




