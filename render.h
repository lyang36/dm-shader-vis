#ifndef __LY__RENDER__
#define __LY__RENDER__
#include "types.h"
#include "buffers.h"
#include "shaders.h"
#include "datareader.h"
#include "Parameter.h"

class render{
private:
    //for the lower sphere
    fluxShader * fshader;
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
    void findMinMax(float &fluxmax, float &fluxmin);
    
    REAL * fluxmap;//, *fluxmapU;  //L and U map
    
    void readFluxMap();         //read map from the GPU
    
   	//void saveHealPix();         //save healpix map to the file
    void saveFluxMap();         //save flux map to the file
    //double _getpixflux(int x1, int y1, bool isupshere);
    double totalFlux;
    double rendertime;
    
public:
    DataReader * reader;
    Parameter * params;
    render(){
        fluxmap = NULL;
        
        windowSize = 512;
        pointSize = 256;
        initialed = false;
        orthsize = 1.0;
        totalFlux = 0.0;
        rendertime = 0.0;
    };
    void start(int argc, char **argv);

};

#endif
