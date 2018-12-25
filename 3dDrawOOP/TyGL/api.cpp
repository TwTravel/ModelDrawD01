#include "zgl.h"
#include <stdio.h>
/* glVertex */
 void gl_vertex_transform(GLContext * c, GLVertex * v);

void glVertex4f(float x,float y,float z,float w)
{
  GLContext *c=gl_get_context(); 
  GLVertex *v;

    int n, i, cnt;

    assert(c->in_begin != 0);

    n = c->vertex_n;
    cnt = c->vertex_cnt;
    cnt++;
    c->vertex_cnt = cnt;

    /* quick fix to avoid crashes on large polygons */
    if (n >= c->vertex_max) {
	GLVertex *newarray;
	c->vertex_max <<= 1;	/* just double size */
	newarray = (GLVertex *)gl_malloc(sizeof(GLVertex) * c->vertex_max);
	if (!newarray) {
	    gl_fatal_error("unable to allocate GLVertex array.\n");
	}
	memcpy(newarray, c->vertex, n * sizeof(GLVertex));
	gl_free(c->vertex);
	c->vertex = newarray;
    }
    /* new vertex entry */
    v = &c->vertex[n];
    n++;

    v->coord.X = x;
    v->coord.Y = y;
    v->coord.Z = z;
    v->coord.W = w;

    gl_vertex_transform(c, v);

    /* color */

    if (c->lighting_enabled) {
	gl_shade_vertex(c, v);
    } else {
	v->color = c->current_color;
    }

    /* tex coords */

    if (c->texture_2d_enabled) {
	if (c->apply_texture_matrix) {
	    M4::gl_M4_MulV4(&v->tex_coord, c->matrix_buffer_ptr[2], &c->current_tex_coord);
	} else {
	    v->tex_coord = c->current_tex_coord;
	}
    }
    /* precompute the mapping to the viewport */
    if (v->clip_code == 0)
	gl_transform_to_viewport(c, v);

    /* edge flag */

    v->edge_flag = c->current_edge_flag;

    switch (c->begin_type) {
     

   

    case GL_TRIANGLES:
	if (n == 3) {
	    gl_draw_triangle(c, &c->vertex[0], &c->vertex[1], &c->vertex[2]);
	    n = 0;
	}
	break;
    case GL_TRIANGLE_STRIP:
	if (cnt >= 3) {
	    if (n == 3)
		n = 0;
            /* needed to respect triangle orientation */
            switch(cnt & 1) {
            case 0:
      		gl_draw_triangle(c,&c->vertex[2],&c->vertex[1],&c->vertex[0]);
      		break;
            default:
            case 1:
      		gl_draw_triangle(c,&c->vertex[0],&c->vertex[1],&c->vertex[2]);
      		break;
            }
	}
	break;
    case GL_TRIANGLE_FAN:
	if (n == 3) {
	    gl_draw_triangle(c, &c->vertex[0], &c->vertex[1], &c->vertex[2]);
	    c->vertex[1] = c->vertex[2];
	    n = 2;
	}
	break;

    case GL_QUADS:
	if (n == 4) {
	    c->vertex[2].edge_flag = 0;
	    gl_draw_triangle(c, &c->vertex[0], &c->vertex[1], &c->vertex[2]);
	    c->vertex[2].edge_flag = 1;
	    c->vertex[0].edge_flag = 0;
	    gl_draw_triangle(c, &c->vertex[0], &c->vertex[2], &c->vertex[3]);
	    n = 0;
	}
	break;

    case GL_QUAD_STRIP:
	if (n == 4) {
	    gl_draw_triangle(c, &c->vertex[0], &c->vertex[1], &c->vertex[2]);
	    gl_draw_triangle(c, &c->vertex[1], &c->vertex[3], &c->vertex[2]);
	    for (i = 0; i < 2; i++)
		c->vertex[i] = c->vertex[i + 2];
	    n = 2;
	}
	break;
    case GL_POLYGON:
	break;
    default:
	gl_fatal_error("glBegin: type %x not handled\n", c->begin_type);
    }

    c->vertex_n = n;
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
  GLContext *c=gl_get_context(); 
  c->current_normal.X = x;
  c->current_normal.Y = y;
  c->current_normal.Z = z;
  c->current_normal.W = 0;
}

void glNormal3fv(float *v) 
{
  glNormal3f(v[0],v[1],v[2]);
}

/* glColor */

