#pragma once
#include <stdbool.h>

typedef struct { float x,y,z; } vec3;
typedef struct { float m[16]; } mat4;

vec3 v3(float x, float y, float z);
vec3 v3_add(vec3 a, vec3 b);
vec3 v3_sub(vec3 a, vec3 b);
vec3 v3_mul(vec3 a, float s);
float v3_dot(vec3 a, vec3 b);
vec3 v3_cross(vec3 a, vec3 b);
float v3_len(vec3 a);
vec3 v3_norm(vec3 a);

mat4 m4_identity(void);
mat4 m4_mul(mat4 a, mat4 b);
mat4 m4_translate(vec3 t);
mat4 m4_scale(vec3 s);
mat4 m4_rotate_y(float rad);
mat4 m4_rotate_x(float rad);
mat4 m4_perspective(float fovy_rad, float aspect, float znear, float zfar);
mat4 m4_lookat(vec3 eye, vec3 center, vec3 up);

vec3 m4_mul_point(mat4 m, vec3 p);
