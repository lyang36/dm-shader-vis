#ifndef __LYANG_HEALPIX__
#define __LYANG_HEALPIX__
void pix2ang_ring( long nside, long ipix, double *theta, double *phi);
void ang2pix_ring( const int nside, double theta, double phi, int *ipix);
#endif
