#pragma once
#include <GL/glew.h>

GLuint gl_make_shader(GLenum type, const char* src);
GLuint gl_make_program(const char* vs_src, const char* fs_src);
char*  slurp_text(const char* path);
