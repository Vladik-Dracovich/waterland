#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNor;
layout(location=2) in vec2 aUV;

uniform mat4 uMVP;
uniform mat4 uModel;
uniform float uTime;

out vec3 vPosW;
out vec3 vNorW;
out vec2 vUV;

struct Wave { vec2 dir; float amp; float wl; float spd; };
const int WN = 4;
Wave waves[WN] = Wave[WN](
    Wave(normalize(vec2( 1.0, 0.2)), 0.16, 3.8, 1.2),
    Wave(normalize(vec2(-0.4, 0.9)), 0.10, 2.1, 1.6),
    Wave(normalize(vec2( 0.2,-1.0)), 0.06, 1.2, 2.2),
    Wave(normalize(vec2( 0.8,-0.3)), 0.04, 0.8, 3.0)
);

vec3 gerstner(vec3 p, out vec3 N){
    vec3 pos = p;
    vec3 n = vec3(0.0, 1.0, 0.0);
    for(int i=0;i<WN;i++){
        float k = 6.2831853 / waves[i].wl;
        float c = waves[i].spd;
        float d = dot(waves[i].dir, pos.xz);
        float ph = k * d + c * uTime;

        float a = waves[i].amp;
        float s = sin(ph);
        float co = cos(ph);

        // Displace (x,z) a bit too
        pos.x += waves[i].dir.x * (a * co);
        pos.z += waves[i].dir.y * (a * co);
        pos.y += a * s;

        // Normal approx
        n.x += -waves[i].dir.x * (a * k * s);
        n.z += -waves[i].dir.y * (a * k * s);
        n.y +=  a * k * co;
    }
    N = normalize(n);
    return pos;
}

void main(){
    vUV = aUV * 12.0;
    vec3 N;
    vec3 displaced = gerstner(aPos, N);

    vec4 wpos = uModel * vec4(displaced, 1.0);
    vPosW = wpos.xyz;
    vNorW = mat3(uModel) * N;

    gl_Position = uMVP * vec4(displaced, 1.0);
}
