#include "shaders.h"

using namespace std;
void fluxShader::loadUniform(){

    zaxisloc = glGetUniformLocation(shader->progmObj,"zaxis");
    //printf("zloc, %d\n", zaxisloc);
    xaxisloc = glGetUniformLocation(shader->progmObj,"xaxis");
    //printf("xloc, %d\n", xaxisloc);

    yaxisloc = glGetUniformLocation(shader->progmObj,"yaxis");
    //printf("yloc, %d\n", yaxisloc);

    oposloc = glGetUniformLocation(shader->progmObj,"opos");
    //printf("oloc, %d\n", oposloc);

    geofacloc = glGetUniformLocation(shader->progmObj,"geofac");
    //printf("gloc, %d\n", geofacloc);


    
}


//set values when use the program
void fluxShader::setzaxis3fv(REAL * za){
    //printf("%d, zaxis %f %f %f\n",shader->progmObj, za[0], za[1], za[2]);
    glUniform3f(zaxisloc, za[0], za[1], za[2]);
   
}

void fluxShader::setzaxis3f(REAL x, REAL y, REAL z){
    //printf("%d, zaxis %f %f %f\n",shader->progmObj, x, y, z);

    glUniform3f(zaxisloc, x, y, z);

}

void fluxShader::setxaxis3fv(REAL * xa){
    glUniform3f(xaxisloc, xa[0], xa[1], xa[2]);
}

void fluxShader::setxaxis3f(REAL x, REAL y, REAL z){
    glUniform3f(xaxisloc, x, y, z);
}

void fluxShader::setyaxis3fv(REAL * ya){
    glUniform3f( yaxisloc, ya[0], ya[1], ya[2]);
}

void fluxShader::setyaxis3f(REAL x, REAL y, REAL z){
    glUniform3f(yaxisloc, x, y, z);
}

void fluxShader::setopos3fv(REAL * ya){
    glUniform3f(oposloc, ya[0], ya[1], ya[2]);
}

void fluxShader::setopos3f(REAL x, REAL y, REAL z){
    glUniform3f(oposloc, x, y, z);
}

void fluxShader::setgeofac3fv(REAL * ya){
    glUniform3f(geofacloc, ya[0], ya[1], ya[2]);
}

void fluxShader::setgeofac3f(REAL x, REAL y, REAL z){
    glUniform3f(geofacloc, x, y, z);
}



void minMaxShader::setsppos(REAL x, REAL y){

    GLint loc = glGetUniformLocation(shader->progmObj,"sppos");
    if(loc == -1){
        printf("no sppos! Are you using the correct shader?\n" );
    }else{
        glUniform2f(loc, x, y);
    }
    
}

void minMaxShader::setparams(REAL mapsize, REAL blocksize, REAL scale){

    GLint loc = glGetUniformLocation(shader->progmObj,"params");
    if(loc == -1){
        printf("no params! Are you using the correct shader?\n" );
    }else{
        glUniform3f(loc, mapsize, blocksize, scale);
    }
}

