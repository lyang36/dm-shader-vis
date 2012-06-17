#ifdef __APPLE__
#include <glew.h>
#include <GLUT/glut.h> // darwin uses glut.h rather than GL/glut.h
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif


#include "buffers.h"
#include "shaders.h"
#include "render.h"
#include <iostream>
#include <sys/time.h>
#include <cmath>
using namespace std;

colorBuffer * CB, *CBL, *CBU;       //for final rendering
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

void render::drawFlux(){
    Particles part;
    if(!reader->isOpen()){
        reader->open();
    }

    
    //bind buffer
    //fbufferL->bindBuf();
    
    //bind texture
    glBindTexture(GL_TEXTURE_2D, textureIni);
    
    
    {//setup shaders L and U
        
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
    
        //begin shader
        fshaderL->begin();
    
        //setup shader parameters 
        fshaderL->setgeofac3f(orthsize, windowSize, pointSize);
        
        //now use the rotatation matrix
        fshaderL->setzaxis3f(vx, vy, vz);
        fshaderL->setyaxis3f(-sinphi, cosphi, 0);
        fshaderL->setxaxis3f(cosphi*costheta, sinphi*costheta, -sintheta);
        fshaderL->setopos3f(params->oposx, params->oposy, params->oposz);
        fshaderL->end();    
    
        //begin shader
        fshaderU->begin();
    
        //setup shader parameters 
        fshaderU->setgeofac3f(orthsize, windowSize, pointSize);
    
        //now use the rotatation matrix
        fshaderU->setzaxis3f(-vx, -vy, -vz);
        fshaderU->setyaxis3f(-sinphi, cosphi, 0);
        fshaderU->setxaxis3f(cosphi*costheta, sinphi*costheta, -sintheta);
        fshaderU->setopos3f(params->oposx, params->oposy, params->oposz);
        fshaderU->end();     
    
    }
    
    fbufferL->bindBuf();
    {
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
    }
    fbufferL->unbindBuf();
    
    fbufferU->bindBuf();
    {
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
    }
    fbufferU->unbindBuf();   
    
    
    //load particles here
    cout << "--------------------------------------------------"<<endl;
    cout << ("..10%..20%..30%..40%..50%..60%..70%..80%..90%.100%\n");
    int kk = 0; //particle number counter
    
    int outc = reader->getPartNum() * 2 / 100 / (params->CPU_MEM) ;
    if(outc == 0){
        outc = 1;
    }

    while(reader->hasNext()){
        
        
        
        reader->loadBuffer();
        GLfloat * vetexarray = (GLfloat *) reader->getBuf();
        glEnableClientState (GL_VERTEX_ARRAY);
        glEnableClientState (GL_COLOR_ARRAY);
        glColorPointer (3, GL_FLOAT, 6*sizeof(GLfloat), &(vetexarray[0]));
        glVertexPointer (3, GL_FLOAT, 6*sizeof(GLfloat), &(vetexarray[3]));
        
        //lower sphere
        fbufferL->bindBuf();
        fshaderL->begin();
        {
            glDrawArrays(GL_POINTS, 0, reader->getMemparts());
            glFlush();
        }
        fshaderL->end();
        fbufferL->unbindBuf();
        
        //upper sphere
        fbufferU->bindBuf();
        fshaderU->begin();
        {
            glDrawArrays(GL_POINTS, 0, reader->getMemparts());
            glFlush();
        }
        fshaderU->end();
        fbufferU->unbindBuf();       
        
        reader -> move2bufEnd();
        
        glDisableClientState (GL_VERTEX_ARRAY);
        glDisableClientState (GL_COLOR_ARRAY);
        //cout << reader->getMemparts() << endl;
        
        
        
        
        kk++;
        if(kk % (outc) == 0){
            cout << "*";
            cout.flush();
        }
    }
    cout << endl;
    cout.flush();


    glBindTexture(GL_TEXTURE_2D, 0);

    glPopAttrib();
    reader->close();
}


void render::findMinMax(float &fluxmin, float &fluxmax){
    //lower buffer
    fbufferL->bindBuf();
    float *pic = new float[windowSize*windowSize];
    glReadPixels(0, 0, windowSize, windowSize,
                 GL_RED, GL_FLOAT, pic);
    fluxmax = 0;
    fluxmin = 1.0e36;
    float average;
    int pixss = windowSize * windowSize;
    for(int i = 0; i < pixss; i++){
        float x = i/windowSize - windowSize / 2.0;
        float y = i%windowSize - windowSize / 2.0;
        float r = windowSize / 2.0;
        if(pic[i] == 0 ){
            pic[i] = pic[pixss / 2];
        }
        if(x*x + y*y < r*r-10){
            average += pic[i] / (float) pixss;
        }
        if(fluxmax <= pic[i]) fluxmax = pic[i];
        else if(fluxmin >= pic[i]) fluxmin = pic[i];
        
    }
    fbufferL->unbindBuf();
    
    
    fbufferU->bindBuf();
    glReadPixels(0, 0, windowSize, windowSize,
                 GL_RED, GL_FLOAT, pic);
    for(int i = 0; i < pixss; i++){
        float x = i/windowSize - windowSize / 2.0;
        float y = i%windowSize - windowSize / 2.0;
        float r = windowSize / 2.0;
        if(pic[i] == 0 ){
            pic[i] = pic[pixss / 2];
        }
        if(x*x + y*y < r*r-10){
            average += pic[i] / (float) pixss;
        }
        if(fluxmax <= pic[i]) fluxmax = pic[i];
        else if(fluxmin >= pic[i]) fluxmin = pic[i];
        
    }
    average /= 2.0;
    printf("min = %f, max = %f, middle: %f\n", fluxmin, fluxmax, pic[pixss/2]);
    fluxmin = average / 10.0;
    fluxmax = average * 10.0;
    printf("fmin = %f, fmax = %f, average: %f\n", fluxmin, fluxmax, average);
    fbufferU->unbindBuf();
    
    delete pic;
}

