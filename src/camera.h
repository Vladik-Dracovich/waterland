#pragma once
#include "math3d.h"
#include <stdbool.h>

typedef struct {
    vec3 pos;
    float yaw;   // radians
    float pitch; // radians
    float speed;
    float sens;
} Camera;

Camera cam_default(void);
mat4 cam_view(const Camera* c);
vec3 cam_forward(const Camera* c);
vec3 cam_right(const Camera* c);
void cam_apply_mouse(Camera* c, float dx, float dy);
Camera cam_mirrored_y0(const Camera* c); // mirror across water plane y=0
