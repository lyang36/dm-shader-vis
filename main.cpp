/* GPU rendering the darkmatter all skymap. Using stereographic projection */
/* Classes:
 * Parameter: read parameter from configure.ini. 
 * Including the units infomation, datafile path etc.
 * Kernel.cu: the GPU files
 * Skymap: dealwith all other stuffes
 *
*/
#include <string>
#include <iostream>
#include <vector>
#include <string>
#include "types.h"
#include "Parameter.h"
#include "datareader.h"
#include "render.h"


//#include "Skymap.h"

using namespace std;

void usage(){
    printf("--help          :  get help\n");
    printf("--conf conffile :  use conffile as the configuration\n" );
}

int main(int argc, char** argv){
    bool ishelp = false;
    string conffile;
    #if defined(_WIN32) || defined(_WIN64)
        conffile = ("config_win.ini");
    #else
        conffile = ("config_uni.ini");
    #endif
    for(int i = 1; i < argc; i++){
        string argstr;
        argstr = argv[i];
        if(argstr.find("--help", 0) != string::npos){
            ishelp = true;
        }else if(argstr.find("--conf", 0) != string::npos){
            if(i == argc - 1){
                usage();
                exit(0);
            }else{
                conffile = argv[i + 1];
                i++;
            }
        }else{
            usage();
            exit(0);
        }
    }

    if(ishelp){
        usage();
        exit(0);
    }
    
    printf("Use configuration file: %s\n", conffile.c_str());
    Parameter params(conffile);
    params.readParameter();
    
    DataReader reader;

    reader.setTest(params.TEST);
    reader.setPath(params.DATAFILE);
    reader.setBuf(params.CPU_MEM);
 
    render Render;
    Render.reader = &reader;
    Render.params = &params;
    Render.start(argc, argv);
    return 0;    
}