void render::drawImage(){
    float fluxmax;
    float fluxmin;
    
    findMinMax(fluxmin, fluxmax);
    
    fbufferL->bindTex();
    cshaderL -> begin();
    cshaderL -> setminmax(fluxmin, fluxmax);
    cbufferL -> bindBuf();

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
    
    glTexCoord2i(0, 0); glVertex3f(-orthsize, -orthsize, 10);
    glTexCoord2i(0, 1); glVertex3f(-orthsize, orthsize, 10);
    glTexCoord2i(1, 1); glVertex3f(orthsize,  orthsize, 10);
    glTexCoord2i(1, 0); glVertex3f(orthsize,  -orthsize, 10);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    
    cbufferL ->unbindBuf();
    cshaderL -> end();
    fbufferL -> unbindTex();
    
    
    fbufferU->bindTex();
    cshaderU -> begin();
    cshaderU -> setminmax(fluxmin, fluxmax);
    cbufferU -> bindBuf();
    
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
    
    glTexCoord2i(0, 0); glVertex3f(-orthsize, -orthsize, 10);
    glTexCoord2i(0, 1); glVertex3f(-orthsize, orthsize, 10);
    glTexCoord2i(1, 1); glVertex3f(orthsize,  orthsize, 10);
    glTexCoord2i(1, 0); glVertex3f(orthsize,  -orthsize, 10);
    
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    
    cbufferU ->unbindBuf();
    cshaderU -> end();
    fbufferU -> unbindTex();


}

void rendsenc(){
    //glClientActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);

    CBL ->unbindBuf();
    CBU ->unbindBuf();
    

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_POINT_SPRITE);
    glEnable(GL_TEXTURE_2D);


    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-4, 4, -2, 2, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    CBL ->bindTex();
    //glPushMatrix();
    //glTranslatef(-0.02, 0, 0);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex3f(-4, -2, 10);
    glTexCoord2i(0, 1); glVertex3f(-4, 2, 10);
    glTexCoord2i(1, 1); glVertex3f(0,  2, 10);
    glTexCoord2i(1, 0); glVertex3f(0,  -2, 10);
    glEnd();
    //glPopMatrix();
    
    CBU ->bindTex();
    //glPushMatrix();
    //glTranslatef(0.02, 0, 0);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex3f(0, -2, 10);
    glTexCoord2i(0, 1); glVertex3f(0, 2, 10);
    glTexCoord2i(1, 1); glVertex3f(4,  2, 10);
    glTexCoord2i(1, 0); glVertex3f(4,  -2, 10);
    glEnd();
    //glPopMatrix();
  
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
  /*else if(key == 'c'){
      if(CB == CBL){
          CB = CBU;
      }else{
          CB = CBL;
      }
      glutPostRedisplay();
  }*/
}


void render::start(int argc, char **argv){
    windowSize = params->WSIZE;
    pointSize = params->PSIZE;

    //initialize glut and glew
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(2 * windowSize, windowSize);
    glutCreateWindow("Dark Matter rendering!");
   
    glewInit();
    glewExperimental = GL_TRUE;        

    //set up shaders and buffers
    if(!initialed){
        fshaderL = new fluxShader();
        cshaderL = new colorShader();
        fbufferL = new fluxBuffer(windowSize, windowSize);
        cbufferL = new colorBuffer(windowSize, windowSize);
        fbufferL->setBuffer();
        
        fshaderU = new fluxShader();
        cshaderU = new colorShader();
        fbufferU = new fluxBuffer(windowSize, windowSize);
        cbufferU = new colorBuffer(windowSize, windowSize);
        fbufferU->setBuffer();
        
        initialed = true;
    }

    //set the global pointers
    WSIZE = windowSize;
    PSIZE = pointSize;
    CBL = cbufferL;
    CBU = cbufferU;
    CB = CBL;

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

    cbufferL->setBuffer();
    cbufferU->setBuffer();
    drawImage();
    delete fbufferL;
    delete fbufferU;
    gettimeofday(&tim, NULL);
    double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
    printf("End rendering. %.6lf seconds elapsed\n", t2-t1);
    cout << "--------------------------------------------------"<<endl;
    glutMainLoop();

}



