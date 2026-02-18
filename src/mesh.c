#include "mesh.h"
#include "noise.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    float px, py, pz;
    float nx, ny, nz;
    float u, v;
} Vtx;

static void compute_normals(Vtx* v, int nx, int nz){
    // Simple central-diff normals on grid (height in py)
    for(int z=0; z<nz; z++){
        for(int x=0; x<nx; x++){
            int i = z*nx + x;
            int xm = x>0 ? x-1 : x;
            int xp = x<nx-1 ? x+1 : x;
            int zm = z>0 ? z-1 : z;
            int zp = z<nz-1 ? z+1 : z;

            float hl = v[z*nx + xm].py;
            float hr = v[z*nx + xp].py;
            float hd = v[zm*nx + x].py;
            float hu = v[zp*nx + x].py;

            float dx = hr - hl;
            float dz = hu - hd;

            // up vector is ( -dx, 2, -dz )
            float nxv = -dx;
            float nyv = 2.0f;
            float nzv = -dz;
            float len = 1.0f / (float)sqrt(nxv*nxv + nyv*nyv + nzv*nzv + 1e-8f);
            v[i].nx = nxv*len; v[i].ny = nyv*len; v[i].nz = nzv*len;
        }
    }
}

static Mesh mesh_from_grid(Vtx* verts, int vcount, unsigned* idx, int icount){
    Mesh m = {0};
    m.index_count = icount;

    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);

    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vtx)*vcount, verts, GL_STATIC_DRAW);

    glGenBuffers(1, &m.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*icount, idx, GL_STATIC_DRAW);

    // layout: pos(3) normal(3) uv(2)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vtx), (void*)(6*sizeof(float)));

    glBindVertexArray(0);
    return m;
}

Mesh mesh_make_grid(int nx, int nz, float sx, float sz){
    int vcount = nx*nz;
    int quads = (nx-1)*(nz-1);
    int icount = quads*6;

    Vtx* v = (Vtx*)calloc((size_t)vcount, sizeof(Vtx));
    unsigned* idx = (unsigned*)malloc(sizeof(unsigned)*(size_t)icount);

    for(int z=0; z<nz; z++){
        for(int x=0; x<nx; x++){
            int i = z*nx + x;
            float fx = (float)x/(float)(nx-1);
            float fz = (float)z/(float)(nz-1);
            v[i].px = (fx-0.5f)*sx;
            v[i].py = 0.0f;
            v[i].pz = (fz-0.5f)*sz;
            v[i].nx = 0.0f; v[i].ny = 1.0f; v[i].nz = 0.0f;
            v[i].u = fx; v[i].v = fz;
        }
    }

    int k=0;
    for(int z=0; z<nz-1; z++){
        for(int x=0; x<nx-1; x++){
            unsigned i0 = (unsigned)(z*nx + x);
            unsigned i1 = (unsigned)(z*nx + x+1);
            unsigned i2 = (unsigned)((z+1)*nx + x);
            unsigned i3 = (unsigned)((z+1)*nx + x+1);
            idx[k++] = i0; idx[k++] = i2; idx[k++] = i1;
            idx[k++] = i1; idx[k++] = i2; idx[k++] = i3;
        }
    }

    Mesh m = mesh_from_grid(v, vcount, idx, icount);
    free(v); free(idx);
    return m;
}

Mesh mesh_make_terrain(int nx, int nz, float sx, float sz){
    int vcount = nx*nz;
    int quads = (nx-1)*(nz-1);
    int icount = quads*6;

    Vtx* v = (Vtx*)calloc((size_t)vcount, sizeof(Vtx));
    unsigned* idx = (unsigned*)malloc(sizeof(unsigned)*(size_t)icount);

    for(int z=0; z<nz; z++){
        for(int x=0; x<nx; x++){
            int i = z*nx + x;
            float fx = (float)x/(float)(nx-1);
            float fz = (float)z/(float)(nz-1);
            float wx = (fx-0.5f)*sx;
            float wz = (fz-0.5f)*sz;

            float h = fbm2(fx*3.0f, fz*3.0f, 5);
            h = (h*h) * 6.0f; // bias: more lowlands, some hills
            // push shoreline: keep center flatter to show water
            float r2 = wx*wx + wz*wz;
            h *= (r2 > 30.0f ? 1.0f : 0.35f);

            v[i].px = wx;
            v[i].py = h;
            v[i].pz = wz;
            v[i].u = fx*8.0f;
            v[i].v = fz*8.0f;
        }
    }

    compute_normals(v, nx, nz);

    int k=0;
    for(int z=0; z<nz-1; z++){
        for(int x=0; x<nx-1; x++){
            unsigned i0 = (unsigned)(z*nx + x);
            unsigned i1 = (unsigned)(z*nx + x+1);
            unsigned i2 = (unsigned)((z+1)*nx + x);
            unsigned i3 = (unsigned)((z+1)*nx + x+1);
            idx[k++] = i0; idx[k++] = i2; idx[k++] = i1;
            idx[k++] = i1; idx[k++] = i2; idx[k++] = i3;
        }
    }

    Mesh m = mesh_from_grid(v, vcount, idx, icount);
    free(v); free(idx);
    return m;
}

void mesh_draw(const Mesh* m){
    glBindVertexArray(m->vao);
    glDrawElements(GL_TRIANGLES, m->index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void mesh_free(Mesh* m){
    if(m->ebo) glDeleteBuffers(1, &m->ebo);
    if(m->vbo) glDeleteBuffers(1, &m->vbo);
    if(m->vao) glDeleteVertexArrays(1, &m->vao);
    *m = (Mesh){0};
}
