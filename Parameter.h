
/*Parameter.h*/
/*class: Parameter*/
/*Lin Yang (Forrest)*/
#include "types.h"
#include <string>

#ifndef __LY__PARAMETER__
#define __LY__PARAMETER__
//Parameters:
/*DATAFILE = data file path (string)
 *FLUXFACTOR = flux factor (real)
 *CPU_MEM = number of particles (int)
 *GPU_MEM = number of particles (int)
 *PIXES = number of pixels on the stereographics projection south sphere plane (int)
 *
 */

using namespace std;

class Parameter{
public:
    bool is_ready;
    int CPU_MEM;              //how many particles is in the CPU memory
    REAL FLUXFACTOR;
    
    //int NSIDE;                //pixels on one side
    REAL oposx, oposy, oposz; //standing position
    REAL opos[3];
    REAL vposx, vposy, vposz; //viewing vector
    REAL vvec[3];
    REAL cposx, cposy, cposz; //center position
    REAL cpos[3];
    
    
    REAL WSIZE, PSIZE;           //size of the projection plane (a side)
    string DATAFILE;          //Data file path
    int TEST;                 //how many particles are used to test
    string OUTFILE;           //output the data
    string HEALPIXFILE;       //convert healpix file
    string PICFILE;           //picture file save
    
    int NSIDE;                //NSIDE for healpix map
    

    //initialization
    Parameter();
    //readparameter from the configure.ini
    //if data is ready return is_ready
    bool readParameter();
};
#endif
