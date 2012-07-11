#include <stdlib.h>
#include <cmath>

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>


#ifdef __APPLE__
#include <glew.h>
#include <GLUT/glut.h> // darwin uses glut.h rather than GL/glut.h
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "buffers.h"

using namespace std;

void buffer::checkbuffer(){
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if(status != GL_FRAMEBUFFER_COMPLETE){
        fboUsed = false;
        printf("No frame buffer!\n");
        exit(-1);
    }
}

buffer::buffer(unsigned int w, unsigned int h){
    twidth = w;
    theight = h;
}


buffer::~buffer(){
    glDeleteFramebuffers(1, &fboId);
    glDeleteRenderbuffers(1, &rboId);
    glDeleteTextures(1, &textureId);
}

GLuint buffer::getTex(){
    return textureId;
}

void buffer::bindTex(){
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void buffer::unbindTex(){
    glBindTexture(GL_TEXTURE_2D, 0);
}

void buffer::setTex(GLuint tex){
    textureId = tex;
}

void buffer::genTex(){
    glGenTextures(1, &textureId);
    bindTex();
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, twidth, theight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, 0);
    
    // set its parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // setup some generic opengl options
    glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
    glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
    glClampColorARB(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);
    
    unbindTex();

}


void buffer::attachTex(){
    // attach the texture to FBO color attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, textureId, 0);

}


void buffer::genBuffer(){
    // create a renderbuffer object to store depth info
    //GLuint rboId;
    glGenRenderbuffers(1, &rboId);
    glBindRenderbuffer(GL_RENDERBUFFER, rboId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          twidth, theight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    // create a framebuffer object
    //GLuint fboId;
    glGenFramebuffers(1, &fboId);
    //glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    bindBuf();
    // attach the renderbuffer to depth attachment point
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, rboId);
    //unbindBuf();
    printf("Buffer generated!\n");
    //printf("frame: %d, render: %d \n", fboId, rboId);
}

void buffer::setBuffer(){
    genTex();
    genBuffer();
    attachTex();
    checkbuffer();
    unbindBuf();
}

void buffer::bindBuf(){

    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
}

void buffer::unbindBuf(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void buffer::readTex(void* tex){
    
}

void buffer::saveImg(string filename){
    
}



float profile(float * r, float * r0, float dtheta){ 
    
    float costheta = r[0] * r0[0] + r[1] * r0[1] + r[2] * r0[2];
    float t2 = acos(costheta);
    t2 = t2*t2;
    float d2 = t2 / dtheta / dtheta;
    return exp(- 1.5 * d2);
}

//reverse stereoprojection
void prev(float * xy, float * r){
    float r2 = xy[0]*xy[0] + xy[1]*xy[1];
    r[0] = 2.0 * xy[0]/(1.0 + r2);
    r[1] = 2.0 * xy[1]/(1.0 + r2);
    r[2] = (r2 - 1.0)/(r2 + 1.0);
}

float projprofile(float * xy, float * r0, float fc, float dtheta){
    float r[3];
    prev(xy, r);
    return fc * profile(r, r0, dtheta);
}


void fluxBuffer::loadnorm(){
    //check whether the file is exsisted
    //if so, read from file
    ifstream infile;
    infile.open(normfile.c_str(), ios::in | ios::binary);
    if(infile.good())
    {
        int tempsize;
        infile.read((char *) &tempsize, sizeof(int));
        if(tempsize != normMapRes){
            //the file do not match
            printf("Current norm file %s do not match required, please delete or move it.\n", normfile.c_str());
            infile.close();
            exit(1);
        }
        printf("Loading norm map from file %s ...\n", normfile.c_str());
        infile.read((char *) normtextbuf, sizeof(float) * normMapRes * normMapRes);
        printf("Loaded ...\n", normfile.c_str());
        infile.close();
        return;
    }else{
        infile.close();
        printf("Normalization file not found, generating ...\n");
        //otherwise, create the file
        float r0,r1;
        float vr0[3];
        float xy[2];
        float dtheta, theta, phi;
        float norm = 0;
        int _ck = 0;
        for(int i = 0; i < normMapRes; i ++){
            for(int j = 0; j < normMapRes; j ++){
                if(_ck % (normMapRes*normMapRes / 20) == 0 ){
                    printf("%d%% ", _ck * 100/(normMapRes*normMapRes));
                    cout.flush();
                }
                _ck++;
                
                r0 = ((float) i) / ((float) (normMapRes - 1));
                r1 = ((float) j) / ((float) (normMapRes - 1));

                float theta1 = acos((-1.0 + (r1+r0)*(r1+r0))/(1.0 + (r1+r0)*(r1+r0)));
                float theta2 = acos((-1.0 + (r0-r1)*(r0-r1))/(1.0 + (r0-r1)*(r0-r1)));
            
                dtheta = abs(theta1 - theta2) / 2.0;
                theta = abs(theta1 + theta2) / 2.0;
                if(((r1+r0)) * ((r0-r1)) < 0.0){
                    dtheta =abs(2*PI - theta1 - theta2) / 2.0;
                    theta = abs(2*PI - theta2 + theta1) / 2.0;
                }
            
                phi = 0;
            
                vr0[0] = sin(theta);
                vr0[1] = 0.0;
                vr0[2] = cos(theta);
                norm = 0;
                //printf(" %f, %f, %f  %f \n", r0, r1, theta, dtheta);
                if(dtheta > 0.0){
                    for(float xx = - floor((normMapRes - 1) * r1); xx <= floor((normMapRes - 1) * r1); xx ++){
                        for(float yy = - floor((normMapRes - 1) * r1); yy <= floor((normMapRes - 1) * r1); yy ++){
                            float _x = xx / (floor((normMapRes - 1) * r1));
                            float _y = yy / (floor((normMapRes - 1) * r1));
                            float _r2 = _x*_x + _y*_y;
                            if(_r2 <= r1 * r1){
                                xy[0] = _x + r0;
                                xy[1] = _y;
                                float rp2 = xy[0] * xy[0] + xy[1]*xy[1];
                                norm += projprofile(xy, vr0, 1.0, dtheta) * 4.0 / (1+rp2)/(1+rp2);
                                // / (float)(normMapRes * normMapRes)
                            }
                        }
                    }
                }
                normtextbuf[j * normMapRes + j] = norm;
                //printf("%f\n", norm);
            }
        }
        printf("Saving to file %s ... \n", normfile.c_str());
        ofstream oufile;
        //printf("ok\n");
        oufile.open(normfile.c_str(), ios::out | ios::binary);
        //printf("ok\n");
        oufile.write((char * ) &normMapRes, sizeof(int));
        //printf("ok\n");
        oufile.write((char * ) normtextbuf, normMapRes * normMapRes * sizeof(float));
        //printf("ok\n");
        oufile.close();
        //printf("Done \n");
    }
}

//18885004802