void glColor4f(float r,float g,float b,float a)
{
  GLContext *c=gl_get_context(); 
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
  c->current_color.X = p[1].f;
    c->current_color.Y = p[2].f;
    c->current_color.Z = p[3].f;
    c->current_color.W = p[4].f;
    c->longcurrent_color[0] = p[5].ui;
    c->longcurrent_color[1] = p[6].ui;
    c->longcurrent_color[2] = p[7].ui;

    if (c->color_material_enabled) {
	GLParam q[7];
	q[0].op = OP_Material;
	q[1].i = c->current_color_material_mode;
	q[2].i = c->current_color_material_type;
	q[3].f = p[1].f;
	q[4].f = p[2].f;
	q[5].f = p[3].f;
	q[6].f = p[4].f;
	glopMaterial(c, q);
    }
}

 

void glColor3f(float x,float y,float z) 
{
  glColor4f(x,y,z,1);
}

 

/* TexCoord */

void glTexCoord4f(float s,float t,float r,float q)
{
    GLContext *c=gl_get_context(); 
    c->current_tex_coord.X = s;
    c->current_tex_coord.Y =  t;
    c->current_tex_coord.Z = r;
    c->current_tex_coord.W = q;
}

void glTexCoord2f(float s,float t)
{
  glTexCoord4f(s,t,0,1);
}

void glTexCoord2fv(float *v)
{
  glTexCoord4f(v[0],v[1],0,1);
}

 
void GL_EnableDisable(GLContext *c,GLParam *p)
{
  int code=p[1].i;
  int v=p[2].i;

  switch(code) {
  case GL_CULL_FACE:
    c->cull_face_enabled=v;
    break;
  case GL_LIGHTING:
    c->lighting_enabled=v;
    break;
  case GL_COLOR_MATERIAL:
    c->color_material_enabled=v;
      break;
  case GL_TEXTURE_2D:
    c->texture_2d_enabled=v;
    break;
  case GL_NORMALIZE:
    c->normalize_enabled=v;
    break;
  case GL_DEPTH_TEST:
    c->depth_test = v;
    break;
  case GL_POLYGON_OFFSET_FILL:
    if (v) c->offset_states |= TGL_OFFSET_FILL;
    else c->offset_states &= ~TGL_OFFSET_FILL;
    break; 
  case GL_POLYGON_OFFSET_POINT:
    if (v) c->offset_states |= TGL_OFFSET_POINT;
    else c->offset_states &= ~TGL_OFFSET_POINT;
    break; 
  case GL_POLYGON_OFFSET_LINE:
    if (v) c->offset_states |= TGL_OFFSET_LINE;
    else c->offset_states &= ~TGL_OFFSET_LINE;
    break; 
  default:
    if (code>=GL_LIGHT0 && code<GL_LIGHT0+MAX_LIGHTS) {
      gl_enable_disable_light(c,code - GL_LIGHT0, v);
    } else {
      /*
      fprintf(stderr,"glEnableDisable: 0x%X not supported.\n",code);
      */
    }
    break;
  }
}
   

/* glEnable / glDisable */

void glEnable(int cap)
{
  GLParam p[3];

  p[0].op=OP_EnableDisable;
  p[1].i=cap;
  p[2].i=1;

   GLContext *c=gl_get_context(); 
   GL_EnableDisable( c, p);
   
}

void glDisable(int cap)
{
  GLParam p[3];
  p[0].op=OP_EnableDisable;
  p[1].i=cap;
  p[2].i=0;
  
  GLContext *c=gl_get_context();
  GL_EnableDisable( c, p);
}

