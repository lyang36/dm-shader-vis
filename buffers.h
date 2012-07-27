#ifndef __LY_BUFFERS__
#define __LY_BUFFERS__
#include <string>
//#include <glew.h>
//#include <GLUT/glut.h>
#ifdef __APPLE__
#include <glew.h>
#include <GLUT/glut.h> // darwin uses glut.h rather than GL/glut.h
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include "shaders.h"

using namespace std;

class buffer{              //buffers
protected:
    GLuint textureId;       //texture binding
    bool fboUsed;          //fbo ok?
    GLuint fboId, rboId;   //framebuffer and renderbuffer ID
    unsigned int twidth, theight; //the width and height of the buffer
    void checkbuffer();     //check whether buffer is ok
    bool  issetup;
    
public:     
    buffer(unsigned int w, unsigned int h);
    ~buffer();
    GLuint getTex();     //get texture ID
    void bindTex();          //bind the texture
    void unbindTex();        //unbind texture
    void setTex(GLuint tex);       //set a texture, bind it to the buffer
    void genTex();           //generate a new texture
    void attachTex();        //attach the texture
    void genBuffer();        //generate the frame buffer and render buffer
                             //and attach them together and bind buffer
    void setBuffer();        //automatically set up all the things
    
    void bindBuf();          //bind buffer to render
    void unbindBuf();        //unbind buffer from render
    
    void readTex(void * tex);//read the data from the GPU
    void saveImg(string filename);  //save the picture to file
};

class fluxBuffer:public buffer{       //buffer for additing flux
private:
    GLuint normtex;          //map used to calculate map flux
    int normMapRes;          //map resolution, in pixels.
    int normPointSize;       //max point size of the point sprite, in pixels
    void loadnorm();         //calculate normalization text or from file
    string normfile;// = "norm.dat";
    float * normtextbuf;
public:
    //load norm and bind texture
    void setNormTex(){
        normtextbuf = new float[normMapRes * normMapRes];
        loadnorm();
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &normtex);
        glBindTexture(GL_TEXTURE_2D, normtex);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, normMapRes, normMapRes, 0, GL_RED, GL_FLOAT, normtextbuf);
        /*for(int i = 0; i < normMapRes*normMapRes; i++){
            printf("%f  ", normtextbuf[i]);
        }*/
        // set its parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    };
    
    void setMapRes(int m, int n){
        normMapRes = m;
        normPointSize = n;
    };
    
    fluxBuffer(unsigned int w, unsigned int h):buffer(w,h){
        normtex = 100;
        normMapRes = 1024;
        normPointSize = 256;
        normfile = "norm.dat";
        normtextbuf = NULL;
    };//:buffer(w,h);
    
    ~fluxBuffer(){
        if(normtextbuf != NULL){
            delete normtextbuf;
            normtextbuf = NULL;
        }
    }
    
};

class minmaxBuffer:public buffer{     //min max buffer, for calculating min and max
private:
    GLuint textureId2;       //second texture map used for finding the min max
    GLuint textureInput;     //input texture
    unsigned int mapsize;
    unsigned int scale;
    bool  texgenned; 
    void gentex2();
    void draw();
    
public:
    minMaxShader * mshader;
    minmaxBuffer(unsigned int w, unsigned int h):buffer(w,h){
        mapsize = 512;
        scale = 2;
        texgenned = false;
        issetup = false;
    };
    
    void setsize(int msize, int ss){
        mapsize = msize;
        scale = ss;
    };
    
    void setInput(GLuint tex){
        textureInput = tex;
    };
    //find the min and max inside the disk of radius 1
    //use scale to do search for min and max
    //for example, there is 512*512 pixels, use scale of 4
    //the first step search for min and max in 4*4 pixels
    //and then search in 16*16 pixels and then 64*64 ...
    void findMinMax();  //do find the minmax. And finally store the map to be in textureId for bindTex to read
    ~minmaxBuffer(){
        glDeleteFramebuffers(1, &fboId);
        glDeleteRenderbuffers(1, &rboId);
        glDeleteTextures(1, &textureId);
        glDeleteTextures(1, &textureId2);
    };
};                      

class colorBuffer:public buffer{      //buffer for final output
public:
    colorBuffer(unsigned int w, unsigned int h):buffer(w,h){
        
    };
};

#endif
