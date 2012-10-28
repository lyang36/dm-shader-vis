#include <iostream>

//#ifdef _WIN32 || _WIN64
#if defined(_WIN32) || defined(_WIN64)
#include <time.h>
#include <windows.h>
#include "gettime.h"
#define round(x) floor(x)
#else
#include <sys/time.h>
#endif

#include <cmath>
#include <fstream>

#include "buffers.h"
#include "shaders.h"
#include "render.h"

#include "healpix.h"
#include "types.h"

#ifdef __APPLE__
#include <glew.h>
#include <GLUT/glut.h> // darwin uses glut.h rather than GL/glut.h
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
using namespace std;

colorBuffer * CB, *CBL, *CBU;       //for final rendering
static unsigned int WSIZE, POINTSIZE;
string picfile;
bool isonscreen = false;

void render::init(){
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  

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
    //Particles part;
    if(!reader->isOpen()){
        reader->open();
    }

    
    //bind buffer
    //fbufferL->bindBuf();
    
    //bind texture
    if(params->isUseNormMap){
        printf("Generating normalization buffer...\n");
        fbufferL -> setMapRes(windowSize, params->PSIZE);
        fbufferL -> setNormTex();
        printf("Norm map generated.\n");
    }else{
        glBindTexture(GL_TEXTURE_2D, textureIni);
    }
    
    
    {//setup shaders L and U
        
        /*REAL vx = params->vposx;
        REAL vy = params->vposy;
        REAL vz = params->vposz;
        REAL vn = sqrt(vx * vx + vy * vy + vz * vz);
        vx = vx / vn;
        vy = vy / vn;
        vz = vz / vn;
        //REAL costheta = vz;
        REAL sintheta = sqrt(1-vz * vz);
        REAL cosphi, sinphi;
        if(sintheta < 1.0e-8){
            cosphi = 1;
            sinphi = 0;
        }else{
            cosphi = vx / sintheta;
            sinphi = vy / sintheta;
        }*/
    
        //begin shader
        fshaderL->begin();
        fshaderL->setIsUseRotm(params->isROTM);

        //setup shader parameters 
        fshaderL->setgeofac3f(orthsize, windowSize, pointSize);
        
        //now use the rotatation matrix
        //fshaderL->setzaxis3f(vx, vy, vz);
        //fshaderL->setyaxis3f(-sinphi, cosphi, 0);
        //fshaderL->setxaxis3f(cosphi*costheta, sinphi*costheta, -sintheta);
        fshaderL->setopos3f(params->oposx, params->oposy, params->oposz);
        fshaderL->setrotmatrix(params->vvec, params->opos, params->cpos, false);
        fshaderL->setusenormmap(params->isUseNormMap);
        
        fshaderL->end();    
    
        //begin shader
        fshaderU->begin();
        fshaderU->setIsUseRotm(params->isROTM);

    
        //setup shader parameters 
        fshaderU->setgeofac3f(orthsize, windowSize, pointSize);
    
        //now use the rotatation matrix
        fshaderU->setopos3f(params->oposx, params->oposy, params->oposz);
        //reflect the view vector
        fshaderU->setrotmatrix(params->vvec, params->opos, params->cpos, true);
        fshaderU->setusenormmap(params->isUseNormMap);
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

    while(reader->hasNext())
    {
        

        GLfloat * vetexarray = (GLfloat *) reader->getBuf();
        glEnableClientState (GL_VERTEX_ARRAY);
        glEnableClientState (GL_COLOR_ARRAY);
        glColorPointer (3, GL_FLOAT, 6*sizeof(GLfloat), &(vetexarray[0]));
        glVertexPointer (3, GL_FLOAT, 6*sizeof(GLfloat), &(vetexarray[3]));
        
	timeval tim;
        gettimeofday(&tim, NULL);
        double t1=tim.tv_sec+(tim.tv_usec/1000000.0);

        //lower sphere
        fbufferL->bindBuf();
        fshaderL->begin();
        {
            glDrawArrays(GL_POINTS, 0, reader->getMemparts());
            //glFlush();
            //printf("Particles: %d\n", reader->getMemparts());
        }
        fshaderL->end();
        fbufferL->unbindBuf();
        
        //upper sphere
        fbufferU->bindBuf();
        fshaderU->begin();
        {
            glDrawArrays(GL_POINTS, 0, reader->getMemparts());
            //glFlush();
            //printf("Particles: %d\n", reader->getMemparts());
        }
        fshaderU->end();
        fbufferU->unbindBuf();       

        //TIME MEASURING
        if(params->isTimeMeasure){
            glFinish();
        }
        
        gettimeofday(&tim, NULL);
        double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
        rendertime += t2 - t1;
        reader -> move2bufEnd();
        
        glDisableClientState (GL_VERTEX_ARRAY);
        glDisableClientState (GL_COLOR_ARRAY);
        //cout << reader->getMemparts() << endl;
        
        
        
        
        kk++;
        if(kk % (outc) == 0){
            cout << "*";
            cout.flush();
        }
        reader->loadBuffer();
    }
    cout << endl;
    cout.flush();


    glBindTexture(GL_TEXTURE_2D, 0);

    glPopAttrib();
    reader->close();
}
 
void render::readFluxMap(){
    glPixelStorei(GL_PACK_ALIGNMENT, 4);  
    fluxmapL = new float[windowSize*windowSize];
    fluxmapU = new float[windowSize*windowSize];

    fbufferL -> bindTex();
    glGetTexImage(GL_TEXTURE_2D,0,GL_RED,GL_FLOAT,fluxmapL);
    fbufferL->unbindTex();
    
    fbufferU -> bindTex();
    glGetTexImage(GL_TEXTURE_2D,0,GL_RED,GL_FLOAT,fluxmapU);
    fbufferU->unbindTex();

    
}

void render::findMinMax(float &fluxmin, float &fluxmax){
    //lower buffer
    //fbufferL->bindBuf();
    //float *pic = new float[windowSize*windowSize];
    //glReadPixels(0, 0, windowSize, windowSize,
    //             GL_RED, GL_FLOAT, pic);
    fluxmax = 0;
    fluxmin = 1.0e36;
    double average = 0.0;
    double total = 0.0;
    
    int pixss = windowSize * windowSize;
    for(int i = 0; i < pixss; i++){
        float x = (int)(i/windowSize) - windowSize / 2.0;
        float y = (int)(i%windowSize) - windowSize / 2.0;
        float r = windowSize / 2.0;
        if(x*x + y*y <= r*r){
            //float pr = (x*x + y*y) / (windowSize / 2.0) / (windowSize / 2.0) ;
            average += (fluxmapL[i] + fluxmapU[i])/ (float) pixss;
            total += (fluxmapL[i] + fluxmapU[i]);// * (1 + pr) * (1 + pr) / 4.0;
        }else{
            continue;
        }
        if(fluxmax <= fluxmapL[i]) fluxmax = fluxmapL[i];
        else if(fluxmin >= fluxmapL[i]) fluxmin = fluxmapL[i];
        if(fluxmax <= fluxmapU[i]) fluxmax = fluxmapU[i];
        else if(fluxmin >= fluxmapU[i]) fluxmin = fluxmapU[i];
    }
    
    average /= 2.0;
    printf("min = %f, max = %f\n", fluxmin, fluxmax);
    fluxmax = (fluxmax + average) / 4;
    fluxmin = (average + fluxmin) / 8;
    //average = average * 2.0 * pixss *params->FLUXFACTOR/(4*PI/12.0/512.0/512.0);
    this->totalFlux = total;
    printf("fmin = %f, fmax = %f, average: %f total: %f\n", fluxmin, fluxmax, average, total);
    //fbufferU->unbindBuf();
    
    //delete pic;
}

void render::drawImage(){
    float fluxmax=0;
    float fluxmin=0;
    
    readFluxMap();
    findMinMax(fluxmin, fluxmax);
    
    
    if(params->OUTFILE != ""){
        printf("Save to file \"%s\"...\n", (params->OUTFILE).c_str());
        saveFluxMap();
    }
    
    if(params->HEALPIXFILE != ""){
        printf("Convert and save to healpix file \"%s\"...\n", (params->HEALPIXFILE).c_str());
        saveHealPix();
    }
    
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
	glViewport(0,0,2*WSIZE, WSIZE); 
    glActiveTexture(GL_TEXTURE0);

    

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
    
	if(isonscreen){
		glutSwapBuffers();
	}


}

void ReshapeFunc(int width, int height)
{
    glViewport(0, 0, width, height);
}

void savePic(){
    glReadBuffer(GL_FRONT);
    printf("Save pic to file: %s ...\n", picfile.c_str());
    //save to file
    ofstream fScreenshot(picfile.c_str(), ios::out | ios::binary);
    if (!fScreenshot.good()) {
        printf("Open pic file error.\n");
    }else{
        GLubyte *pixels = new GLubyte[WSIZE*2*WSIZE*3];
        glReadPixels(0, 0, WSIZE*2, WSIZE, GL_RGB,
                     GL_UNSIGNED_BYTE, pixels);
        unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
        unsigned char header[6] = {WSIZE*2%256,WSIZE*2/256,
            WSIZE%256,WSIZE/256,24,0};
        //convert to BGR format
        unsigned char temp;
        unsigned int i = 0;
        while (i < WSIZE*2*WSIZE*3)
        {
            temp = pixels[i];       //grab blue
            pixels[i] = pixels[i+2];//assign red to blue
            pixels[i+2] = temp;     //assign blue to red
            i += 3;     //skip to next blue byte
        }
        fScreenshot.write((char *)TGAheader, 12);
        fScreenshot.write((char *)header, 6);
        fScreenshot.write((char *)pixels, WSIZE*2*WSIZE*3);
        fScreenshot.close();
        printf("Saving ok.\n");
        delete pixels;
    }
}

void KeyboardFunc(unsigned char key, int x, int y)
{
  //int foo;
  //foo = x + y; //Has no effect: just to avoid a warning 
  if ('q' == key || 'Q' == key || 27 == key)
      exit(0);
  else if(key == 's' && picfile!=""){
      savePic();
  }
}


void render::start(int argc, char **argv){
    windowSize = params->WSIZE;
    pointSize = params->PSIZE;

    //initialize glut and glew
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(2 * windowSize, windowSize);
    glutCreateWindow("Dark Matter rendering!");

    if(!(params->isOnScreenRend)){
        glutHideWindow();
    }

    glewExperimental = GL_TRUE; 
    glewInit();

    if(!(params->isOnScreenRend)){
        glutHideWindow();
    }

    //glewExperimental = GL_TRUE; 

    //if(glewIsSupported("GL_VERSION_2_0")) cout << "true" <<endl;
	    //check shaders
    if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
    else {
		printf("No GLSL support\n");
		exit(1);
    }
    


    //set up shaders and buffers
    if(!initialed){

        fshaderL = new fluxShader(params->spriteVertexShader, params->spriteFragmentShader);
        cshaderL = new colorShader();
        fbufferL = new fluxBuffer(windowSize, windowSize);
        cbufferL = new colorBuffer(windowSize, windowSize);
        fbufferL->setBuffer();
        
        fshaderU = new fluxShader(params->spriteVertexShader, params->spriteFragmentShader);
        cshaderU = new colorShader();
        fbufferU = new fluxBuffer(windowSize, windowSize);
        cbufferU = new colorBuffer(windowSize, windowSize);
        fbufferU->setBuffer();
        
        initialed = true;
    }

    //set the global pointers
    WSIZE = windowSize;
    POINTSIZE = pointSize;
    CBL = cbufferL;
    CBU = cbufferU;
    CB = CBL;

    //initialize enviroment    
    init();
    //setup blending   
    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE,GL_ONE);    //blending


    //starting drawing
    cout << "Start rendering..." << endl;
    timeval tim;
    gettimeofday(&tim, NULL);
    double t1=tim.tv_sec+(tim.tv_usec/1000000.0);   

    //cbufferL->setBuffer();
    //cbufferU->setBuffer();
    
    drawFlux();

    cbufferL->setBuffer();
    cbufferU->setBuffer();
    drawImage();
	
    CBL ->unbindBuf();
    CBU ->unbindBuf();
    delete fbufferL;
    delete fbufferU;
    gettimeofday(&tim, NULL);
    double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
    if(params->isTimeMeasure){
        printf("End rendering. %.6lf seconds elapsed. Rendertime: %.6lf. Reading time: %.6lf\n",
               t2-t1, rendertime, t2-t1-rendertime);
    }else{
        printf("End rendering. %.6lf seconds elapsed.\n",
               t2-t1);
    }
    cout << "--------------------------------------------------"<<endl;
    delete fluxmapL;
    delete fluxmapU;
    picfile = params->PICFILE;
    
    if(params->isOnScreenRend){
		isonscreen = true;
        //set up glut
        glutDisplayFunc(&rendsenc);
        glutReshapeFunc(&ReshapeFunc);
        glutKeyboardFunc(&KeyboardFunc);
        glutMainLoop();
    }else{
		fluxBuffer * screenbuffer = new fluxBuffer(2.0*windowSize, windowSize);
		screenbuffer->setBuffer();
		screenbuffer->bindBuf();
        rendsenc();
        //Save the picture
        savePic();
		screenbuffer->unbindBuf();
		delete screenbuffer;
    }
    

    
    delete fshaderL;
    delete fshaderU;   
    delete cshaderL;
    delete cshaderU;
    delete cbufferU;
    delete cbufferL; 

}

