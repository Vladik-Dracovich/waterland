#version 410 core
layout(location=0) in vec2 aXZ;
uniform mat4 uVP;
uniform float uTime;
out vec3 vWorld;
out vec3 vNormal;
// Four analytic Gerstner waves with derivatives for stable per-pixel lighting.
void wave(inout vec3 p, inout vec3 dx, inout vec3 dz, vec2 dir, float amplitude, float wavelength, float speed, float steepness){
 dir=normalize(dir);float k=6.2831853/wavelength;float phase=k*dot(dir,p.xz)-speed*uTime;float s=sin(phase),c=cos(phase);float q=steepness/k;
 p.xz+=q*dir*c;p.y+=amplitude*s;
 dx+=vec3(-q*k*dir.x*dir.x*s,amplitude*k*dir.x*c,-q*k*dir.x*dir.y*s);
 dz+=vec3(-q*k*dir.x*dir.y*s,amplitude*k*dir.y*c,-q*k*dir.y*dir.y*s);
}
void main(){vec3 p=vec3(aXZ.x,0,aXZ.y);vec3 dx=vec3(1,0,0),dz=vec3(0,0,1);
 wave(p,dx,dz,vec2(1,.4),.85,19.,1.4,.23);
 wave(p,dx,dz,vec2(-.3,1),.40,10.,2.1,.18);
 wave(p,dx,dz,vec2(.8,-.6),.20,5.5,3.0,.13);
 wave(p,dx,dz,vec2(-1,-.2),.09,2.8,4.3,.08);
 vWorld=p;vNormal=normalize(cross(dz,dx));gl_Position=uVP*vec4(p,1);}
