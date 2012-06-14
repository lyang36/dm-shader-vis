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
            }else if(word == "OPOSX"){
                line_buf >> oposx;
                cout << "oposx " << oposx << endl;
            }else if(word == "OPOSY"){
                line_buf >> oposy;
                cout << "oposy " << oposy << endl;
            }else if(word == "OPOSZ"){
                line_buf >> oposz;
                cout << "oposz " << oposz << endl;
            }else if(word == "VPOSX"){
                line_buf >> vposx;
                cout << "vposx " << vposx << endl;
            }else if(word == "VPOSY"){
                line_buf >> vposy;
                cout << "vposy " << vposy << endl;
            }else if(word == "VPOSZ"){
                line_buf >> vposz;
                cout << "vposz " << vposz << endl;
            }else if(word == "TEST"){
                line_buf >> TEST;
                if(TEST != -1){
                    cout << "Test with " << TEST << " particles" << endl; 
                }
            }
        }
        //cout << word << " -- " << line << endl;
    }
    conf.close();
    return 0;
}
