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
 
	render Render;
    Render.reader = &reader;
    Render.params = &params;
    Render.start(argc, argv);
    return 0;    
}
