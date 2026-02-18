#version 330 core
in vec3 vNor;
in vec3 vPosW;
out vec4 FragColor;

uniform vec3 uSunDir;
uniform vec3 uCamPos;

vec3 skyColor(vec3 dir){
    float t = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 top = vec3(0.05, 0.10, 0.18);
    vec3 mid = vec3(0.10, 0.15, 0.22);
    vec3 bot = vec3(0.02, 0.02, 0.03);
    return mix(bot, mix(mid, top, t), t);
}

void main(){
    vec3 N = normalize(vNor);
    float ndl = max(dot(N, normalize(uSunDir)), 0.0);
    float amb = 0.25;

    // Height-based terrain tint
    float h = vPosW.y;
    vec3 low = vec3(0.06, 0.08, 0.06);
    vec3 high = vec3(0.18, 0.18, 0.16);
    vec3 base = mix(low, high, clamp(h/8.0, 0.0, 1.0));

    // A touch of sky ambient
    vec3 V = normalize(uCamPos - vPosW);
    vec3 sky = skyColor(V);

    vec3 col = base * (amb + ndl) + 0.15 * sky;
    FragColor = vec4(col, 1.0);
}
