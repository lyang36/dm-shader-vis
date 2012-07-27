#ifndef __LY_PARTICLESHADER__
#define __LY_PARTICLESHADER__
#include <string>
#include <cstdio>
#include <stdlib.h>
#ifdef _WIN32 || _WIN64
#include <windows.h>
#endif
#include "types.h"
#include "shaderManager.h"

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
    //GLint xaxisloc, yaxisloc, zaxisloc;
    GLint rotmloc;
    GLint oposloc;
    GLint geofacloc;
    void setrotm(bool updown);  //setup rotation matrix, 9 variables
                                //updown: true/up; false/down
    void loadUniform();
    REAL * align_vec;
    REAL * opos;
    REAL * cpos;
    REAL rotmatrix[9];
public:
    //void setzaxis3f(REAL x, REAL y, REAL z); //set vpos
    //void setzaxis3fv(REAL * vpos);
    //void setyaxis3f(REAL x, REAL y, REAL z); //set vpos
    //void setyaxis3fv(REAL * vpos);
    //void setxaxis3f(REAL x, REAL y, REAL z); //set vpos
    //void setxaxis3fv(REAL * vpos);
    void setopos3f(REAL x, REAL y, REAL z); //set opos
    void setopos3fv(REAL * opos);
    void setgeofac3f(REAL x, REAL y, REAL z); //set opos
    void setgeofac3fv(REAL * geo);
    void setrotmatrix(REAL * alignvec, REAL * obsvec, REAL * centervec, bool updown);     //setup rotation matrix
                                            //alignvec has 3 vars
    
    fluxShader(){
        
#ifdef _WIN32 || _WIN64
        vfile = "shaderfiles\\flux.vert";
        ffile = "shaderfiles\\flux.frag";
#else
        vfile = "./shaderfiles/flux.vert";
        ffile = "./shaderfiles/flux.frag";
#endif
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


class minMaxShader:public Shader{
private:
    //use pixstep to do search in pixscale * pixscale region
    void searchstep(int pixstep, int pixscale);  
    string fillfile;
    shaderObj * fillshader;
    
public:
    minMaxShader(){
#ifdef _WIN32 || _WIN64
        vfile = "shaderfiles\\coor.vert";
        ffile = "shaderfiles\\minmax.frag";
        fillfile = "shaderfiles\\fill.frag";
#else
        vfile = "./shaderfiles/coor.vert";
        ffile = "./shaderfiles/minmax.frag";
        fillfile = "./shaderfiles/fill.frag";
#endif
        shaderManager SM, SM1;
        shader = SM.loadShaderFile(vfile.c_str(), ffile.c_str());
        fillshader = SM1.loadShaderFile(vfile.c_str(), fillfile.c_str());
        if(shader == 0 || fillshader == 0){
            printf("Shader error!\n");
            exit(0);
        }
        if(shader->progmObj == 0 || fillshader->progmObj == 0){
            printf("Shader error!\n");
            exit(0);
            //good = false;
        }else{
            good = true;
        }
    };
    void setsppos(REAL x, REAL y);  //setup the sppos in the fill shader
    void setparams(REAL mapsize, REAL blocksize, REAL scales);  //setup params in the minmax shader
    void beginfill(){
        fillshader->begin();
    };       //begin the fill shader
    void beginsearching(){
        shader->begin();
    };  //begin the min max searching shader
    void end(){
        shader->end();
        fillshader->end();
    };
 
};


class colorShader:public Shader{
public:
    colorShader(){
#ifdef _WIN32 || _WIN64
        vfile = "shaderfiles\\color.vert";
        ffile = "shaderfiles\\color.frag";
#else
        vfile = "./shaderfiles/color.vert";
        ffile = "./shaderfiles/color.frag";
#endif
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
    
    /*void setmultitex(){
        int loc1 = glGetUniformLocationARB(shader->progmObj, "tex1");
        int loc2 = glGetUniformLocationARB(shader->progmObj, "tex2");
        if(loc1 == -1 || loc2 == -1){
            printf("cannot find tex1 or tex2\n");
        }
        glUniform1iARB(loc1, 0);
        glUniform1iARB(loc2, 1);
    };*/
    
    void setminmax(float fmin, float fmax){
        int lmax = glGetUniformLocationARB(shader->progmObj, "FMAX");
        int lmin = glGetUniformLocationARB(shader->progmObj, "FMIN");
        if(lmax == -1 || lmin == -1){
            printf("cannot find MAX or MIN\n");
        }
        ///printf("mm: %f, %f \n", fmin, fmax);
        glUniform1fARB(lmax, fmax);
        glUniform1fARB(lmin, fmin);
    }
};
#endif
