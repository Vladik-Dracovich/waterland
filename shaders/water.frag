#version 410 core
in vec3 vWorld;in vec3 vNormal;out vec4 FragColor;
void main(){vec3 N=normalize(vNormal);vec3 L=normalize(vec3(.45,.85,-.3));vec3 V=normalize(vec3(0,9,22)-vWorld);float ndv=clamp(dot(N,V),0.,1.);float fresnel=pow(1.-ndv,5.);float diffuse=max(dot(N,L),0.);vec3 deep=vec3(.008,.075,.14);vec3 shallow=vec3(.015,.24,.30);vec3 water=mix(deep,shallow,clamp(.25+.65*diffuse,0.,1.));vec3 sky=vec3(.46,.69,.86);vec3 H=normalize(L+V);float sparkle=pow(max(dot(N,H),0.),220.)*2.4;vec3 color=mix(water,sky,.10+.70*fresnel)+vec3(1.,.88,.65)*sparkle;FragColor=vec4(pow(color,vec3(1./2.2)),1.);}
