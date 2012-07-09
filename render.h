#ifndef __LY__RENDER__
#define __LY__RENDER__
#include "types.h"
#include "buffers.h"
#include "shaders.h"
#include "datareader.h"
#include "Parameter.h"

class render{
private:
    //L for the lower sphere
    //U for the upper sphere
    fluxShader * fshaderL;
    colorShader * cshaderL;
    fluxBuffer * fbufferL;
    colorBuffer * cbufferL;
    
    fluxShader * fshaderU;
    colorShader * cshaderU;
    fluxBuffer * fbufferU;
    colorBuffer * cbufferU;
    
    
    GLuint textureIni;  //initial empty texture

    unsigned int windowSize;       //2^m
    unsigned int pointSize;
    REAL orthsize;                //orthogonal view port size                  
    
    void init();
    void drawFlux();
    void drawImage();

    bool initialed;     //is initialed?
    void findMinMax(float &fluxmax, float &fluxmin);
    
    REAL * fluxmapL, *fluxmapU;  //L and U map
    
    void readFluxMap();         //read map from the GPU
    
    void saveHealPix();         //save healpix map to the file
    void saveFluxMap();         //save flux map to the file
    double _getpixflux(int x1, int y1, bool isupshere);
    
public:
    DataReader * reader;
    Parameter * params;
    render(){
            windowSize = 512;
            pointSize = 256;
            initialed = false;
            orthsize = 1.0;
    };
    void start(int argc, char **argv);
};

#endif