/* glBegin / glEnd */
void gl_eval_viewport(GLContext * c);
void glBegin(int mode)
{
   GLContext *c=gl_get_context(); 
   
   int type;
   M4 tmp;

    assert(c->in_begin == 0);

    type = mode;
    c->begin_type = type;
    c->in_begin = 1;
    c->vertex_n = 0;
    c->vertex_cnt = 0;

    if (c->matrix_model_projection_updated) {

	if (c->lighting_enabled) {
	    /* precompute inverse modelview */
	    M4::gl_M4_Inv(&tmp, c->matrix_buffer_ptr[0]);
	    M4::gl_M4_Transpose(&c->matrix_model_view_inv, &tmp);
	} else {
	    float *m = &c->matrix_model_projection.m[0][0];
	    /* precompute projection matrix */
	    M4::gl_M4_Mul(&c->matrix_model_projection,
		      c->matrix_buffer_ptr[1],
		      c->matrix_buffer_ptr[0]);
	    /* test to accelerate computation */
	    c->matrix_model_projection_no_w_transform = 0;
	    if (m[12] == 0.0 && m[13] == 0.0 && m[14] == 0.0)
		c->matrix_model_projection_no_w_transform = 1;
	}

	/* test if the texture matrix is not Identity */
	c->apply_texture_matrix = !M4::gl_M4_IsId(c->matrix_buffer_ptr[2]);

	c->matrix_model_projection_updated = 0;
    }
    /*  viewport */
    if (c->viewport.updated) {
	gl_eval_viewport(c);
	c->viewport.updated = 0;
    }
    /* triangle drawing functions */
    //if (c->render_mode == GL_SELECT) {
	//c->draw_triangle_front = gl_draw_triangle_select;
	//c->draw_triangle_back = gl_draw_triangle_select;
    //} else 
	{
	switch (c->polygon_mode_front) {
 
	default:
	    c->draw_triangle_front = gl_draw_triangle_fill;
	    break;
	}

	switch (c->polygon_mode_back) {
 
	default:
	    c->draw_triangle_back = gl_draw_triangle_fill;
	    break;
	}
    }
}

void glEnd(void)
{
  GLParam p[1];

  p[0].op=OP_End;

  glRunFunc(p);
}

/* matrix */
void gl_matrix_update(GLContext *c);

void glMatrixMode(int mode)
{
  GLContext *c=gl_get_context(); 
  GLParam p[2];

  p[0].op=OP_MatrixMode;
  p[1].i=mode;

//  glRunFunc(p);
  //==================================
   //int mode=p[1].i;
  switch(mode) {
  case GL_MODELVIEW:
    c->matrix_mode=0;
    break;
  case GL_PROJECTION:
    c->matrix_mode=1;
    break;
  case GL_TEXTURE:
    c->matrix_mode=2;
    break;
  default:
    assert(0);
  }
}

void glLoadMatrixf(const float *mm)
{
   GLContext *c=gl_get_context(); 
  GLParam p[17];
  int i;

  p[0].op=OP_LoadMatrix;
  for(i=0;i<16;i++) p[i+1].f=mm[i];

 // glRunFunc(p);
   M4 *m;
 
  
  GLParam *q;

  m=c->matrix_buffer_ptr[c->matrix_mode];
  q=p+1;

  for(i=0;i<4;i++) {
    m->m[0][i]=q[0].f;
    m->m[1][i]=q[1].f;
    m->m[2][i]=q[2].f;
    m->m[3][i]=q[3].f;
    q+=4;
  }

  gl_matrix_update(c);
}

void glLoadIdentity(void)
{
  GLContext *c=gl_get_context(); 
  GLParam p[1];

  p[0].op=OP_LoadIdentity;

  //glRunFunc(p);
  M4::gl_M4_Id(c->matrix_buffer_ptr[c->matrix_mode]);

  gl_matrix_update(c);
}

void glMultMatrixf(const float *mm)
{
   GLContext *c=gl_get_context(); 
  GLParam p[17];
  int i;

  p[0].op=OP_MultMatrix;
  for(i=0;i<16;i++) p[i+1].f=mm[i];

  //glRunFunc(p);
   M4 m;
 

  GLParam *q;
  q=p+1;

  for(i=0;i<4;i++) {
    m.m[0][i]=q[0].f;
    m.m[1][i]=q[1].f;
    m.m[2][i]=q[2].f;
    m.m[3][i]=q[3].f;
    q+=4;
  }

  M4::gl_M4_MulLeft(c->matrix_buffer_ptr[c->matrix_mode],&m);

  gl_matrix_update(c);
}

void glPushMatrix(void)
{
  GLContext *c=gl_get_context(); 
  GLParam p[1];

  p[0].op=OP_PushMatrix;

  //glRunFunc(p);
  int n=c->matrix_mode;
  M4 *m;

//  assert( (c->matrix_buffer_ptr[n] - c->matrix_stack[n] + 1 )
//	   < c->matrix_stack_depth_max[n] );

  m=++c->matrix_buffer_ptr[n];
  
  M4::gl_M4_Move(&m[0],&m[-1]);

  gl_matrix_update(c);
}

