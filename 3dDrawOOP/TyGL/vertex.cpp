#include "zgl.h"
#include "string.h"

void glopNormal(GLContext * c, GLParam * p)
{
  
}

void glopTexCoord(GLContext * c, GLParam * p)
{
 
}

/*void glopEdgeFlag(GLContext * c, GLParam * p)
{
    c->current_edge_flag = p[1].i;
}*/

void glopColor(GLContext * c, GLParam * p)
{

    
}


void gl_eval_viewport(GLContext * c)
{
    GLViewport *v;
    float zsize = (1 << (ZB_Z_BITS + ZB_POINT_Z_FRAC_BITS));
	int dgbzsize = zsize;

    v = &c->viewport;

    v->trans.X = ((v->xsize - 0.5) / 2.0) + v->xmin;
    v->trans.Y = ((v->ysize - 0.5) / 2.0) + v->ymin;
    v->trans.Z = ((zsize - 0.5) / 2.0) + ((1 << ZB_POINT_Z_FRAC_BITS)) / 2;

    v->scale.X = (v->xsize - 0.5) / 2.0;
    v->scale.Y = -(v->ysize - 0.5) / 2.0;
    v->scale.Z = -((zsize - 0.5) / 2.0);
}

void glopBegin(GLContext * c, GLParam * p)
{
   
}

void gl_vertex_transform(GLContext * c, GLVertex * v);

void glopVertex(GLContext * c, GLParam * p)
{
   
}

void glopEnd(GLContext * c, GLParam * param)
{
   
}
