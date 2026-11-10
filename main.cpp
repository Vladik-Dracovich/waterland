#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
static std::string read(const char* path) { std::ifstream f(path); if(!f) throw std::runtime_error(std::string("Cannot open ")+path); std::stringstream s; s<<f.rdbuf(); return s.str(); }
static GLuint shader(GLenum type,const std::string& src){GLuint s=glCreateShader(type);const char* p=src.c_str();glShaderSource(s,1,&p,nullptr);glCompileShader(s);GLint ok;glGetShaderiv(s,GL_COMPILE_STATUS,&ok);if(!ok){char log[4096];glGetShaderInfoLog(s,sizeof(log),nullptr,log);throw std::runtime_error(log);}return s;}
int main(){try{
if(!glfwInit()) throw std::runtime_error("GLFW initialization failed");
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
GLFWwindow* w=glfwCreateWindow(1280,720,"Water Renderer",nullptr,nullptr);if(!w)throw std::runtime_error("OpenGL context creation failed");glfwMakeContextCurrent(w);glfwSwapInterval(1);
GLuint vs=shader(GL_VERTEX_SHADER,read("shaders/water.vert")),fs=shader(GL_FRAGMENT_SHADER,read("shaders/water.frag"));GLuint prog=glCreateProgram();glAttachShader(prog,vs);glAttachShader(prog,fs);glLinkProgram(prog);GLint linked;glGetProgramiv(prog,GL_LINK_STATUS,&linked);if(!linked){char log[4096];glGetProgramInfoLog(prog,sizeof(log),nullptr,log);throw std::runtime_error(log);}glDeleteShader(vs);glDeleteShader(fs);
const int N=220;const float SIZE=360.0f;std::vector<float> verts;std::vector<unsigned> inds;for(int z=0;z<=N;z++)for(int x=0;x<=N;x++){verts.push_back((float(x)/N-.5f)*SIZE);verts.push_back((float(z)/N-.5f)*SIZE);}for(int z=0;z<N;z++)for(int x=0;x<N;x++){unsigned a=z*(N+1)+x,b=a+1,c=a+N+1,d=c+1;inds.insert(inds.end(),{a,c,b,b,c,d});}
GLuint vao,vbo,ebo;glGenVertexArrays(1,&vao);glBindVertexArray(vao);glGenBuffers(1,&vbo);glBindBuffer(GL_ARRAY_BUFFER,vbo);glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(float),verts.data(),GL_STATIC_DRAW);glGenBuffers(1,&ebo);glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);glBufferData(GL_ELEMENT_ARRAY_BUFFER,inds.size()*sizeof(unsigned),inds.data(),GL_STATIC_DRAW);glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);glEnableVertexAttribArray(0);
glEnable(GL_DEPTH_TEST);float yaw=-90.0f,pitch=-13.0f;glm::vec3 pos(0,8,22);double lx=0,ly=0;bool first=true;glfwSetInputMode(w,GLFW_CURSOR,GLFW_CURSOR_DISABLED);double prev=glfwGetTime();while(!glfwWindowShouldClose(w)){double now=glfwGetTime();float dt=std::min(float(now-prev),.05f);prev=now;glfwPollEvents();if(glfwGetKey(w,GLFW_KEY_ESCAPE)==GLFW_PRESS)glfwSetWindowShouldClose(w,1);double mx,my;glfwGetCursorPos(w,&mx,&my);if(first){lx=mx;ly=my;first=false;}yaw+=float(mx-lx)*.09f;pitch-=float(my-ly)*.09f;lx=mx;ly=my;pitch=glm::clamp(pitch,-85.f,85.f);glm::vec3 forward=glm::normalize(glm::vec3(cos(glm::radians(yaw))*cos(glm::radians(pitch)),sin(glm::radians(pitch)),sin(glm::radians(yaw))*cos(glm::radians(pitch))));glm::vec3 right=glm::normalize(glm::cross(forward,glm::vec3(0,1,0)));float speed=glfwGetKey(w,GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS?24.f:10.f;if(glfwGetKey(w,GLFW_KEY_W)==GLFW_PRESS)pos+=forward*speed*dt;if(glfwGetKey(w,GLFW_KEY_S)==GLFW_PRESS)pos-=forward*speed*dt;if(glfwGetKey(w,GLFW_KEY_A)==GLFW_PRESS)pos-=right*speed*dt;if(glfwGetKey(w,GLFW_KEY_D)==GLFW_PRESS)pos+=right*speed*dt;if(glfwGetKey(w,GLFW_KEY_SPACE)==GLFW_PRESS)pos.y+=speed*dt;if(glfwGetKey(w,GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS)pos.y-=speed*dt;
int width,height;glfwGetFramebufferSize(w,&width,&height);if(width==0||height==0)continue;glViewport(0,0,width,height);glClearColor(.49f,.70f,.84f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);glm::mat4 projection=glm::perspective(glm::radians(65.f),float(width)/height,.1f,900.f);glm::mat4 view=glm::lookAt(pos,pos+forward,glm::vec3(0,1,0));glUseProgram(prog);glUniformMatrix4fv(glGetUniformLocation(prog,"uVP"),1,GL_FALSE,glm::value_ptr(projection*view));glUniform1f(glGetUniformLocation(prog,"uTime"),float(now));glBindVertexArray(vao);glDrawElements(GL_TRIANGLES,(GLsizei)inds.size(),GL_UNSIGNED_INT,nullptr);glfwSwapBuffers(w);}
glDeleteBuffers(1,&ebo);glDeleteBuffers(1,&vbo);glDeleteVertexArrays(1,&vao);glDeleteProgram(prog);glfwDestroyWindow(w);glfwTerminate();return 0;
}catch(const std::exception& e){std::cerr<<"Error: "<<e.what()<<'\n';glfwTerminate();return 1;}}
