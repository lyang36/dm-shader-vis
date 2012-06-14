#include <stdlib.h>
#include <glew.h>
#include <GLUT/glut.h> // darwin uses glut.h rather than GL/glut.h
#include <stdio.h>
#include <string>
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
    issetup = false;
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
    if(issetup){
        return;
    }
    issetup = true;
    
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



void minmaxBuffer::gentex2(){
    if(texgenned){
        return;
    }
    texgenned = true;
    glGenTextures(1, &textureId2);
    glBindTexture(GL_TEXTURE_2D, textureId2);
    
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
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void minmaxBuffer::draw(){
    
    glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT); 
    glViewport(0,0,mapsize, mapsize); 
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_POINT_SPRITE);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    int orthsize = 10;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-orthsize, orthsize, -orthsize, orthsize, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glBegin(GL_QUADS);
    
    glTexCoord2i(0, 0); glVertex3f(-orthsize, -orthsize, 10);
    glTexCoord2i(0, 1); glVertex3f(-orthsize, orthsize, 10);
    glTexCoord2i(1, 1); glVertex3f(orthsize,  orthsize, 10);
    glTexCoord2i(1, 0); glVertex3f(orthsize,  -orthsize, 10);
    
    glEnd();
    glFinish(); //finish the drawing
    
    glPopAttrib();

}

void minmaxBuffer::findMinMax(){
    GLuint temptex;
    //first step: fill the map
    setBuffer();
    gentex2();
    bindBuf();
    
    //fill the map
    mshader->beginfill();   //start the fill shader
    mshader->setsppos(0.5, 0.5);    //set to the center point, must set
    draw();                 //draw a sqare on the whole screen
    mshader->end();
    
    //swap the texture
    temptex = textureId;
    setTex(textureId2);
    attachTex();            //attach to the buffer
    textureId2 = temptex;   //now textureId2 has the filled map
    
    //second step ... nth step find the minmax 
    glBindTexture(GL_TEXTURE_2D, textureId2);
    mshader->beginsearching();
    int blocksize = 1;
    while(blocksize < mapsize){
        mshader->setparams(mapsize, blocksize, scale);
        draw();         //draw a square
        glBindTexture(GL_TEXTURE_2D, 0);
        
        //swap the texture
        temptex = textureId;
        setTex(textureId2);
        attachTex();            //attach to the buffer
        textureId2 = temptex;   //now textureId2 has the nexstage map
        
        glBindTexture(GL_TEXTURE_2D, textureId2);
        blocksize *= scale;
    }
    
    unbindBuf();    //finished
}
