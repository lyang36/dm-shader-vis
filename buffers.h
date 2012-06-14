#ifndef __LY_BUFFERS__
#define __LY_BUFFERS__
#include <string>
#include <glew.h>
#include <GLUT/glut.h>

using namespace std;

class buffer{              //buffers
protected:
    GLuint textureId;       //texture binding
    bool fboUsed;          //fbo ok?
    GLuint fboId, rboId;   //framebuffer and renderbuffer ID
    unsigned int twidth, theight; //the width and height of the buffer
    void checkbuffer();     //check whether buffer is ok
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
    
public:
    fluxBuffer(unsigned int w, unsigned int h):buffer(w,h){
    };//:buffer(w,h);
    
};

class minmaxBuffer:public buffer{     //min max buffer, for calculating min and max
public:
    minmaxBuffer(unsigned int w, unsigned int h):buffer(w,h){
        
    };
    
};                      

class colorBuffer:public buffer{      //buffer for final output
public:
    colorBuffer(unsigned int w, unsigned int h):buffer(w,h){
        
    };
};

#endif