void glPopMatrix(void)
{
   GLContext *c=gl_get_context(); 
   GLParam p[1];

  p[0].op=OP_PopMatrix;

 // glRunFunc(p);
  int n=c->matrix_mode;

//  assert( c->matrix_buffer_ptr[n] > c->matrix_stack[n] );
  c->matrix_buffer_ptr[n]--;
  gl_matrix_update(c);
}

void glRotatef(float angle,float x,float y,float z)
{
  GLContext *c=gl_get_context(); 
  GLParam p[5];

  p[0].op=OP_Rotate;
  p[1].f=angle;
  p[2].f=x;
  p[3].f=y;
  p[4].f=z;

 // glRunFunc(p);

   M4 m;
  float u[3];
//  float angle;
  int dir_code;

  angle = p[1].f * M_PI / 180.0;
  u[0]=p[2].f;
  u[1]=p[3].f;
  u[2]=p[4].f;

  /* simple case detection */
  dir_code = ((u[0] != 0)<<2) | ((u[1] != 0)<<1) | (u[2] != 0);

  switch(dir_code) {
  case 0:
    M4::gl_M4_Id(&m);
    break;
  case 4:
    if (u[0] < 0) angle=-angle;
    M4::gl_M4_Rotate(&m,angle,0);
    break;
  case 2:
    if (u[1] < 0) angle=-angle;
    M4::gl_M4_Rotate(&m,angle,1);
    break;
  case 1:
    if (u[2] < 0) angle=-angle;
    M4::gl_M4_Rotate(&m,angle,2);
    break;
  default:
    {
      float cost, sint;

      /* normalize vector */
      float len = u[0]*u[0]+u[1]*u[1]+u[2]*u[2];
      if (len == 0.0f) return;
      len = 1.0f / sqrt(len);
      u[0] *= len;
      u[1] *= len;
      u[2] *= len;

      /* store cos and sin values */
      cost=cos(angle);
      sint=sin(angle);

      /* fill in the values */
      m.m[3][0]=m.m[3][1]=m.m[3][2]=
        m.m[0][3]=m.m[1][3]=m.m[2][3]=0.0f;
      m.m[3][3]=1.0f;

      /* do the math */
      m.m[0][0]=u[0]*u[0]+cost*(1-u[0]*u[0]);
      m.m[1][0]=u[0]*u[1]*(1-cost)-u[2]*sint;
      m.m[2][0]=u[2]*u[0]*(1-cost)+u[1]*sint;
      m.m[0][1]=u[0]*u[1]*(1-cost)+u[2]*sint;
      m.m[1][1]=u[1]*u[1]+cost*(1-u[1]*u[1]);
      m.m[2][1]=u[1]*u[2]*(1-cost)-u[0]*sint;
      m.m[0][2]=u[2]*u[0]*(1-cost)-u[1]*sint;
      m.m[1][2]=u[1]*u[2]*(1-cost)+u[0]*sint;
      m.m[2][2]=u[2]*u[2]+cost*(1-u[2]*u[2]);
    }
  }

  M4::gl_M4_MulLeft(c->matrix_buffer_ptr[c->matrix_mode],&m);

  gl_matrix_update(c);
}

void glTranslatef(float x,float y,float z)
{
  GLContext *c=gl_get_context(); 
  GLParam p[4];

  p[0].op=OP_Translate;
  p[1].f=x;
  p[2].f=y;
  p[3].f=z;

 // glRunFunc(p);
   float *m;
 // float x=p[1].f,y=p[2].f,z=p[3].f;

  m=&c->matrix_buffer_ptr[c->matrix_mode]->m[0][0];

  m[3] = m[0] * x + m[1] * y + m[2]  * z + m[3];
  m[7] = m[4] * x + m[5] * y + m[6]  * z + m[7];
  m[11] = m[8] * x + m[9] * y + m[10] * z + m[11];
  m[15] = m[12] * x + m[13] * y + m[14] * z + m[15];

  gl_matrix_update(c);
}

void glScalef(float x,float y,float z)
{
  GLContext *c=gl_get_context(); 
  GLParam p[4];

  p[0].op=OP_Scale;
  p[1].f=x;
  p[2].f=y;
  p[3].f=z;

  //glRunFunc(p);
  float *m;
//  float x=p[1].f,y=p[2].f,z=p[3].f;

  m=&c->matrix_buffer_ptr[c->matrix_mode]->m[0][0];

  m[0] *= x;   m[1] *= y;   m[2]  *= z;
  m[4] *= x;   m[5] *= y;   m[6]  *= z;
  m[8] *= x;   m[9] *= y;   m[10] *= z;
  m[12] *= x;   m[13] *= y;   m[14] *= z;
  gl_matrix_update(c);
}


