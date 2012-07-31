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
#include "types.h"
#include "Parameter.h"
#include "datareader.h"
#include "render.h"


//#include "Skymap.h"

using namespace std;

int main(int argc, char** argv){
    Parameter params;
    params.readParameter();
    
    DataReader reader;

    reader.setTest(params.TEST);
    reader.setPath(params.DATAFILE);
    reader.setBuf(params.CPU_MEM);
	printf("ok1\n");    
//#ifdef _WIN32 || _WIN64		   
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    glewInit();
    glewExperimental = GL_TRUE; 
#endif
	printf("ok2\n"); 
//	printf("ok\n");
    render Render;
    Render.reader = &reader;
    Render.params = &params;
//  	printf("ok\n");
    Render.start(argc, argv);
    return 0;    
}