double render::_getpixflux(int x1, int y1, bool isupshere){
    //inside the circle
    double f11 = 0;
    int d = round(windowSize / 2.0);
    double _r = sqrt((double)(x1 * x1 + y1 * y1)/(double)(d * d));
    if(x1 * x1 + y1 * y1 <= d * d){
        if(x1 < -(d)) x1 = -(d);
        if(x1 > d-1) x1 = d-1;
        if(y1 < -(d-1)) y1 = -(d-1);
        if(y1 > d) y1 = d;
        if(isupshere){
            f11 = fluxmapU[(d - y1) * windowSize + x1 + d];
        }else{
            f11 = fluxmapL[(d - y1) * windowSize + x1 + d];
        }
        f11 =  f11;// / (4.0 / (1 + _r*_r)/(1 + _r*_r));
    }else{
        //converted it to theta phi and add it to another sphere
        double _y = (double) y1 / (double)d;
        double _x = (double) x1 / (double)d;
        double _sinphi = _y / _r;
        double _cosphi = _x / _r;
        double _theta;
        _theta = (_r == 0.0) ? PI : 2.0 * atan(1.0 / _r);
        _theta = PI - _theta;
        double r1 = sin(_theta)/(1-cos(_theta));
        double _px = r1 * _cosphi;
        double _py = r1 * _sinphi;
        int kx = floor((_px + 1.0) * windowSize / 2);
        int ky = floor((_py + 1.0) * windowSize / 2);
        
        double _flux = 0;
        if(ky < 1) kx = 1;
        if(ky > (int)windowSize) ky = windowSize;
        if(kx < 0) kx = 0;
        if(kx > (int)windowSize - 1) kx = windowSize - 1;
        if(!isupshere){
            _flux = fluxmapU[(windowSize - ky) * windowSize + kx];
        }
        else{
            _flux = fluxmapL[(windowSize - ky) * windowSize + kx];
        }
        f11 = _flux;// / (4.0 / (1 + r1*r1)/(1 + r1*r1));
    }
    return f11;

}


