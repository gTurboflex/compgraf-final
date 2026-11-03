#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Lighting.h"
#include "Texture.h"
#include "ServerEffects.h"
#include "Camera.h"
#include "Input.h"
#include "UI.h"
#include "PerformanceStats.h"
#include "RackDoors.h"
#include "../tests/input_smoke.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;
struct Options {
    bool smoke = false, benchmark = false;
    std::string mode = "normal", output, capture;
    double seconds = 30;
};
Options options(int argc, char** argv) {
    Options out;
    for (int i=1; i<argc; ++i) {
        std::string arg = argv[i];
        if (arg=="--smoke-test") out.smoke=true;
        else if (arg=="--benchmark" && i+1<argc) { out.benchmark=true; out.mode=argv[++i]; }
        else if (arg=="--seconds" && i+1<argc) out.seconds=std::stod(argv[++i]);
        else if (arg=="--output" && i+1<argc) out.output=argv[++i];
        else if (arg=="--capture" && i+1<argc) out.capture=argv[++i];
        else throw std::runtime_error("Usage: OpenGLApp [--smoke-test] [--benchmark normal|alarm|ceiling-off] [--seconds 30] [--output result.json] [--capture frame.ppm]");
    }
    if (out.mode!="normal" && out.mode!="alarm" && out.mode!="ceiling-off") throw std::runtime_error("Unknown benchmark mode");
    if (!(out.seconds>0 && out.seconds<=3600)) throw std::runtime_error("Benchmark seconds must be in (0,3600]");
    return out;
}
void captureFrame(const std::string& path, int width, int height) {
    std::vector<unsigned char> pixels(static_cast<size_t>(width)*height*3);
    GLint alignment; glGetIntegerv(GL_PACK_ALIGNMENT,&alignment);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    glPixelStorei(GL_PACK_ALIGNMENT,alignment);
    std::ofstream file(path,std::ios::binary);
    if (!file) throw std::runtime_error("Cannot write frame capture");
    file<<"P6\n"<<width<<' '<<height<<"\n255\n";
    for(int y=height-1; y>=0; --y)
        file.write(reinterpret_cast<const char*>(pixels.data()+static_cast<size_t>(y)*width*3),width*3);
}

