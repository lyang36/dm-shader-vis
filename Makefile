#MACOS

CC=g++

FITSIOLIB=/Users/lyang/Tools/cfitsio
HEALPIXHOME=/Users/lyang/Tools/Healpix_3.00
HEALPIXSRC = $(HEALPIXHOME)/src/cxx/basic_gcc/include
HEALPIXLIB = $(HEALPIXHOME)/src/cxx/basic_gcc/lib
HEALPIXCXX = $(HEALPIXHOME)/src/cxx/cxxsupport
HEALPIXC  = $(HEALPIXHOME)/cxx/Healpix_cxx

LFLAGS=-framework Carbon -framework OpenGL -framework GLUT \
	   $(HEALPIXLIB)/libhealpix_cxx.a\
	   $(HEALPIXLIB)/libcxxsupport.a\
	   $(FITSIOLIB)/libcfitsio.a

IFLAG= -I /usr/include/GL/ -I $(HEALPIXSRC) -I$(FITSIOLIB)
GLEWO=/usr/lib/libGLEW.a

all: obj
	$(CC) *.o $(GLEWO) -o dmvis $(LFLAGS)
	@echo success

obj:
	$(CC) -c -Wall main.cpp buffers.cpp pix2ang_ring.cpp shaders.cpp Parameter.cpp\
		   render.cpp ang2pix_ring.cpp shaderManager.cpp datareader.cpp	$(IFLAG)  

clean:
	rm -f *.o
	rm -f dmvis
