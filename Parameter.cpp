#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "Parameter.h"

using namespace std;

Parameter::Parameter(string conf){
    
    is_ready = false;
    CPU_MEM = 0;
    FLUXFACTOR = 0;
    DATAFILE = "";
    oposx = 0;
    oposy = 0;
    oposz = 0;
    vposx = 0;
    vposy = 0;
    vposz = 0;
    PSIZE = 256;
    WSIZE = 512;
    TEST = -1;
    OUTFILE = "";
    HEALPIXFILE = "";
    PICFILE = "";
   
    isUseNormMap = false;
    isOnScreenRend = false;
    isTimeMeasure = false;
    verbose = false;
    isROTM = false;
    conffile = conf;

	#if defined(_WIN32) || defined(_WIN64)
        SHADERDIR = "";
	    triangleVertexShader = SHADERDIR + "shaderfiles\\flux.gvert";
        triangleGeometryShader = SHADERDIR + "shaderfiles\\flux.tgeom";
        triangleFragmentShader = SHADERDIR + "shaderfiles\\flux.tfrag";

		pointVertexShader = SHADERDIR + "shaderfiles\\flux.gvert";
		pointGeometryShader = SHADERDIR + "shaderfiles\\flux.pgeom";
		pointFragmentShader = SHADERDIR + "shaderfiles\\flux.pfrag";
    
        spriteVertexShader = SHADERDIR + "shaderfiles\\flux.vert";;
        spriteFragmentShader = SHADERDIR + "shaderfiles\\flux.frag";;
        colorvertexshader = SHADERDIR + "shaderfiles\\color.vert";;
        colorfragmentshader = SHADERDIR + "shaderfiles\\color.frag";;
	#else
	    SHADERDIR = "./";
	    triangleVertexShader = SHADERDIR + "shaderfiles/flux.gvert";
        triangleGeometryShader = SHADERDIR + "shaderfiles/flux.tgeom";
        triangleFragmentShader = SHADERDIR + "shaderfiles/flux.tfrag";

        pointVertexShader = SHADERDIR + "shaderfiles/flux.gvert";
        pointGeometryShader = SHADERDIR + "shaderfiles/flux.pgeom";
        pointFragmentShader = SHADERDIR + "shaderfiles/flux.pfrag";

        spriteVertexShader = SHADERDIR + "shaderfiles/flux.vert";;
        spriteFragmentShader = SHADERDIR + "shaderfiles/flux.frag";;
        colorvertexshader = SHADERDIR + "shaderfiles/color.vert";;
        colorfragmentshader = SHADERDIR + "shaderfiles/color.frag";;
	#endif
}

