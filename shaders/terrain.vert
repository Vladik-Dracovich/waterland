#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNor;
layout(location=2) in vec2 aUV;

uniform mat4 uMVP;
uniform mat4 uModel;

out vec3 vNor;
out vec3 vPosW;

void main(){
    vec4 wpos = uModel * vec4(aPos, 1.0);
    vPosW = wpos.xyz;
    vNor = mat3(uModel) * aNor;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