void glViewport(int x,int y,int width,int height)
{
	GLContext *c=gl_get_context(); 
  /*GLParam p[5];

  p[0].op=OP_Viewport;
  p[1].i=x;
  p[2].i=y;
  p[3].i=width;
  p[4].i=height;

  glRunFunc(p);*/
  int  xsize_req,ysize_req;
  
 // xmin=p[1].i;
 // ymin=p[2].i;
  int xmin(x), ymin(y);
  int xsize=width;
  int ysize=height;

  /* we may need to resize the zbuffer */

  if (c->viewport.xmin != xmin ||
      c->viewport.ymin != ymin ||
      c->viewport.xsize != xsize ||
      c->viewport.ysize != ysize) {

    xsize_req=xmin+xsize;
    ysize_req=ymin+ysize;

    if (c->gl_resize_viewport && 
        c->gl_resize_viewport(c,&xsize_req,&ysize_req) != 0) {
      gl_fatal_error("glViewport: error while resizing display");
    }

    xsize=xsize_req-xmin;
    ysize=ysize_req-ymin;
    if (xsize <= 0 || ysize <= 0) {
      gl_fatal_error("glViewport: size too small");
    }

    //tgl_trace("glViewport: %d %d %d %d\n",
     //         xmin, ymin, xsize, ysize);
    c->viewport.xmin=xmin;
    c->viewport.ymin=ymin;
    c->viewport.xsize=xsize;
    c->viewport.ysize=ysize;
    
    c->viewport.updated=1;
  }
}


void glFrustum(double left,double right,double bottom,double top,
               double near,double farp)
{
  GLContext *c=gl_get_context(); 
  float *r;
  M4 m;
 
  float x,y,A,B,C,D;

  x = (2.0*near) / (right-left);
  y = (2.0*near) / (top-bottom);
  A = (right+left) / (right-left);
  B = (top+bottom) / (top-bottom);
  C = -(farp+near) / ( farp-near);
  D = -(2.0*farp*near) / (farp-near);

  r=&m.m[0][0];
  r[0]= x; r[1]=0; r[2]=A; r[3]=0;
  r[4]= 0; r[5]=y; r[6]=B; r[7]=0;
  r[8]= 0; r[9]=0; r[10]=C; r[11]=D;
  r[12]= 0; r[13]=0; r[14]=-1; r[15]=0;

  M4::gl_M4_MulLeft(c->matrix_buffer_ptr[c->matrix_mode],&m);

  gl_matrix_update(c);
}

void gluPerspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
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


void glLightf(int light,int type,float val)
{ 
  GLContext *c=gl_get_context(); 
  V4 v;
  GLLight *l;
  int i;
  
  assert(light >= GL_LIGHT0 && light < GL_LIGHT0+MAX_LIGHTS );

  l=&c->lights[light-GL_LIGHT0];

  for(i=0;i<4;i++) v.v[i]= val;//p[3+i].f;

  switch(type) {
  case GL_AMBIENT:
    l->ambient=v;
    break;
  case GL_DIFFUSE:
    l->diffuse=v;
    break;
  case GL_SPECULAR:
    l->specular=v;
    break;
  case GL_POSITION:
    {
      V4 pos;
      M4::gl_M4_MulV4(&pos,c->matrix_buffer_ptr[0],&v);

      l->position=pos;

      if (l->position.v[3] == 0) {
        l->norm_position.X=pos.X;
        l->norm_position.Y=pos.Y;
        l->norm_position.Z=pos.Z;
        
        gl_V3_Norm(&l->norm_position);
      }
    }
    break;
  case GL_SPOT_DIRECTION:
    for(i=0;i<3;i++) {
      l->spot_direction.v[i]=v.v[i];
      l->norm_spot_direction.v[i]=v.v[i];
    }
    gl_V3_Norm(&l->norm_spot_direction);
    break;
  case GL_SPOT_EXPONENT:
    l->spot_exponent=v.v[0];
    break;
  case GL_SPOT_CUTOFF:
    {
      float a=v.v[0];
      assert(a == 180 || (a>=0 && a<=90));
      l->spot_cutoff=a;
      if (a != 180) l->cos_spot_cutoff=cos(a * M_PI / 180.0);
    }
    break;
  case GL_CONSTANT_ATTENUATION:
    l->attenuation[0]=v.v[0];
    break;
  case GL_LINEAR_ATTENUATION:
    l->attenuation[1]=v.v[0];
    break;
  case GL_QUADRATIC_ATTENUATION:
    l->attenuation[2]=v.v[0];
    break;
  default:
    assert(0);
  }
}

