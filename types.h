/*Define some types*/
/*Lin Yang (Forrest)*/

#ifndef __LY__TYPES__
#define __LY__TYPES__

#define REAL float
#define Real REAL

#define PI 3.141592653589793
//48 bytes
struct Particles{
    REAL mass;
    REAL density;
    REAL hsmooth;
    REAL xpos;
    REAL ypos;
    REAL zpos;
};

//a flux circle on the projection plane
//20 bytes
struct Circle{
    int radius;
    int xc;
    int yc;
    REAL flux;
};

struct Point{
    REAL pos[3];
    REAL color[4];
};
#endif
