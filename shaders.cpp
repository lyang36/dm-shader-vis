#include <cmath>
#include "types.h"
#include "shaders.h"

using namespace std;
void fluxShader::loadUniform(){

    //zaxisloc = glGetUniformLocation(shader->progmObj,"zaxis");
    //printf("zloc, %d\n", zaxisloc);
    //xaxisloc = glGetUniformLocation(shader->progmObj,"xaxis");
    //printf("xloc, %d\n", xaxisloc);

    //yaxisloc = glGetUniformLocation(shader->progmObj,"yaxis");
    //printf("yloc, %d\n", yaxisloc);

    rotmloc = glGetUniformLocation(shader->progmObj,"rotmatrix");
    
    oposloc = glGetUniformLocation(shader->progmObj,"opos");
    //printf("oloc, %d\n", oposloc);

    geofacloc = glGetUniformLocation(shader->progmObj,"geofac");
    //printf("gloc, %d\n", geofacloc);

    isnormmaploc = glGetUniformLocation(shader->progmObj,"usenormmap");

    
}




void fluxShader::setrotm(bool updown){
    REAL obsvec[] = {opos[0] - cpos[0], opos[1] - cpos[1], opos[2] - cpos[2]};
    REAL radius = sqrt(obsvec[0]*obsvec[0] + obsvec[1]*obsvec[1] + obsvec[2]*obsvec[2]);
    Real otheta = acos(obsvec[2]/radius);
    Real ophi = asin((obsvec[1])/sqrt(obsvec[1] * obsvec[1] + obsvec[0]*obsvec[0]));
    
    
    if (obsvec[0] >= 0 && obsvec[1] < 0)
        ophi = 2.0 * PI ;
    else if(obsvec[0] <0){
        ophi = PI - ophi;
    }
    
    //ophi += PI;
    Real rotaxis[3];
    
    printf("cpos %f %f %f\n", cpos[0], cpos[1], cpos[2]);
    printf("opos %f %f %f\n", opos[0], opos[1], opos[2]);
    printf("vvec %f %f %f\n", align_vec[0], align_vec[1], align_vec[2]);
    
    printf("otheta %f ophi %f\n", otheta, ophi);
    //printf("");
    
    rotaxis[0] = 0.0;
    rotaxis[1] = cos(otheta) / sqrt((sin(otheta) * sin(ophi)) * (sin(otheta) * sin(ophi)) + (cos(otheta) * cos(otheta)));
    rotaxis[2] = -sin(otheta) * sin(ophi) / sqrt((sin(otheta)*sin(ophi)) * (sin(otheta)*sin(ophi)) + (cos(otheta) * cos(otheta)));
    
    Real c = -sin(otheta) * cos(ophi);
    Real s = -sqrt((sin(otheta) * sin(ophi) * sin(otheta) * sin(ophi)) + 
                   (cos(otheta) * cos(otheta)));
    Real t = 1.0 - c;
    Real rotma[9];
    for(int i = 0; i < 9; i ++ ){
        rotma[i]=0;
    }
    
    rotma[0] = t * rotaxis[0] * rotaxis[0]+ c;
    rotma[1*3 + 0] = t * rotaxis[0] * rotaxis[1] - s * rotaxis[2];
    rotma[2*3 + 0] = t * rotaxis[0] * rotaxis[2] + s * rotaxis[1];
    
    rotma[1] = t * rotaxis[0] * rotaxis[1] + s * rotaxis[2];
    rotma[1*3 + 1] = t * pow(rotaxis[1], 2) + c;
    rotma[2*3 + 1] = t * rotaxis[1] * rotaxis[2] - s * rotaxis[0];
    
    rotma[0*3 + 2] = t * rotaxis[0] * rotaxis[2] - s * rotaxis[1];
    rotma[1*3 + 2] = t * rotaxis[1] * rotaxis[2] + s * rotaxis[0];
    rotma[2*3 + 2] = t * pow(rotaxis[2], 2) + c;
    
    //cout << "ROT[0][0] " << rotmatrix[0][0] << endl;
    
    Real distance = radius;
    Real tmpvec[] = {cpos[0] + distance * align_vec[0] - opos[0], 
        cpos[1] + distance * align_vec[1] - opos[1], 
        cpos[2] + distance * align_vec[2] - opos[2]};
    
    Real xtmp = tmpvec[0] * rotma[0*3 + 0] + 
    tmpvec[1] * rotma[1*3 + 0] + 
    tmpvec[2] * rotma[2*3 + 0];
    
    Real ytmp = tmpvec[0] * rotma[0 * 3 + 1] + 
    tmpvec[1] * rotma[1 * 3 + 1] + 
    tmpvec[2] * rotma[2 * 3 + 1];

    Real ztmp = tmpvec[0] * rotma[0 * 3 + 2] + 
    tmpvec[1] * rotma[1 * 3 + 2] + 
    tmpvec[2] * rotma[2 * 3 + 2];
    
    tmpvec[0] = xtmp;
    tmpvec[1] = ytmp;
    tmpvec[2] = ztmp;
    Real gamma_t = fabs(atan2(ztmp, ytmp));
    
    if (gamma_t > PI/4.0)
        gamma_t *= -1.0;
    //cout << "GAMMA = " << gamma_t <<endl;
    
    Real rotmatrix2[] = { 1.0, 0.0, 0.0,
        0.0, cos(gamma_t), sin(gamma_t),
        0.0, sin(gamma_t), cos(gamma_t)};
    
    //static Real rottmp[3][3];       
    for (int i = 0; i<3; i++){
        for (int j =0; j<3; j++){
            Real val = 0;
            for (int k=0; k<3; k++)
                val += rotma[i * 3 + k] * rotmatrix2[k * 3 + j];
            
            rotmatrix[i * 3 + j] = val;
            //printf("%d\n", i*3+j);
        }
    }
    

    //rotate
    if(!isRotm){
        for(int i = 0; i < 9 ; i ++){
            rotmatrix[i] = 0;
        }
    
        rotmatrix[0] = 1;
        rotmatrix[4] = 1;
        rotmatrix[8] = 1;
    }

    if(updown){
        rotmatrix[2] *= -1;
        rotmatrix[5] *= -1;
        rotmatrix[8] *= -1;
    }
    printf("ROTMATRIX:\n");
    printf("%f %f %f\n", rotmatrix[0], rotmatrix[1], rotmatrix[2]);
    printf("%f %f %f\n", rotmatrix[3], rotmatrix[4], rotmatrix[5]);
    printf("%f %f %f\n", rotmatrix[6], rotmatrix[7], rotmatrix[8]);

    return;                 
    
}

void fluxShader::setrotmatrix(REAL * alignvec, REAL * obsvec, REAL * centvec, bool updown){
    this->align_vec = alignvec;
    this->opos = obsvec;
    this->cpos = centvec;
    setrotm(updown);
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

