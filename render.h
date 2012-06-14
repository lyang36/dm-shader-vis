#ifndef __LY__RENDER__
#define __LY__RENDER__
#include "types.h"
#include "buffers.h"
#include "shaders.h"
#include "datareader.h"
#include "Parameter.h"

class render{
private:
    fluxShader * fshader;
    //minMaxShader * mshader;
    colorShader * cshader;
    fluxBuffer * fbuffer;
    colorBuffer * cbuffer;
    
    GLuint textureIni;  //initial empty texture

    unsigned int windowSize;       //2^m
    unsigned int pointSize;
    REAL orthsize;                //orthogonal view port size                  
    
    void init();
    void drawFlux();
    void drawImage();

    bool initialed;     //is initialed?
public:
    DataReader * reader;
    Parameter * params;
    render(){
            windowSize = 512;
            pointSize = 256;
            initialed = false;
            orthsize = 10.0;
/*            fshader = new fluxShader();
            cshader = new colorShader();
            fbuffer = new fluxBuffer(windowSize, windowSize);
            cbuffer = new colorBuffer(windowSize, windowSize);
                FB->setBuffer();
    CB->setBuffer();
*/
    };
    void start(int argc, char **argv);
};

#endif
