#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gl_util.h"
#include "math3d.h"
#include "camera.h"
#include "mesh.h"
#include "fbo.h"

static int win_w = 1280, win_h = 720;
static int first_mouse = 1;
static double last_mx=0, last_my=0;
static int capture_mouse = 1;

static void die(const char* msg){ fprintf(stderr, "%s\n", msg); exit(1); }

static void glfw_err(int code, const char* desc){
    fprintf(stderr, "GLFW error %d: %s\n", code, desc);
}

static void resize_cb(GLFWwindow* w, int x, int y){
    (void)w; win_w = x; win_h = y;
}

static void key_cb(GLFWwindow* w, int key, int sc, int action, int mods){
    (void)sc; (void)mods;
    if(action == GLFW_PRESS){
        if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(w, 1);
        if(key == GLFW_KEY_M){
            capture_mouse = !capture_mouse;
            glfwSetInputMode(w, GLFW_CURSOR, capture_mouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            first_mouse = 1;
        }
    }
}

static void mouse_cb(GLFWwindow* w, double x, double y){
    (void)w;
    if(!capture_mouse) return;
    if(first_mouse){ last_mx=x; last_my=y; first_mouse=0; return; }
    double dx = x - last_mx;
    double dy = y - last_my;
    last_mx=x; last_my=y;
    // camera updated in loop (store deltas global? simplest: static)
}

static void set_u_mat4(GLuint p, const char* name, mat4 m){
    GLint loc = glGetUniformLocation(p, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, m.m);
}
static void set_u_v3(GLuint p, const char* name, vec3 v){
    GLint loc = glGetUniformLocation(p, name);
    glUniform3f(loc, v.x, v.y, v.z);
}
static void set_u_f(GLuint p, const char* name, float v){
    GLint loc = glGetUniformLocation(p, name);
    glUniform1f(loc, v);
}
static void set_u_i(GLuint p, const char* name, int v){
    GLint loc = glGetUniformLocation(p, name);
    glUniform1i(loc, v);
}
static void set_u_v2(GLuint p, const char* name, float x, float y){
    GLint loc = glGetUniformLocation(p, name);
    glUniform2f(loc, x, y);
}

static GLuint make_fullscreen_triangle(void){
    float tri[] = {
        -1.f,-1.f,0.f,
         3.f,-1.f,0.f,
        -1.f, 3.f,0.f
    };
    GLuint vao,vbo;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(tri),tri,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glBindVertexArray(0);
    return vao;
}

static void draw_scene(GLuint prog_terrain, GLuint prog_sky, GLuint sky_vao,
                       const Mesh* terrain,
                       mat4 VP, mat4 invVP, vec3 camPos)
{
    // Sky first (no depth write)
    glDepthMask(GL_FALSE);
    glUseProgram(prog_sky);
    set_u_mat4(prog_sky, "uInvVP", invVP);
    glBindVertexArray(sky_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    // Terrain
    glUseProgram(prog_terrain);
    mat4 M = m4_identity();
    mat4 MVP = m4_mul(VP, M);
    set_u_mat4(prog_terrain, "uModel", M);
    set_u_mat4(prog_terrain, "uMVP", MVP);
    set_u_v3(prog_terrain, "uCamPos", camPos);
    set_u_v3(prog_terrain, "uSunDir", v3_norm(v3(-0.4f, 0.9f, -0.2f)));
    mesh_draw(terrain);
}

int main(void){
    glfwSetErrorCallback(glfw_err);
    if(!glfwInit()) die("glfwInit failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* w = glfwCreateWindow(win_w, win_h, "WaterLand-C", NULL, NULL);
    if(!w) die("glfwCreateWindow failed");
    glfwMakeContextCurrent(w);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(w, resize_cb);
    glfwSetKeyCallback(w, key_cb);

    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLenum err = glewInit();
    if(err != GLEW_OK) die("glewInit failed");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    char* terrain_vs = slurp_text("shaders/terrain.vert");
    char* terrain_fs = slurp_text("shaders/terrain.frag");
    char* water_vs   = slurp_text("shaders/water.vert");
    char* water_fs   = slurp_text("shaders/water.frag");
    char* sky_vs     = slurp_text("shaders/sky.vert");
    char* sky_fs     = slurp_text("shaders/sky.frag");
    if(!terrain_vs||!terrain_fs||!water_vs||!water_fs||!sky_vs||!sky_fs) die("Failed to load shaders");

    GLuint prog_terrain = gl_make_program(terrain_vs, terrain_fs);
    GLuint prog_water   = gl_make_program(water_vs, water_fs);
    GLuint prog_sky     = gl_make_program(sky_vs, sky_fs);
    free(terrain_vs); free(terrain_fs); free(water_vs); free(water_fs); free(sky_vs); free(sky_fs);

    Mesh terrain = mesh_make_terrain(256, 256, 80.0f, 80.0f);
    Mesh water   = mesh_make_grid(256, 256, 120.0f, 120.0f);

    GLuint sky_vao = make_fullscreen_triangle();

    // FBOs
    int refl_w = 1024, refl_h = 1024;
    int refr_w = 1024, refr_h = 1024;
    FBO fbo_reflection = fbo_make_color_rbo(refl_w, refl_h);
    FBO fbo_refraction = fbo_make_color_depth(refr_w, refr_h); // includes depth texture

    Camera cam = cam_default();

    double last_t = glfwGetTime();
    double mx,my;
    glfwGetCursorPos(w, &mx, &my);
    last_mx = mx; last_my = my;

    while(!glfwWindowShouldClose(w)){
        double t = glfwGetTime();
        float dt = (float)(t - last_t);
        last_t = t;

        // Mouse delta
        glfwGetCursorPos(w, &mx, &my);
        double dx = mx - last_mx;
        double dy = my - last_my;
        last_mx = mx; last_my = my;
        if(capture_mouse){
            cam_apply_mouse(&cam, (float)dx, (float)dy);
        }

        // Movement
        vec3 fwd = cam_forward(&cam);
        vec3 right = cam_right(&cam);
        vec3 up = v3(0,1,0);

        float sp = cam.speed * (glfwGetKey(w, GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS ? 2.0f : 1.0f);

        if(glfwGetKey(w, GLFW_KEY_W)==GLFW_PRESS) cam.pos = v3_add(cam.pos, v3_mul(fwd, sp*dt));
        if(glfwGetKey(w, GLFW_KEY_S)==GLFW_PRESS) cam.pos = v3_sub(cam.pos, v3_mul(fwd, sp*dt));
        if(glfwGetKey(w, GLFW_KEY_D)==GLFW_PRESS) cam.pos = v3_add(cam.pos, v3_mul(right, sp*dt));
        if(glfwGetKey(w, GLFW_KEY_A)==GLFW_PRESS) cam.pos = v3_sub(cam.pos, v3_mul(right, sp*dt));
        if(glfwGetKey(w, GLFW_KEY_SPACE)==GLFW_PRESS) cam.pos = v3_add(cam.pos, v3_mul(up, sp*dt));
        if(glfwGetKey(w, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS) cam.pos = v3_sub(cam.pos, v3_mul(up, sp*dt));

        // Matrices
        float aspect = (win_h>0) ? (float)win_w/(float)win_h : 1.0f;
        mat4 P = m4_perspective(60.0f*(3.14159f/180.0f), aspect, 0.1f, 200.0f);
        mat4 V = cam_view(&cam);
        mat4 VP = m4_mul(P, V);

        // Inverse VP for sky (cheap approximate: invert only rotation part not done; but ok: use inverse of (P*V) not trivial).
        // We'll hack by using a large far plane direction via inverse of P and V separately (good enough for procedural sky).
        // For simplicity, compute invVP as inverse(P)*inverse(V) with approximate inverses:
        // - inverse(V) for rigid transform is transpose rotation + translate; we skip exact, but sky doesn't need perfection.
        // We'll build invVP using eye and direction basis:
        vec3 F = cam_forward(&cam);
        vec3 R = cam_right(&cam);
        vec3 U = v3_cross(R, F);

        // Build inverse view rotation matrix (columns are R,U,-F)
        mat4 invV = m4_identity();
        invV.m[0]=R.x; invV.m[4]=U.x; invV.m[8]=-F.x;
        invV.m[1]=R.y; invV.m[5]=U.y; invV.m[9]=-F.y;
        invV.m[2]=R.z; invV.m[6]=U.z; invV.m[10]=-F.z;
        invV.m[12]=cam.pos.x; invV.m[13]=cam.pos.y; invV.m[14]=cam.pos.z;

        // Inverse projection for clip->view
        float f = 1.0f/tanf((60.0f*(3.14159f/180.0f))*0.5f);
        mat4 invP = (mat4){0};
        invP.m[0]=aspect/f;
        invP.m[5]=1.0f/f;
        invP.m[11]=(0.1f-200.0f)/(2.0f*0.1f*200.0f);
        invP.m[14]=-1.0f;
        invP.m[15]=(200.0f+0.1f)/(2.0f*0.1f*200.0f);
        // This "invP" is not exact; but good enough to get direction-like vDir.

        mat4 invVP = m4_mul(invV, invP);

        // --- Pass 1: Reflection (mirror camera across water plane y=0) ---
        Camera camR = cam_mirrored_y0(&cam);
        mat4 VR = cam_view(&camR);
        mat4 VPR = m4_mul(P, VR);

        // clip everything below water by flipping face cull? We'll use clip plane in shaders later; for minimal, just render terrain (mostly above water)
        fbo_bind(&fbo_reflection);
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_scene(prog_terrain, prog_sky, sky_vao, &terrain, VPR, invVP, camR.pos);
        fbo_unbind();

        // --- Pass 2: Refraction (normal scene render into texture + depth) ---
        fbo_bind(&fbo_refraction);
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_scene(prog_terrain, prog_sky, sky_vao, &terrain, VP, invVP, cam.pos);
        fbo_unbind();

        // --- Pass 3: Main render to screen ---
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,win_w,win_h);
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw sky + terrain first to screen too (so water distortion has believable context behind it)
        draw_scene(prog_terrain, prog_sky, sky_vao, &terrain, VP, invVP, cam.pos);

        // Draw water last
        glUseProgram(prog_water);
        mat4 Mw = m4_identity(); // water plane at y=0
        mat4 MVPw = m4_mul(VP, Mw);
        set_u_mat4(prog_water, "uModel", Mw);
        set_u_mat4(prog_water, "uMVP", MVPw);
        set_u_mat4(prog_water, "uVP", VP);
        set_u_v3(prog_water, "uCamPos", cam.pos);
        set_u_v3(prog_water, "uSunDir", v3_norm(v3(-0.4f, 0.9f, -0.2f)));
        set_u_f(prog_water, "uTime", (float)t);
        set_u_v2(prog_water, "uViewport", (float)win_w, (float)win_h);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo_reflection.color);
        set_u_i(prog_water, "uReflection", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fbo_refraction.color);
        set_u_i(prog_water, "uRefraction", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, fbo_refraction.depth);
        set_u_i(prog_water, "uRefractDepth", 2);

        mesh_draw(&water);

        glfwSwapBuffers(w);
        glfwPollEvents();
    }

    fbo_free(&fbo_reflection);
    fbo_free(&fbo_refraction);
    mesh_free(&terrain);
    mesh_free(&water);
    glDeleteVertexArrays(1, &sky_vao);
    glDeleteProgram(prog_terrain);
    glDeleteProgram(prog_water);
    glDeleteProgram(prog_sky);

    glfwDestroyWindow(w);
    glfwTerminate();
    return 0;
}
