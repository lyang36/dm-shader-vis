CC=g++
#MACOS
LFLAGS= -lGL -lGLU -lglut 
IFLAG= -I /usr/include/GL/ -I /home/lyang/soft/glew/include/ 
#GLEWO=/usr/lib/libGLEW.a 
GLEWO= /home/lyang/soft/glew/lib/libGLEW.a

all: obj
	$(CC) *.o $(GLEWO) -o dmvis $(LFLAGS)
	@echo success

obj:
	$(CC) -c main.cpp buffers.cpp pix2ang_ring.cpp shaders.cpp Parameter.cpp\
                   render.cpp ang2pix_ring.cpp shaderManager.cpp datareader.cpp $(IFLAG)  

clean:
	rm -f *.o
	rm -f dmvis
