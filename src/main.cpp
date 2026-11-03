#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Lighting.h"
#include "Texture.h"
#include "ServerEffects.h"
#include "PerformanceStats.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char** argv) {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
    GLFWwindow* w=glfwCreateWindow(1280,720,"Server Room | A: alarm | L: ceiling | F: fog | Esc: exit",nullptr,nullptr);
    if (!w) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(w);
    glfwSetInputMode(w,GLFW_STICKY_KEYS,GLFW_TRUE);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {glfwDestroyWindow(w); glfwTerminate(); return 1;}
    int result=0;
    try {
        Shader lit(SHADER_DIR "/lit.vert",SHADER_DIR "/lit.frag");
        Shader led(SHADER_DIR "/led.vert",SHADER_DIR "/led.frag");
        PerformanceStats stats;
        Lighting lighting;
        Texture floorTex(TEXTURE_DIR "/floor.pgm"), metalTex(TEXTURE_DIR "/metal.pgm");
        Texture wallTex(TEXTURE_DIR "/wall.pgm"), panelTex(TEXTURE_DIR "/panel.pgm");
        Material floor{floorTex,.50f,.56f,.61f,1.f,28.f,.18f};
        Material wall{wallTex,.62f,.68f,.70f,.6f,12.f,.08f};
        Material metal{metalTex,.25f,.32f,.38f,1.f,80.f,.52f};
        Material panel{panelTex,.42f,.48f,.53f,2.f,44.f,.32f};
        vector<float> vertices;
        // Six separate faces preserve hard normals on the demonstration boxes.
        for(int axis=0;axis<3;axis++) for(int sign: {-1,1}) {
            const int indices[]={0,1,2,0,2,3};
            const float corners[][2]={{-0.5f,-0.5f},{0.5f,-0.5f},{0.5f,0.5f},{-0.5f,0.5f}};
            for(int i:indices) {
                glm::vec3 p(0),n(0); p[axis]=sign*0.5f; n[axis]=float(sign);
                p[(axis+1)%3]=corners[i][0]; p[(axis+2)%3]=corners[i][1];
                for(int j=0;j<3;j++) vertices.push_back(p[j]);
                for(int j=0;j<3;j++) vertices.push_back(n[j]);
            }
        }
        GLuint vao,vbo;
        glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo); glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float),vertices.data(),GL_STATIC_DRAW);
        for(int i=0;i<2;i++){glEnableVertexAttribArray(i);glVertexAttribPointer(i,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(i*3*sizeof(float)));}
        glEnable(GL_DEPTH_TEST);
        bool lastA=false,lastL=false,lastF=false,fog=true;
        int frames=0;
        const bool smoke=argc>1 && string(argv[1])=="--smoke-test";
        double previous=glfwGetTime(),started=previous;
        while(!glfwWindowShouldClose(w)){
            glfwPollEvents();
            bool a=glfwGetKey(w,GLFW_KEY_A)==GLFW_PRESS,l=glfwGetKey(w,GLFW_KEY_L)==GLFW_PRESS;
            if(a&&!lastA)lighting.toggleAlarm();
            if(l&&!lastL)lighting.toggleCeiling();
            bool f=glfwGetKey(w,GLFW_KEY_F)==GLFW_PRESS;
            if(f&&!lastF)fog=!fog;
            lastF=f;lastA=a;lastL=l;
            if(glfwGetKey(w,GLFW_KEY_ESCAPE)==GLFW_PRESS)break;
            int width,height;glfwGetFramebufferSize(w,&width,&height);
            if(width<=0 || height<=0){glfwWaitEventsTimeout(.1);previous=glfwGetTime();continue;}
            glViewport(0,0,width,height);
            glClearColor(0.008f,0.012f,0.025f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            float time=float(glfwGetTime());
            glm::vec3 eye(7,5,10);
            auto view=glm::lookAt(eye,glm::vec3(0,1,-1),glm::vec3(0,1,0));
            auto projection=glm::perspective(glm::radians(48.f),float(width)/height,0.1f,100.f);
            auto draw=[&](Shader& s,glm::vec3 p,glm::vec3 scale,bool quad=false){
                s.use();
                auto model=glm::scale(glm::translate(glm::mat4(1),p),scale);
                s.setMat4("model",glm::value_ptr(model));s.setMat4("view",glm::value_ptr(view));s.setMat4("projection",glm::value_ptr(projection));
                glDrawArrays(GL_TRIANGLES,quad?30:0,quad?6:36);
            };
            lighting.apply(lit,time);lit.setVec3("eye",eye.x,eye.y,eye.z);
            for(Shader* shader : {&lit,&led}) {
                shader->use();
                shader->setVec3("eye",eye.x,eye.y,eye.z);
                shader->setInt("fogEnabled",fog);
            }
            floor.apply(lit);
            draw(lit,{0,-0.1f,-1},{10,0.2f,8});
            wall.apply(lit);
            draw(lit,{0,2,-5},{10,4,0.15f});
            draw(lit,{-5,2,-1},{0.15f,4,8});
            for(int rack=0;rack<4;rack++){
                float x=-3.f+rack*2.f;
                metal.apply(lit);
                draw(lit,{x,1.5f,-2},{1.4f,3,1.5f});
                for(int row=0;row<8;row++){
                    panel.apply(lit);
                    draw(lit,{x,0.25f+row*0.35f,-1.22f},{1.25f,0.28f,0.05f});
                    for(int k=0;k<3;k++){
                        ServerEffects::apply(led,lighting,time,rack*24+row*3+k);
                        draw(led,{x-0.45f+k*0.14f,0.25f+row*0.35f,-1.18f},{0.065f,0.065f,0.01f},true);
                    }
                }
            }
            if(glGetError()!=GL_NO_ERROR)throw runtime_error("OpenGL rendering error");
            glfwSwapBuffers(w);
            const double now=glfwGetTime();
            if(stats.addFrame(now-previous)) {
                ostringstream title;
                title<<"Server Room | FPS "<<fixed<<setprecision(1)<<stats.fps()
                     <<" | "<<setprecision(2)<<stats.frameMs()<<" ms"
                     <<" | A: alarm | L: ceiling | F: fog | Esc: exit";
                glfwSetWindowTitle(w,title.str().c_str());
            }
            previous=now;
            ++frames;
            if(smoke && now-started>=1.2) {
                if(stats.fps()<=0)throw runtime_error("FPS counter did not update");
                break;
            }
            if(smoke && frames<=4){fog=!fog;lighting.toggleAlarm();lighting.toggleCeiling();}
        }
        glDeleteBuffers(1,&vbo);glDeleteVertexArrays(1,&vao);
    }catch(const exception& e){cerr<<e.what()<<'\n';result=1;}
    glfwDestroyWindow(w);glfwTerminate();return result;
}
