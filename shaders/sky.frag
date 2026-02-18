#version 330 core
in vec3 vDir;
out vec4 FragColor;

vec3 skyColor(vec3 dir){
    dir = normalize(dir);
    float t = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 top = vec3(0.04, 0.08, 0.16);
    vec3 mid = vec3(0.10, 0.13, 0.20);
    vec3 bot = vec3(0.01, 0.01, 0.02);
    return mix(bot, mix(mid, top, t), t);
}

void main(){
    FragColor = vec4(skyColor(vDir), 1.0);
}
