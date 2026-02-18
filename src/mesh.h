#pragma once
#include <GL/glew.h>

typedef struct {
    GLuint vao, vbo, ebo;
    int index_count;
} Mesh;

Mesh mesh_make_grid(int nx, int nz, float sx, float sz);
Mesh mesh_make_terrain(int nx, int nz, float sx, float sz);
void mesh_draw(const Mesh* m);
void mesh_free(Mesh* m);
