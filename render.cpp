#include <glew.h>
#include <GLUT/glut.h>
#include "buffers.h"
#include "shaders.h"
#include "render.h"
#include <iostream>
#include <sys/time.h>
#include <cmath>
using namespace std;

colorBuffer * CB;       //for final rendering
static unsigned int WSIZE, PSIZE;

void render::init(){
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  

    glGenTextures(1, &textureIni);
    glBindTexture(GL_TEXTURE_2D, textureIni);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, windowSize, windowSize, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // set its parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glBindTexture(GL_TEXTURE_2D, 0);

    // setup some generic opengl options
    glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
    glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
    glClampColorARB(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);


    glEnable(GL_POINT_SPRITE);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
   
    
    GLfloat sizes[2];
    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, sizes);
	glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 1000.0 );
	glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, sizes[0]);
	glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, sizes[1]);
    //printf("min and max point size %f, %f\n", sizes[0], sizes[1]);

    //must be this
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);

    glPointSize(pointSize);
}


void render::setMinMaxTex(){

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    fbuffer->bindTex();
    glEnable(GL_TEXTURE_2D);
    //glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
    
    glActiveTexture(GL_TEXTURE1);
    mbuffer->bindTex();
    glEnable(GL_TEXTURE_2D);
    //glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
}


void render::drawFlux(){
    Particles part;
    if(!reader->isOpen()){
        reader->open();
    }

    //bind buffer
    fbuffer->bindBuf();

    //bind texture
    glBindTexture(GL_TEXTURE_2D, textureIni);

    //begin shader
    fshader->begin();

    //setup shader parameters 
    fshader->setgeofac3f(orthsize, windowSize, pointSize);
    
    REAL vx = params->vposx;
    REAL vy = params->vposy;
    REAL vz = params->vposz;
    REAL vn = sqrt(vx * vx + vy * vy + vz * vz);
    vx = vx / vn;
    vy = vy / vn;
    vz = vz / vn;
    REAL costheta = vz;
    REAL sintheta = sqrt(1-vz * vz);
    REAL cosphi, sinphi;
    if(sintheta < 1.0e-8){
        cosphi = 1;
        sinphi = 0;
    }else{
        cosphi = vx / sintheta;
        sinphi = vy / sintheta;
    }
    //now use the rotatation matrix
    printf("%f, %f, %f, %f\n", costheta, sintheta, cosphi, sinphi);
    fshader->setzaxis3f(vx, vy, vz);
    fshader->setyaxis3f(-sinphi, cosphi, 0);
    fshader->setxaxis3f(cosphi*costheta, sinphi*costheta, -sintheta);
    
    fshader->setopos3f(params->oposx, params->oposy, params->oposz);
    
    //start drawing
    glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT); 
    glViewport(0,0,WSIZE, WSIZE); 
    
    //setup matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-orthsize, orthsize, -orthsize, orthsize, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //clear color
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //load particles here
    cout << "--------------------------------------------------"<<endl;
    cout << ("..10%..20%..30%..40%..50%..60%..70%..80%..90%.100%\n");
    int kk = 0; //particle number counter
    int outc = reader->getPartNum() * 2 / 100;
    while(reader->hasNext()){
        glBegin(GL_POINTS);
        int tp = 0;
        while(reader->hasNext() && tp < 10000){
            kk++;
            tp++;
            if(kk % (outc) == 0){
                cout << "*";
                cout.flush();
            }

            reader->readParticle(&part);
            glColor4f(part.mass, part.density, part.hsmooth, 1);
            glVertex3f(part.xpos, part.ypos, part.zpos);
            glColor4f(1,2,1,100);   
            glVertex3f(40, 0, -30);


        }
        glEnd();
        glFlush();

    }
    cout << endl;
    cout.flush();

    glFlush();

    glBindTexture(GL_TEXTURE_2D, 0);

    glPopAttrib();
    fshader->end(); 
    fbuffer->unbindBuf();
    reader->close();
}

