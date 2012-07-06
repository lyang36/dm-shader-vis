#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Parameter.h"

using namespace std;

Parameter::Parameter(){
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
}

bool Parameter::readParameter(){
    ifstream conf;
    string line;
    conf.open ("configure.ini");
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
            }
        }
        //cout << word << " -- " << line << endl;
    }
    conf.close();
    return 0;
}
