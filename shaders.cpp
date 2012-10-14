#include <cmath>
#include "types.h"
#include "shaders.h"

using namespace std;
void fluxShader::loadUniform(){

    rotmloc = glGetUniformLocation(shader->progmObj,"rotmatrix");
    
    oposloc = glGetUniformLocation(shader->progmObj,"opos");
    //printf("oloc, %d\n", oposloc);

    geofacloc = glGetUniformLocation(shader->progmObj,"geofac");
    //printf("gloc, %d\n", geofacloc);

    isnormmaploc = glGetUniformLocation(shader->progmObj,"usenormmap");

    lim_angleloc = glGetUniformLocation(shader->progmObj, "lim_angle");
    
}



void fluxShader::setrotm(){

    double x = align_vec[0];
    double y = align_vec[1];
    double z = align_vec[2];
    double r = sqrt(x*x+y*y+z*z);
    double z1 = x / r;
    double z2 = y / r;
    double z3 = z / r;
    //printf("z %f %f %f\n", z1, z2, z3);
    
    //Rotate or not?
    for(int i = 0; i < 9 ; i ++){
        rotmatrix[i] = 0;
    }
    
    r = sqrt(z1 * z1 + z2 * z2);
    if(r == 0.0){
        if(z3 < 0){
            rotmatrix[0] = 1;
            rotmatrix[4] = 1;
            rotmatrix[8] = 1;
        }else{
            rotmatrix[0] = -1;
            rotmatrix[4] = -1;
            rotmatrix[8] = -1;
        }
        
    }else{
    
    
        rotmatrix[0] = -z2 / r;
        rotmatrix[3] = +z1 / r;
        rotmatrix[6] = 0;


        r = sqrt(z1 * z3 * z1 * z3 + z2 * z3 * z2 * z3 + (z1*z1 + z2*z2) * (z1*z1 + z2 * z2));
        rotmatrix[1] = -z1 * z3 / r;
        rotmatrix[4] = -z2 * z3 / r;
        rotmatrix[7] = (z1 * z1 + z2 * z2) / r;

        rotmatrix[2] = -z1;
        rotmatrix[5] = -z2;
        rotmatrix[8] = -z3;
    }
    
    printf("ROTMATRIX:\n");
    printf("%f %f %f\n", rotmatrix[0], rotmatrix[1], rotmatrix[2]);
    printf("%f %f %f\n", rotmatrix[3], rotmatrix[4], rotmatrix[5]);
    printf("%f %f %f\n", rotmatrix[6], rotmatrix[7], rotmatrix[8]);

    return; 
}


void fluxShader::setrotmatrix(REAL * alignvec, REAL * obsvec, REAL * centvec){
    this->align_vec = alignvec;
    this->opos = obsvec;
    this->cpos = centvec;
    setrotm();
    glUniformMatrix3fv(rotmloc, 1, GL_FALSE, this->rotmatrix);
    
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

void fluxShader::setusenormmap(bool isnm){
    if(isnm){
        glUniform1i(isnormmaploc, 1);
    }else{
        glUniform1i(isnormmaploc, 0);
    }
}


void fluxShader::setlimitangle(float angle){
    glUniform1f(lim_angleloc, PI - angle);
}


void minMaxShader::setsppos(REAL x, REAL y){

    GLint loc = glGetUniformLocation(fillshader->progmObj,"sppos");
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

