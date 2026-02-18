#include "gl_util.h"
#include <stdio.h>
#include <stdlib.h>

char* slurp_text(const char* path){
    FILE* f = fopen(path, "rb");
    if(!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* s = (char*)malloc((size_t)n + 1);
    if(!s){ fclose(f); return NULL; }
    fread(s, 1, (size_t)n, f);
    s[n] = 0;
    fclose(f);
    return s;
}

static void check_shader(GLuint sh, const char* label){
    GLint ok=0; glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if(!ok){
        GLint len=0; glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        char* msg=(char*)malloc(len+1);
        glGetShaderInfoLog(sh, len, NULL, msg);
        fprintf(stderr, "Shader compile failed (%s):\n%s\n", label, msg);
        free(msg);
        exit(1);
    }
}

static void check_program(GLuint p){
    GLint ok=0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok){
        GLint len=0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        char* msg=(char*)malloc(len+1);
        glGetProgramInfoLog(p, len, NULL, msg);
        fprintf(stderr, "Program link failed:\n%s\n", msg);
        free(msg);
        exit(1);
    }
}

GLuint gl_make_shader(GLenum type, const char* src){
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);
    check_shader(sh, type==GL_VERTEX_SHADER?"VS":"FS");
    return sh;
}

GLuint gl_make_program(const char* vs_src, const char* fs_src){
    GLuint vs=gl_make_shader(GL_VERTEX_SHADER, vs_src);
    GLuint fs=gl_make_shader(GL_FRAGMENT_SHADER, fs_src);
    GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);
    check_program(p);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return p;
}
