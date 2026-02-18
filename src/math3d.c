#include "math3d.h"
#include <math.h>

vec3 v3(float x, float y, float z){ return (vec3){x,y,z}; }
vec3 v3_add(vec3 a, vec3 b){ return v3(a.x+b.x,a.y+b.y,a.z+b.z); }
vec3 v3_sub(vec3 a, vec3 b){ return v3(a.x-b.x,a.y-b.y,a.z-b.z); }
vec3 v3_mul(vec3 a, float s){ return v3(a.x*s,a.y*s,a.z*s); }
float v3_dot(vec3 a, vec3 b){ return a.x*b.x+a.y*b.y+a.z*b.z; }
vec3 v3_cross(vec3 a, vec3 b){
    return v3(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}
float v3_len(vec3 a){ return sqrtf(v3_dot(a,a)); }
vec3 v3_norm(vec3 a){
    float l=v3_len(a);
    return (l>1e-8f)? v3_mul(a,1.0f/l) : v3(0,0,0);
}

mat4 m4_identity(void){
    mat4 m = {0};
    m.m[0]=m.m[5]=m.m[10]=m.m[15]=1.0f;
    return m;
}

mat4 m4_mul(mat4 a, mat4 b){
    mat4 r = {0};
    for(int c=0;c<4;c++){
        for(int r0=0;r0<4;r0++){
            r.m[c*4+r0] =
                a.m[0*4+r0]*b.m[c*4+0] +
                a.m[1*4+r0]*b.m[c*4+1] +
                a.m[2*4+r0]*b.m[c*4+2] +
                a.m[3*4+r0]*b.m[c*4+3];
        }
    }
    return r;
}

mat4 m4_translate(vec3 t){
    mat4 m = m4_identity();
    m.m[12]=t.x; m.m[13]=t.y; m.m[14]=t.z;
    return m;
}

mat4 m4_scale(vec3 s){
    mat4 m = {0};
    m.m[0]=s.x; m.m[5]=s.y; m.m[10]=s.z; m.m[15]=1.0f;
    return m;
}

mat4 m4_rotate_y(float r){
    mat4 m = m4_identity();
    float c=cosf(r), s=sinf(r);
    m.m[0]=c;  m.m[8]=s;
    m.m[2]=-s; m.m[10]=c;
    return m;
}
mat4 m4_rotate_x(float r){
    mat4 m = m4_identity();
    float c=cosf(r), s=sinf(r);
    m.m[5]=c;  m.m[9]=-s;
    m.m[6]=s;  m.m[10]=c;
    return m;
}

mat4 m4_perspective(float fovy, float aspect, float znear, float zfar){
    float f = 1.0f/tanf(fovy*0.5f);
    mat4 m = {0};
    m.m[0]=f/aspect;
    m.m[5]=f;
    m.m[10]=(zfar+znear)/(znear-zfar);
    m.m[11]=-1.0f;
    m.m[14]=(2.0f*zfar*znear)/(znear-zfar);
    return m;
}

mat4 m4_lookat(vec3 eye, vec3 center, vec3 up){
    vec3 f = v3_norm(v3_sub(center, eye));
    vec3 s = v3_norm(v3_cross(f, up));
    vec3 u = v3_cross(s, f);

    mat4 m = m4_identity();
    m.m[0]=s.x; m.m[4]=s.y; m.m[8]=s.z;
    m.m[1]=u.x; m.m[5]=u.y; m.m[9]=u.z;
    m.m[2]=-f.x; m.m[6]=-f.y; m.m[10]=-f.z;
    m.m[12]=-v3_dot(s, eye);
    m.m[13]=-v3_dot(u, eye);
    m.m[14]= v3_dot(f, eye);
    return m;
}

vec3 m4_mul_point(mat4 m, vec3 p){
    float x = m.m[0]*p.x + m.m[4]*p.y + m.m[8]*p.z + m.m[12];
    float y = m.m[1]*p.x + m.m[5]*p.y + m.m[9]*p.z + m.m[13];
    float z = m.m[2]*p.x + m.m[6]*p.y + m.m[10]*p.z + m.m[14];
    float w = m.m[3]*p.x + m.m[7]*p.y + m.m[11]*p.z + m.m[15];
    if (fabsf(w) > 1e-8f){ x/=w; y/=w; z/=w; }
    return v3(x,y,z);
}