bool Parameter::readParameter(){
    ifstream conf;
    string line;
    conf.open(conffile.c_str());
    if(!conf.good()){
        printf("Configuration file not found!\n");
        exit(1);
    }
    while(conf.good()){
        stringstream line_buf;
        string word;
        getline(conf, line);
        line_buf << line;
        line_buf >> word;
        if(word[0] != '#'){
            if(word == "CPU_MEM"){
                line_buf >> CPU_MEM;
                cout << "CPU_MEM " << CPU_MEM << endl;
            }else if(word == "WSIZE"){
                line_buf >> WSIZE;
                cout << "WSIZE " << WSIZE << endl;
            }else if(word == "PSIZE"){
                line_buf >> PSIZE;
                cout << "PSIZE " << PSIZE << endl;
            }else if(word == "FLUXFACTOR"){
                line_buf >> FLUXFACTOR;
                cout << "FLUXFACTOR " << FLUXFACTOR << endl;
            }else if(word == "DATAFILE"){
                line_buf >> DATAFILE;
                cout << "DATAFILE " << DATAFILE << endl;
            }else if(word == "USENORMMAP"){
                    //line_buf >> DATAFILE;
                isUseNormMap = true;
                cout << "Use norm map? " << isUseNormMap << endl;
            }else if(word == "OPOS"){
                line_buf >> oposx;
                line_buf >> oposy;
                line_buf >> oposz;
                opos[0] = oposx;
                opos[1] = oposy;
                opos[2] = oposz;
                printf("OPOS: %f, %f, %f\n", oposx, oposy, oposz);
            }else if(word == "CPOS"){
                line_buf >> cposx;
                line_buf >> cposy;
                line_buf >> cposz;
                cpos[0] = cposx;
                cpos[1] = cposy;
                cpos[2] = cposz;
                printf("CPOS: %f, %f, %f\n", cposx, cposy, cposz);
            }else if(word == "VVEC"){
                line_buf >> vposx;
                line_buf >> vposy;
                line_buf >> vposz;
                vvec[0] = vposx;
                vvec[1] = vposy;
                vvec[2] = vposz;
                printf("VPOS: %f, %f, %f\n", vposx, vposy, vposz);
            }else if(word == "TEST"){
                line_buf >> TEST;
                if(TEST != -1){
                    cout << "Test with " << TEST << " particles" << endl; 
                }
            }else if(word == "OUTFILE"){
                line_buf >> OUTFILE;
                cout << "OUTFILE " << OUTFILE << endl; 
            }else if(word == "HEALPIXFILE"){
                line_buf >> HEALPIXFILE;
                cout << "HEALPIXFILE " << HEALPIXFILE << endl; 
            }else if(word == "NSIDE"){
                line_buf >> NSIDE;
                cout << "NSIDE " << NSIDE << endl; 
            }else if(word == "PICFILE"){
                line_buf >> PICFILE;
                cout << "PICFILE " << PICFILE << endl; 
            }else if(word == "TVERTEXSHADER"){
                line_buf >> triangleVertexShader;
                cout << "Triangle Vertex Shader " << triangleVertexShader << endl; 
            }else if(word == "TGEOMETRYSHADER"){
                line_buf >> triangleGeometryShader;
                cout << "Triangle Geometry Shader " << triangleGeometryShader << endl; 
            }else if(word == "TFRAGMENTSHADER"){
                line_buf >> triangleFragmentShader;
                cout << "Triangle Fragment Shader " << triangleFragmentShader << endl; 
            }else if(word == "PVERTEXSHADER"){
                line_buf >> pointVertexShader;
                cout << "Point Vertex Shader " << pointVertexShader << endl; 
            }else if(word == "PGEOMETRYSHADER"){
                line_buf >> pointGeometryShader;
                cout << "Point Geometry Shader " << pointGeometryShader << endl; 
            }else if(word == "PFRAGMENTSHADER"){
                line_buf >> pointFragmentShader;
                cout << "Point Fragment Shader " << pointFragmentShader << endl; 
            }else if(word == "COLORVERTEXSHADER"){
                line_buf >> colorvertexshader;
                cout << "Sprite Color Vertex Shader " << colorvertexshader << endl;
            }else if(word == "COLORFRAGMENTSHADER"){
                line_buf >> colorfragmentshader;
                cout << "Sprite Color Fragment Shader " << colorfragmentshader << endl;
            }else if(word == "FRAGMENTSHADER"){
                line_buf >> spriteFragmentShader;
                cout << "Sprite Fragment Shader " << spriteFragmentShader << endl;
            }else if(word == "VERTEXSHADER"){
                line_buf >> spriteVertexShader;
                cout << "Sprite Fragment Shader " << spriteVertexShader << endl;
            }else if(word == "TIMEMEASURE"){
                isTimeMeasure = true;
                cout << "Measure render time?" << isTimeMeasure << endl;
            }else if(word == "ROTATE"){
                isROTM = true;
                cout << "Use rotation matrix?" << isROTM << endl;
            }else if(word == "ONSCREEN"){
                isOnScreenRend = true;
                cout << "Show picture on screen?" << isOnScreenRend << endl;
            }
            else if(word == "VERBOSE"){
                verbose = true;
                cout << "Verbose: " << verbose << endl;
            }
        }
        //cout << word << " -- " << line << endl;
    }
    conf.close();
    return 0;
}
