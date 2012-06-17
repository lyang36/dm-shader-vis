CC=g++
#MACOS
LFLAGS=-framework Carbon -framework OpenGL -framework GLUT
IFLAG= -I /usr/include/GL/ 
GLEWO=/usr/lib/libGLEW.a 

all: obj
	$(CC) *.o $(GLEWO) -o dmvis $(LFLAGS)
	@echo success

obj:
	$(CC) -c *.cpp $(IFLAG)  

clean:
	rm -f *.o
	rm -f dmvis
