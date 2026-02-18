#version 330 core
in vec3 vPosW;
in vec3 vNorW;
in vec2 vUV;
out vec4 FragColor;

uniform vec3 uCamPos;
uniform vec3 uSunDir;

uniform sampler2D uReflection;
uniform sampler2D uRefraction;
uniform sampler2D uRefractDepth;

uniform mat4 uVP;
uniform vec2 uViewport;

uniform float uTime;

// Convert world pos -> screen UV (0..1)
vec2 screenUV(vec3 wpos){
    vec4 clip = uVP * vec4(wpos, 1.0);
    vec3 ndc = clip.xyz / clip.w;
    return ndc.xy * 0.5 + 0.5;
}

float linearizeDepth(float d, float znear, float zfar){
    // d is depth buffer value 0..1
    float z = d * 2.0 - 1.0;
    return (2.0 * znear * zfar) / (zfar + znear - z * (zfar - znear));
}

vec3 skyColor(vec3 dir){
    dir = normalize(dir);
    float t = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 top = vec3(0.04, 0.08, 0.16);
    vec3 mid = vec3(0.10, 0.13, 0.20);
    vec3 bot = vec3(0.01, 0.01, 0.02);
    return mix(bot, mix(mid, top, t), t);
}

void main(){
    vec3 N = normalize(vNorW);
    vec3 V = normalize(uCamPos - vPosW);

    // Fresnel (Schlick)
    float F0 = 0.02;
    float fres = F0 + (1.0 - F0) * pow(1.0 - max(dot(N,V), 0.0), 5.0);

    // Small normal-based distortion for refraction
    vec2 uv = screenUV(vPosW);
    vec2 dist = (N.xz) * 0.015;

    vec2 uv_refr = uv + dist;
    vec2 uv_refl = vec2(uv.x, 1.0 - uv.y) + dist * 0.6;

    vec3 refl = texture(uReflection, uv_refl).rgb;
    vec3 refr = texture(uRefraction, uv_refr).rgb;

    // Depth-based absorption (uses refraction depth, compare against water depth)
    float znear = 0.1;
    float zfar  = 200.0;

    float d_scene = texture(uRefractDepth, uv).r;
    float sceneZ = linearizeDepth(d_scene, znear, zfar);

    // approximate water surface depth:
    float d_water = gl_FragCoord.z;
    float waterZ = linearizeDepth(d_water, znear, zfar);

    float depthDiff = max(sceneZ - waterZ, 0.0);
    float absorb = 1.0 - exp(-depthDiff * 0.18);

    vec3 deep = vec3(0.01, 0.05, 0.08);
    vec3 shallow = vec3(0.02, 0.12, 0.14);
    vec3 waterTint = mix(shallow, deep, clamp(absorb, 0.0, 1.0));

    // Foam near shoreline (thin band)
    float foam = smoothstep(0.0, 0.8, 1.0 - clamp(depthDiff, 0.0, 1.2));
    foam = foam*foam * 0.35;

    // Sun specular
    vec3 L = normalize(uSunDir);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 120.0) * 0.8;

    vec3 base = mix(refr, refl, fres);
    base = mix(base, waterTint, 0.55);
    base += spec;
    base += foam;

    FragColor = vec4(base, 1.0);
}