int main(int argc, char** argv) {
    Options config;
    try { config=options(argc,argv); } catch(const std::exception& e) { std::cerr<<e.what()<<'\n';return 1; }
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
    GLFWwindow* w=glfwCreateWindow(1280,720,"Server Room | Member 4 controls",nullptr,nullptr);
    if (!w) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(w);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {glfwDestroyWindow(w);glfwTerminate();return 1;}
    glfwSwapInterval(config.benchmark || config.smoke ? 0 : 1);
    int result=0;
    try {
        const fs::path root=fs::absolute(argv[0]).parent_path();
        auto shaderPath=[&](const char* name){return (root/"shaders"/name).string();};
        auto texturePath=[&](const char* name){return (root/"assets/textures"/name).string();};
        Shader lit(shaderPath("lit.vert").c_str(),shaderPath("lit.frag").c_str());
        Shader led(shaderPath("led.vert").c_str(),shaderPath("led.frag").c_str());
        UI ui(shaderPath("ui.vert").c_str(),shaderPath("ui.frag").c_str());
        Camera camera;
        Input input(w,camera);
        if(config.smoke)runInputChecks(w,input,camera);
        Lighting lighting;
        RackDoors doors;
        PerformanceStats stats;
        if (config.benchmark) stats.beginBenchmark(2,config.seconds);
        if (config.mode=="alarm") lighting.toggleAlarm();
        if (config.mode=="ceiling-off") lighting.toggleCeiling();
        Texture floorTex(texturePath("floor.pgm").c_str()),metalTex(texturePath("metal.pgm").c_str());
        Texture wallTex(texturePath("wall.pgm").c_str()),panelTex(texturePath("panel.pgm").c_str());
        Material floor{floorTex,.50f,.56f,.61f,1.f,28.f,.18f};
        Material wall{wallTex,.62f,.68f,.70f,.6f,12.f,.08f};
        Material metal{metalTex,.25f,.32f,.38f,1.f,80.f,.52f};
        Material panel{panelTex,.42f,.48f,.53f,2.f,44.f,.32f};
        std::vector<float> vertices;
        for(int axis=0;axis<3;axis++) for(int sign: {-1,1}) {
            const int indices[]={0,1,2,0,2,3};
            const float corners[][2]={{-.5f,-.5f},{.5f,-.5f},{.5f,.5f},{-.5f,.5f}};
            for(int i:indices) {
                glm::vec3 p(0),n(0);p[axis]=sign*.5f;n[axis]=float(sign);
                p[(axis+1)%3]=corners[i][0];p[(axis+2)%3]=corners[i][1];
                for(int j=0;j<3;j++)vertices.push_back(p[j]);
                for(int j=0;j<3;j++)vertices.push_back(n[j]);
            }
        }
        GLuint vao,vbo;
        glGenVertexArrays(1,&vao);glGenBuffers(1,&vbo);glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float),vertices.data(),GL_STATIC_DRAW);
        for(int i=0;i<2;i++){glEnableVertexAttribArray(i);glVertexAttribPointer(i,3,GL_FLOAT,GL_FALSE,6*sizeof(float),reinterpret_cast<void*>(i*3*sizeof(float)));}
        glEnable(GL_DEPTH_TEST);
        bool fog=true,help=true,capturedFrame=false;
        int frames=0,lastWidth=1280,lastHeight=720;
        double previous=glfwGetTime(),started=previous;
        while(!glfwWindowShouldClose(w)) {
            glfwPollEvents();
            int width,height;glfwGetFramebufferSize(w,&width,&height);
            if(width<=0 || height<=0){glfwWaitEventsTimeout(.1);previous=glfwGetTime();continue;}
            double now=glfwGetTime(),dt=now-previous;previous=now;
            auto actions=input.update(static_cast<float>(dt));
            if(actions.alarm)lighting.toggleAlarm();
            if(actions.ceiling)lighting.toggleCeiling();
            if(actions.fog)fog=!fog;
            if(actions.toggleHelp)help=!help;
            if(actions.door)doors.toggle(doors.nearby(camera.position()));
            doors.update(static_cast<float>(dt));
            stats.addFrame(dt);
            glViewport(0,0,width,height);lastWidth=width;lastHeight=height;
            glClearColor(.008f,.012f,.025f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            const float time=static_cast<float>(now);
            const auto eye=camera.position();
            const auto view=camera.view(),projection=camera.projection(width,height);
            glBindVertexArray(vao);
            auto drawModel=[&](Shader& s,const glm::mat4& model,bool quad=false){
                s.use();s.setMat4("model",glm::value_ptr(model));s.setMat4("view",glm::value_ptr(view));s.setMat4("projection",glm::value_ptr(projection));
                glDrawArrays(GL_TRIANGLES,quad?30:0,quad?6:36);
            };
            auto draw=[&](Shader& s,glm::vec3 p,glm::vec3 scale,bool quad=false){
                drawModel(s,glm::scale(glm::translate(glm::mat4(1),p),scale),quad);
            };
            lighting.apply(lit,time);
            for(Shader* s:{&lit,&led}){s->use();s->setVec3("eye",eye.x,eye.y,eye.z);s->setInt("fogEnabled",fog);}
            floor.apply(lit);draw(lit,{0,-.1f,-1},{10,.2f,8});
            wall.apply(lit);draw(lit,{0,2,-5},{10,4,.15f});draw(lit,{-5,2,-1},{.15f,4,8});
            for(int rack=0;rack<RackDoors::count;rack++){
                float x=RackDoors::rackX(rack);
                metal.apply(lit);draw(lit,{x,1.5f,-2},{1.4f,3,1.5f});
                for(int row=0;row<8;row++){
                    panel.apply(lit);draw(lit,{x,.25f+row*.35f,-1.22f},{1.25f,.28f,.05f});
                    for(int k=0;k<3;k++){
                        ServerEffects::apply(led,lighting,time,rack*24+row*3+k);
                        draw(led,{x-.45f+k*.14f,.25f+row*.35f,-1.18f},{.065f,.065f,.01f},true);
                    }
                }
                metal.apply(lit);
                auto door=doors.transform(rack);
                auto rail=[&](glm::vec3 p,glm::vec3 size){drawModel(lit,glm::scale(glm::translate(door,p),size));};
                rail({.02f,0,0},{.04f,2.9f,.07f});rail({1.38f,0,0},{.04f,2.9f,.07f});
                rail({.7f,-1.43f,0},{1.4f,.04f,.07f});rail({.7f,1.43f,0},{1.4f,.04f,.07f});
                for(float xBar:{.35f,.7f,1.05f})rail({xBar,0,0},{.018f,2.86f,.04f});
                rail({1.25f,0,.06f},{.035f,.3f,.05f});
            }
            int nearby=doors.nearby(eye);
            UIState hud;
            hud.fps=stats.fps();hud.frameMs=stats.frameMs();hud.helpVisible=help;hud.mouseCaptured=input.captured();
            hud.alarm=lighting.alarm;hud.ceiling=lighting.ceilingEnabled;hud.fog=fog;
            hud.doorPrompt=nearby<0?"E: WALK CLOSER TO A RACK":"E: "+std::string(doors.isOpen(nearby)?"CLOSE":"OPEN")+" RACK "+std::to_string(nearby+1)+" DOOR";
            ui.draw(width,height,hud);
            if(!config.capture.empty() && !capturedFrame && (config.smoke?frames==2:now-started>=2.5)) {
                captureFrame(config.capture,width,height);capturedFrame=true;
            }
            if((config.smoke || config.benchmark) && glGetError()!=GL_NO_ERROR)throw std::runtime_error("OpenGL rendering error");
            glfwSwapBuffers(w);++frames;
            if(config.smoke){
                if(frames>=8)break;
                fog=!fog;lighting.toggleAlarm();lighting.toggleCeiling();
                doors.toggle(frames%RackDoors::count);camera.move(1,0,.05f);
            }
            if(config.benchmark && stats.benchmarkDone())break;
        }
        if(config.benchmark){
            auto b=stats.benchmark();
            std::ofstream file;
            if(!config.output.empty()){file.open(config.output);if(!file)throw std::runtime_error("Cannot write benchmark results");}
            std::ostream& out=config.output.empty()?std::cout:file;
            out<<std::fixed<<std::setprecision(3)<<"{\n  \"mode\": "<<std::quoted(config.mode)<<",\n  \"width\": "<<lastWidth<<", \"height\": "<<lastHeight
               <<",\n  \"renderer\": "<<std::quoted(reinterpret_cast<const char*>(glGetString(GL_RENDERER)))
               <<",\n  \"opengl\": "<<std::quoted(reinterpret_cast<const char*>(glGetString(GL_VERSION)))
               <<",\n  \"vsync\": false, \"warmup_seconds\": 2,\n  \"frames\": "<<b.frames<<", \"seconds\": "<<b.seconds
               <<",\n  \"mean_fps\": "<<b.meanFps<<", \"p95_frame_ms\": "<<b.p95FrameMs<<",\n  \"max_frame_ms\": "<<b.maxFrameMs
               <<",\n  \"complete\": "<<(stats.benchmarkDone()?"true":"false")<<"\n}\n";
        }
        glDeleteBuffers(1,&vbo);glDeleteVertexArrays(1,&vao);
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';result=1;}
    glfwDestroyWindow(w);glfwTerminate();return result;
}