void render::drawImage(){
    setMinMaxTex();
    //fbuffer->bindTex();
    cshader -> begin();
    cshader -> setmultitex();
    cbuffer -> bindBuf();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_POINT_SPRITE);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-orthsize, orthsize, -orthsize, orthsize, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);

    glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
    glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
    glVertex3f(-orthsize, -orthsize, 10);
    glMultiTexCoord2f(GL_TEXTURE0, 0.0, 1.0);
    glMultiTexCoord2f(GL_TEXTURE1, 0.0, 1.0);
    glVertex3f(-orthsize, orthsize, 10);
    glMultiTexCoord2f(GL_TEXTURE0, 1.0, 1.0);
    glMultiTexCoord2f(GL_TEXTURE1, 1.0, 1.0);
    glVertex3f(orthsize,  orthsize, 10);
    glMultiTexCoord2f(GL_TEXTURE0, 1.0, 0.0);
    glMultiTexCoord2f(GL_TEXTURE1, 1.0, 0.0);
    glVertex3f(orthsize,  -orthsize, 10);
    
    /*glTexCoord2i(0, 0); glVertex3f(-orthsize, -orthsize, 10);
    glTexCoord2i(0, 1); glVertex3f(-orthsize, orthsize, 10);
    glTexCoord2i(1, 1); glVertex3f(orthsize,  orthsize, 10);
    glTexCoord2i(1, 0); glVertex3f(orthsize,  -orthsize, 10);*/

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();


    cbuffer ->unbindBuf();
    cshader -> end();


}

void rendsenc(){
    //glClientActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);

    CB ->unbindBuf();
    
    CB->bindTex();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_POINT_SPRITE);
    glEnable(GL_TEXTURE_2D);


    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2, 2, -2, 2, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);

    glTexCoord2i(0, 0); glVertex3f(-2, -2, 10);
    glTexCoord2i(0, 1); glVertex3f(-2, 2, 10);
    glTexCoord2i(1, 1); glVertex3f(2,  2, 10);
    glTexCoord2i(1, 0); glVertex3f(2,  -2, 10);

    glEnd();
  
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glutSwapBuffers();


}

void ReshapeFunc(int width, int height)
{
    glViewport(0, 0, width, height);
}

void KeyboardFunc(unsigned char key, int x, int y)
{
  int foo;
  foo = x + y; //Has no effect: just to avoid a warning 
  if ('q' == key || 'Q' == key || 27 == key)
      exit(0);
}


void render::start(int argc, char **argv){
    windowSize = params->WSIZE;
    pointSize = params->PSIZE;

    //initialize glut and glew
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(windowSize, windowSize);
    glutCreateWindow("Dark Matter rendering!");
   
    glewInit();
    glewExperimental = GL_TRUE;        

    //set up shaders and buffers
    if(!initialed){
        fshader = new fluxShader();
        cshader = new colorShader();
        fbuffer = new fluxBuffer(windowSize, windowSize);
        cbuffer = new colorBuffer(windowSize, windowSize);
        mbuffer = new minmaxBuffer(windowSize, windowSize);
        mshader = new minMaxShader();
        mbuffer->setsize(windowSize, 2);
        mbuffer->mshader = mshader;
        fbuffer->setBuffer();
        //mbuffer->setBuffer();
        initialed = true;
    }

    //set the global pointers
    WSIZE = windowSize;
    PSIZE = pointSize;
    CB = cbuffer;

    //initialize enviroment    
    init();
    
    //setup blending   
    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE,GL_ONE);    //blending

    //set up glut
    glutDisplayFunc(&rendsenc);
    glutReshapeFunc(&ReshapeFunc);
    glutKeyboardFunc(&KeyboardFunc);

    //check shaders
    if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
    else {
		printf("No GLSL support\n");
		exit(1);
    }
    
    //starting drawing
    cout << "Start rendering..." << endl;
    timeval tim;
    gettimeofday(&tim, NULL);
    double t1=tim.tv_sec+(tim.tv_usec/1000000.0);   

    drawFlux();
    /*float a[windowSize*windowSize];
    glReadPixels(0,
                 0,
                 windowSize,
                 windowSize,
                 GL_RED,
                 GL_FLOAT,
                 a);*/
    mbuffer->setInput(fbuffer->getTex());
    mbuffer->findMinMax();
    cbuffer->setBuffer();
    drawImage();
    delete fbuffer;
    delete mbuffer;
    /*float b = 0;
    for(int i = 0; i<windowSize; i++){
        for(int j =0; j< windowSize; j++){
            b = max(b, a[i*windowSize + j]);
            
        }
    }
    printf("%g\n",b);*/
    gettimeofday(&tim, NULL);
    double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
    
    printf("End rendering. %.6lf seconds elapsed\n", t2-t1);
    cout << "--------------------------------------------------"<<endl;
    //CB->setTex(fbuffer->getTex());
    //CB->setTex(mbuffer->getTex());
    glutMainLoop();

}
