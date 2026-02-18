#pragma once
#include <GL/glew.h>

typedef struct {
    GLuint fbo;
    GLuint color;
    GLuint depth; // depth texture (optional)
    int w, h;
} FBO;

FBO fbo_make_color_depth(int w, int h);  // color + depth texture
FBO fbo_make_color_rbo(int w, int h);    // color + depth renderbuffer
void fbo_bind(const FBO* f);
void fbo_unbind(void);
void fbo_free(FBO* f);
