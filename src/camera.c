#include "camera.h"
#include <math.h>

static float clampf(float x, float a, float b){ return x<a?a:(x>b?b:x); }

Camera cam_default(void){
    Camera c;
    c.pos = v3(0.0f, 2.0f, 6.0f);
    c.yaw = -1.57f;
    c.pitch = -0.15f;
    c.speed = 6.0f;
    c.sens = 0.0025f;
    return c;
}

vec3 cam_forward(const Camera* c){
    float cy = cosf(c->yaw), sy = sinf(c->yaw);
    float cp = cosf(c->pitch), sp = sinf(c->pitch);
    return v3_norm(v3(cy*cp, sp, sy*cp));
}

vec3 cam_right(const Camera* c){
    return v3_norm(v3_cross(cam_forward(c), v3(0,1,0)));
}

mat4 cam_view(const Camera* c){
    vec3 f = cam_forward(c);
    return m4_lookat(c->pos, v3_add(c->pos, f), v3(0,1,0));
}

void cam_apply_mouse(Camera* c, float dx, float dy){
    c->yaw += dx * c->sens;
    c->pitch += dy * c->sens;
    c->pitch = clampf(c->pitch, -1.55f, 1.55f);
}

Camera cam_mirrored_y0(const Camera* c){
    // Mirror across plane y=0: y' = -y
    // Yaw remains; pitch flips sign.
    Camera m = *c;
    m.pos.y = -c->pos.y;
    m.pitch = -c->pitch;
    return m;
}
