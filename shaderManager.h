#ifndef __LY_SHADERMANAGER__
#define __LY_SHADERMANAGER__
//#include <glew.h>
//#include <GLUT/glut.h>
#include <cstdlib>
#include <cstdio>
#ifdef __APPLE__
#include <glew.h>
#include <GLUT/glut.h> // darwin uses glut.h rather than GL/glut.h
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

class shaderObj{
public: 
    GLhandleARB vertexShader;
    GLhandleARB fragmentShader;
    GLhandleARB geometryShader;
    GLhandleARB progmObj;
    void begin();
    void end();
    
};

class shaderManager{
private:
    void printLog(GLhandleARB obj);
    char * textFileRead(const char * fn);
public:
    shaderObj * loadShaderFile(const char * vfile, const char * ffile);
    shaderObj * loadShaderFile(const char * vfile, const char * gfile,
                               const char * ffile); //load a geometry shader
    shaderObj * loadShader(const char *vf, const char * ff);
    shaderObj * loadShader(const char *vf, const char * gf,
                           const char * ff);        //load a geometry shader
};


#endif
