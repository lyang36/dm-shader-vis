#ifndef __LY_PARTICLESHADER__
#define __LY_PARTICLESHADER__
#include "types.h"
#include "shaderManager.h"
#include <string>

using namespace std;

class Shader{
protected:
    shaderObj * shader;
    string vfile, ffile;
    bool good;
    
public:
    bool is_good(){
        return good;
    };
    
    void begin(){
        shader->begin();
    };           //start shader
    
    void end(){
        shader->end();
    };//end shader
    
    void setfile(string v, string f){
        vfile =v;
        ffile =f;
    };
    
    
};


class fluxShader:public Shader{
private:
    REAL vpos[3];
    REAL opos[3];
    GLint xaxisloc, yaxisloc, zaxisloc;
    GLint oposloc;
    GLint geofacloc;
    
    void loadUniform();
public:
    void setzaxis3f(REAL x, REAL y, REAL z); //set vpos
    void setzaxis3fv(REAL * vpos);
    void setyaxis3f(REAL x, REAL y, REAL z); //set vpos
    void setyaxis3fv(REAL * vpos);
    void setxaxis3f(REAL x, REAL y, REAL z); //set vpos
    void setxaxis3fv(REAL * vpos);
    void setopos3f(REAL x, REAL y, REAL z); //set opos
    void setopos3fv(REAL * opos);
    void setgeofac3f(REAL x, REAL y, REAL z); //set opos
    void setgeofac3fv(REAL * geo);
    
    fluxShader(){
        vfile = "./flux.vert";
        ffile = "./flux.frag";
        shaderManager SM;
        shader = SM.loadShaderFile(vfile.c_str(), ffile.c_str());
        if(shader == 0){
            printf("Shader error!\n");
            exit(0);
        }
        if(shader->progmObj == 0){
            printf("Shader error!\n");
            exit(0);
            //good = false;
        }else{
            good = true;
        }
        if(good){
            loadUniform();
        }

    };  
};

/*
class minMaxShader{
private:
    string vfile, ffile;
    shaderObj * shader;
    //shaderManager SM;
public:
    minMaxShader();
    void begin();
    void end();
};
*/

class colorShader:public Shader{
public:
    colorShader(){
        vfile = "./color.vert";
        ffile = "./color.frag";
        shaderManager SM;
        shader = SM.loadShaderFile(vfile.c_str(), ffile.c_str());
        if(shader == 0){
            printf("Shader error!\n");
            exit(0);
        }
        if(shader->progmObj == 0){
            printf("Shader error!\n");
            exit(0);
            //good = false;
        }else{
            good = true;
        }
    };
};
#endif
