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
    //bool isUseMap;
public:
    //load norm and bind texture
    void setNormTex(){
        normtextbuf = new float[normMapRes * normMapRes];
        loadnorm();
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &normtex);
        glBindTexture(GL_TEXTURE_2D, normtex);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, normMapRes, normMapRes, 0, GL_RED, GL_FLOAT, normtextbuf);
        //for(int i = 0; i < normMapRes*normMapRes; i++){
        //    printf("%f  ", normtextbuf[i]);
        //}
        // set its parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        
        // setup some generic opengl options
        glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
        glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
        glClampColorARB(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);
        
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
      
    unsigned int getwidth(){
        return twidth;
    };
    
    unsigned int getheigth(){
        return theight;
    };
    
    
    ~fluxBuffer(){
        if(normtextbuf != NULL){
            delete normtextbuf;
            normtextbuf = NULL;
        }
    };
    
};


class fluxDoubleBuffer:public fluxBuffer{
private:
	GLuint tDepthCubeMap;
	GLuint tColorCubeMap;
public:
	fluxDoubleBuffer(unsigned int w, unsigned int h):fluxBuffer(w,h){
    };
    
	
    void genBuffer(){
        
        // create a framebuffer object
        //GLuint fboId;
        glGenFramebuffers(1, &fboId);
        //glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        bindBuf();
        printf("Buffer generated!\n");
        //printf("frame: %d, render: %d \n", fboId, rboId);
    }
    
	void configure(){ //set a cube texture, bind it to the buffer
        
        
        glEnable(GL_TEXTURE_CUBE_MAP);
        // depth cube map
        glGenTextures(1, &tDepthCubeMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tDepthCubeMap);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
         
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        for (int face = 0; face < 6; face++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT32,
                         twidth, theight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        
        // color cube map
        glGenTextures(1, &tColorCubeMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tColorCubeMap);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        for (int face = 0; face < 6; face++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA32F_ARB,
                         twidth, theight, 0, GL_RGBA, GL_FLOAT, NULL);
        }
        
        // framebuffer object
        glGenFramebuffers(1, &fboId);
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tDepthCubeMap, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tColorCubeMap, 0);
        
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        
        checkbuffer();
        unbindBuf();
    };
                            
                            //copy the texture back to the original one
	void copytex(GLuint tex0, GLuint tex1){        

        glGenRenderbuffers(1, &rboId);
        glBindRenderbuffer(GL_RENDERBUFFER, rboId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, twidth, theight);
        bindBuf();
        //Attach depth buffer to FBO
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0, tColorCubeMap, 0);
        checkbuffer();
        glBindTexture(GL_TEXTURE_2D, tex0);
        //glReadBuffer(GL_COLOR_ATTACHMENT0);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, 0, 0, twidth, theight, 0);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1, tColorCubeMap, 0);
        
        checkbuffer();
        glBindTexture(GL_TEXTURE_2D, tex1);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, 0, 0, twidth, theight, 0);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
	};
    

    ~fluxDoubleBuffer(){
        glDeleteTextures(1, &tColorCubeMap);
        glDeleteTextures(1, &tDepthCubeMap);
        glDisable(GL_TEXTURE_CUBE_MAP);
    };
};

/*

class fluxDoubleBuffer:public fluxBuffer{
private:
    GLuint bufferTex0;
    GLuint bufferTex1;
public:
    fluxDoubleBuffer(unsigned int w, unsigned int h):fluxBuffer(w,h){
    };
    void attachTex(GLuint tex0, GLuint tex1){
        bufferTex0 = tex0;
        bufferTex1 = tex1;
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
         GL_TEXTURE_2D, tex0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
         GL_TEXTURE_2D, tex1, 0);

        
    };
    void configure(GLuint tex0, GLuint tex1){
        genBuffer();
        attachTex(tex0, tex1);
        checkbuffer();
        unbindBuf();
    };
    
    
    void start(){
        bindBuf();
        //GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        //glDrawBuffers(2, buffers);
        
        GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, buffers);
    }
};
*/

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
