#include "fbo.h"
#include <stdio.h>
#include <stdlib.h>

static void check_fbo(void){
    GLenum s = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (s != GL_FRAMEBUFFER_COMPLETE){
        fprintf(stderr, "FBO incomplete: 0x%x\n", (unsigned)s);
        exit(1);
    }
}

FBO fbo_make_color_depth(int w, int h){
    FBO f = {0};
    f.w=w; f.h=h;

    glGenFramebuffers(1, &f.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, f.fbo);

    glGenTextures(1, &f.color);
    glBindTexture(GL_TEXTURE_2D, f.color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, f.color, 0);

    glGenTextures(1, &f.depth);
    glBindTexture(GL_TEXTURE_2D, f.depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, f.depth, 0);

    GLenum draw[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw);

    check_fbo();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return f;
}

FBO fbo_make_color_rbo(int w, int h){
    FBO f = {0};
    f.w=w; f.h=h;

    glGenFramebuffers(1, &f.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, f.fbo);

    glGenTextures(1, &f.color);
    glBindTexture(GL_TEXTURE_2D, f.color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, f.color, 0);

    GLuint rbo=0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    // store rbo id in depth field (abuse) to delete later
    f.depth = rbo;

    GLenum draw[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw);

    check_fbo();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return f;
}

void fbo_bind(const FBO* f){
    glBindFramebuffer(GL_FRAMEBUFFER, f->fbo);
    glViewport(0,0,f->w,f->h);
}
void fbo_unbind(void){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void fbo_free(FBO* f){
    if (f->depth){
        // If depth is a renderbuffer for this FBO type, it's attached as RBO.
        // We can't detect which; safest: try delete as texture and renderbuffer; OpenGL ignores wrong delete.
        glDeleteTextures(1, &f->depth);
        glDeleteRenderbuffers(1, &f->depth);
    }
    if (f->color) glDeleteTextures(1, &f->color);
    if (f->fbo) glDeleteFramebuffers(1, &f->fbo);
    *f = (FBO){0};
}
