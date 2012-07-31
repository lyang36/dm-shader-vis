CC=g++
#MACOS
LFLAGS= -lGL -lglut
IFLAG= -I /usr/include/GL/ 
GLEWO=/usr/lib/libGLEW.a 

all: obj
	$(CC) *.o $(GLEWO) -o dmvis $(LFLAGS)
	@echo success

obj:
	$(CC) -c main.cpp buffers.cpp pix2ang_ring.cpp shaders.cpp Parameter.cpp\
                   render.cpp ang2pix_ring.cpp shaderManager.cpp datareader.cpp $(IFLAG)  

clean:
	rm -f *.o
	rm -f dmvis