void glLightModelfv(int pname,float *param)
{
	  GLContext *c=gl_get_context(); 
 /* GLParam p[6];
  int i;

  p[0].op=OP_LightModel;
  p[1].i=pname;
  for(i=0;i<4;i++) p[2+i].f=param[i];

  glRunFunc(p);*/

  // int pname=p[1].i;
  float *v=param;//&p[2].f;
  int i;

  switch(pname) {
  case GL_LIGHT_MODEL_AMBIENT:
    for(i=0;i<4;i++) 
      c->ambient_light_model.v[i]=v[i];
    break;
  case GL_LIGHT_MODEL_LOCAL_VIEWER:
    c->local_light_model=(int)v[0];
    break;
  case GL_LIGHT_MODEL_TWO_SIDE:
    c->light_model_two_side = (int)v[0];
    break;
  default:
    //tgl_warning("glopLightModel: illegal pname: 0x%x\n", pname);
    //assert(0);
    break;
  }
}

/* clear */

 

 
/* textures */

void glTexImage2D( int target, int level, int components,
                   int width, int height, int border,
                   int format, int type, void *pixels)
{
  GLContext *c=gl_get_context(); 
  
 
  GLImage *im;
  unsigned char *pixels1;
  int do_free;

  if (!(target == GL_TEXTURE_2D && level == 0 && components == 3 && 
        border == 0 && format == GL_RGB &&
        type == GL_UNSIGNED_BYTE)) {
    gl_fatal_error("glTexImage2D: combinaison of parameters not handled");
  }
  
  do_free=0;
  if (width != 256 || height != 256) {
    pixels1 = (unsigned char*)gl_malloc(256 * 256 * 3);
    /* no interpolation is done here to respect the original image aliasing ! */
    gl_resizeImageNoInterpolate(pixels1,256,256,(unsigned char*)pixels,width,height);
    do_free=1;
    width=256;
    height=256;
  } else {
    pixels1=(unsigned char*)pixels;
  }

  im=&c->current_texture->images[level];
  im->xsize=width;
  im->ysize=height;
  if (im->pixmap!=NULL) gl_free(im->pixmap);
//#if TGL_FEATURE_RENDER_BITS == 24 
  im->pixmap=gl_malloc(width*height*3);
  if(im->pixmap) {
      memcpy(im->pixmap,pixels1,width*height*3);
  }
 
  if (do_free) gl_free(pixels1);
}

GLTexture * alloc_texture(GLContext *c,int h)
{
  GLTexture *t,**ht;
  
  t= (GLTexture *)gl_zalloc(sizeof(GLTexture));

  ht=&c->shared_state.texture_hash_table[h % TEXTURE_HASH_TABLE_SIZE];

  t->next=*ht;
  t->prev=NULL;
  if (t->next != NULL) t->next->prev=t;
  *ht=t;

  t->handle=h;
  
  return t;
}
 
 GLTexture * find_texture(GLContext *c,int h)
{
  GLTexture *t;

  t=c->shared_state.texture_hash_table[h % TEXTURE_HASH_TABLE_SIZE];
  while (t!=NULL) {
    if (t->handle == h) return t;
    t=t->next;
  }
  return NULL;
}

void glBindTexture(int target,int texture)
{
  GLContext *c=gl_get_context(); 
  GLParam p[3];

  //p[0].op=OP_BindTexture;
  //p[1].i=target;
  //p[2].i=texture;

  //glRunFunc(p);

  //target =p[1].i;
  //texture=p[2].i;
  GLTexture *t;

  assert(target == GL_TEXTURE_2D && texture >= 0);

  t=find_texture(c,texture);
  if (t==NULL) {
    t=alloc_texture(c,texture);
  }
  c->current_texture=t;
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

/*void glCallList(unsigned int list)
{
  GLParam p[2];

  p[0].op=OP_CallList;
  p[1].i=list;

  glRunFunc(p);
}*/

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

