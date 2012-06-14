#ifndef __LY_SHADERMANAGER__
#define __LY_SHADERMANAGER__
#include <glew.h>
#include <GLUT/glut.h>

class shaderObj{
public: 
    GLhandleARB vertexShader;
    GLhandleARB fragmentShader;
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
    shaderObj * loadShader(const char *vf, const char * ff);
};


#endif