void render::saveHealPix(){
    double * healmap;
    int nside = params -> NSIDE;
    //int pixss = windowSize * windowSize;
    int npix = nside * nside * 12;
    healmap = (double *) calloc(npix, sizeof(double));
    if(healmap == NULL){
        printf("Not enough memory, cannot convert to healpix map!\n");
        return;
    }
    //double domega = 4.0 * PI / (double) npix;
    //double detheta = sqrt(domega);
    double theta;
    double phi;
    
    double _rffmin = 1.0e36;
    double _rffmax = 0.0;
    double total_f = 0.0;

    
    for(int i = 0; i < npix; i++){
        //double x, y,r, factor;
        //int j;
        pix2ang_ring(nside, i, &theta, &phi);
        //now we have theta and phi and dtheta
        //converte (theta, phi, dtheta) to the projectio plane, we have
        phi = 2*PI - phi + PI;

        
        bool isupshere = false;
        if(theta < PI/2){
            theta = PI - theta;
            isupshere = true;
        }
        
        int d = round(windowSize / 2.0);
        //bilinear interpolation
        double pr = sin(theta)/(1-cos(theta));
        double pxc = pr * cos(phi);
        double pyc = pr * sin(phi);
        //double sintpr = sin(theta+detheta);
        //double sintmr = sin(theta-detheta);
        //double costpr = cos(theta+detheta);
        //double costmr = cos(theta-detheta);
        //double pxr = (sintmr/(1-costmr)-sintpr/(1-costpr))/2.0;

        
        double xc = (pxc) * (double)d;
        double yc = (pyc) * (double)d;
        double x1 = floor(xc);
        double x2 = x1 + 1;
        double y1 = floor(yc);
        double y2 = y1 + 1;
        //printf("reading %d\n", i);
        
        double f11 = _getpixflux(round(x1), round(y1), isupshere);
        double f12 = _getpixflux(round(x1), round(y2), isupshere);
        double f21 = _getpixflux(round(x2), round(y1), isupshere);
        double f22 = _getpixflux(round(x2), round(y2), isupshere);
        
        double flux = 0;
        double fr1 = (x2 - xc) / (x2 - x1) * f11 + (xc - x1) / (x2 - x1) * f21;
        double fr2 = (x2 - xc) / (x2 - x1) * f12 + (xc - x1) / (x2 - x1) * f22;
        flux = (y2 - yc) / (y2 - y1) * fr1 + (yc - y1) / (y2 - y1) * fr2;
        
        healmap[i] = flux / (4.0 / (1 + pr*pr)/(1 + pr*pr)) * windowSize * windowSize / 4.0;
        //printf("read ok %d\n", i);
        total_f += healmap[i];
        // * 
        //4 * PI * (windowSize * windowSize) / npix;// / (4.0 / (1 + pr*pr)/(1 + pr*pr));;
        // * params->FLUXFACTOR / 
        if(flux > _rffmax) _rffmax = flux;
        if(flux < _rffmin) _rffmin = flux;
        
    }
    float _ft = 0;
    for(int i = 0; i < npix; i++){
        healmap[i] = healmap[i] * params->FLUXFACTOR;// / total_f * totalFlux * params->FLUXFACTOR /domega;
        _ft += healmap[i];// / total_f * totalFlux * params->FLUXFACTOR /domega;;
    }
    
    printf("%f %f total: %f %f times %f %d\n", _rffmax, _rffmin, total_f * (4*PI/npix), _ft, total_f * (4*PI/npix) / totalFlux, npix/(2*windowSize*windowSize));
    cout.flush();
        
    ofstream output_file ((params->HEALPIXFILE).c_str(), ios::out | ios::binary);
    if(output_file.good()){
        output_file.write ((char *)healmap, 12 * nside * nside * sizeof(double));
    }else{
        cout << "Writing Error!";
    }
    output_file.close();
    free(healmap);
}

void render::saveFluxMap(){
    ofstream output_file ((params->OUTFILE).c_str(), ios::out | ios::binary);
    if(output_file.good()){
        int si = (int) (params -> WSIZE);
        output_file.write ((char *) &si, sizeof(int)); 
        output_file.write ((char *)fluxmapL, si * si * sizeof(Real));
        output_file.write ((char *)fluxmapU, si * si * sizeof(Real));
    }else{
        cout << "Writing Error!";
    }
    output_file.close();
}

