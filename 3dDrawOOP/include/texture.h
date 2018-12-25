/*
 * Texture Manager
 */
#ifndef TEXTURE_HEAD_H
#define TEXTURE_HEAD_H

#include "zgl.h"
#include <string.h>



  GLTexture * find_texture(GLContext *c,int h);
 
static inline void free_texture(GLContext *c,int h)
{
  GLTexture *t,**ht;
  GLImage *im;
  int i;

  t=find_texture(c,h);
  if (t->prev==NULL) {
    ht=&c->shared_state.texture_hash_table
      [t->handle % TEXTURE_HASH_TABLE_SIZE];
    *ht=t->next;
  } else {
    t->prev->next=t->next;
  }
  if (t->next!=NULL) t->next->prev=t->prev;

  for(i=0;i<MAX_TEXTURE_LEVELS;i++) {
    im=&t->images[i];
    if (im->pixmap != NULL) gl_free(im->pixmap);
  }

  gl_free(t);
}

GLTexture  *alloc_texture(GLContext *c,int h);
 


inline void  glInitTextures(GLContext *c)
{
  /* textures */

  c->texture_2d_enabled=0;
  c->current_texture=find_texture(c,0);
}

  inline  void glGenTextures(int n, unsigned int *textures)
{
  GLContext *c=gl_get_context();
  int max,i;
  GLTexture *t;

  max=0;
  for(i=0;i<TEXTURE_HASH_TABLE_SIZE;i++) {
    t=c->shared_state.texture_hash_table[i];
    while (t!=NULL) {
      if (t->handle>max) max=t->handle;
      t=t->next;
    }

  }
  for(i=0;i<n;i++) {
    textures[i]=max+i+1;
  }
}


inline void  glDeleteTextures(int n, const unsigned int *textures)
{
  GLContext *c=gl_get_context();
  int i;
  GLTexture *t;

  for(i=0;i<n;i++) {
    t=find_texture(c,textures[i]);
    if (t!=NULL && t!=0) {
      if (t==c->current_texture) {
	glBindTexture(GL_TEXTURE_2D,0);
      }
      free_texture(c,textures[i]);
    }
  }
}


inline void  glopBindTexture(GLContext *c,GLParam *p)
{
  
}

void inline  glopTexImage2D(GLContext *c,GLParam *p)
{
 
}


/* TODO: not all tests are done */
inline void  glopTexEnv(GLContext *c,GLParam *p)
{
  int target=p[1].i;
  int pname=p[2].i;
  int param=p[3].i;

  if (target != GL_TEXTURE_ENV) {
  error:
    gl_fatal_error("glTexParameter: unsupported option");
  }

  if (pname != GL_TEXTURE_ENV_MODE) goto error;

  if (param != GL_DECAL) goto error;
}

/* TODO: not all tests are done */
inline void  glopTexParameter(GLContext *c,GLParam *p)
{
  int target=p[1].i;
  int pname=p[2].i;
  int param=p[3].i;
  
  if (target != GL_TEXTURE_2D) {
  error:
    gl_fatal_error("glTexParameter: unsupported option");
  }

  switch(pname) {
  case GL_TEXTURE_WRAP_S:
  case GL_TEXTURE_WRAP_T:
    if (param != GL_REPEAT) goto error;
    break;
  }
}

inline void  glopPixelStore(GLContext *c,GLParam *p)
{
  int pname=p[1].i;
  int param=p[2].i;

  if (pname != GL_UNPACK_ALIGNMENT ||
      param != 1) {
    gl_fatal_error("glPixelStore: unsupported option");
  }
}

#endif