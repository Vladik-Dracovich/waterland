#version 330 core
layout(location=0) in vec3 aPos;
out vec3 vDir;
uniform mat4 uInvVP;

void main(){
    // Fullscreen triangle technique: aPos in clip space
    vec4 p = vec4(aPos, 1.0);
    vec4 w = uInvVP * p;
    vDir = w.xyz / w.w;
    gl_Position = p;
}
