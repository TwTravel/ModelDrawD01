#include "zgl.h"

/*
static char *op_table_str[]=
{
#define ADD_OP(a,b,c) "gl" #a " " #c,

#include "opinfo.h"
};
*/

static void (*op_table_func[])(GLContext *,GLParam *)=
{
//#define ADD_OP(a,b,c) glop ## a ,

glopColor, 
glopTexCoord, 
glopEdgeFlag, 
glopNormal, 

glopBegin, 
glopVertex, 
glopEnd, 

glopEnableDisable, 

glopMatrixMode, 
glopLoadMatrix, 
glopLoadIdentity, 
glopMultMatrix, 
glopPushMatrix, 
glopPopMatrix, 
glopRotate, 
glopTranslate, 
glopScale, 

glopViewport, 
glopFrustum, 

glopMaterial, 
glopColorMaterial, 
glopLight, 
glopLightModel, 

 

glopTexImage2D, 
glopBindTexture, 
glopTexEnv, 
glopTexParameter, 
glopPixelStore, 

//glopShadeModel, 
//glopCullFace, 
//glopFrontFace, 
//glopPolygonMode, 

glopCallList, 

/* special opcodes */
glopEndList, 
glopNextBuffer, 

 

/* opengl 1.1 polygon offset */
};

/*static int op_table_size[]=
{
#define ADD_OP(a,b,c) b + 1 ,

#include "opinfo.h"
};*/


GLContext *gl_get_context(void)
{
  return gl_ctx;
}

static GLList *find_list(GLContext *c,unsigned int list)
{
  return c->shared_state.lists[list];
}

static void delete_list(GLContext *c,int list)
{
  GLParamBuffer *pb,*pb1;
  GLList *l;

  l=find_list(c,list);
  assert(l != NULL);
  
  /* free param buffer */
  pb=l->first_op_buffer;
  while (pb!=NULL) {
    pb1=pb->next;
    gl_free(pb);
    pb=pb1;
  }
  
  gl_free(l);
  c->shared_state.lists[list]=NULL;
}

static GLList *alloc_list(GLContext *c,int list)
{
  GLList *l;
  GLParamBuffer *ob;

  l= ( GLList *)gl_zalloc(sizeof(GLList));
  ob=( GLParamBuffer *)gl_zalloc(sizeof(GLParamBuffer));

  ob->next=NULL;
  l->first_op_buffer=ob;
  
  ob->ops[0].op=OP_EndList;

  c->shared_state.lists[list]=l;
  return l;
}


void gl_print_op(FILE *f,GLParam *p)
{
  
}


void gl_compile_op(GLContext *c,GLParam *p)
{
   
}

void glRunFunc(GLParam *p)
{
  GLContext *c=gl_get_context();
  int op;

  op=p[0].op;
  op_table_func[op](c,p);
}

/* this opcode is never called directly */
void glopEndList(GLContext *c,GLParam *p)
{
  assert(0);
}

/* this opcode is never called directly */
void glopNextBuffer(GLContext *c,GLParam *p)
{
  assert(0);
}


void glopCallList(GLContext *c,GLParam *p)
{
  
}




