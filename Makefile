CC=cc
CFLAGS=`sdl-config --cflags`
LIBS=-lm `sdl-config --libs`

all:
	basic
	circles
	faster_circles

lst.o:
	$(CC) -c types/lst.c

particle.o:
	$(CC) -c types/particle.c `sdl-config --cflags`

circlst.o:
	$(CC) -c types/circlst.c
       
basic: basic.c lst.o particle.o types/lst.h types/particle.h
	$(CC) $(CFLAGS) lst.o particle.o basic.c -o basic $(LIBS)
	rm lst.o particle.o

circles: circles.c lst.o particle.o types/lst.h types/particle.h
	$(CC) $(CFLAGS) lst.o particle.o circles.c -o circles $(LIBS)
	rm lst.o particle.o

faster_circles: circles.c circlst.o particles.o types/circlst.h types/particle.h
	$(CC) $(CFLAGS) circlst.o particle.o circles.c -o circles $(LIBS)
	rm circlst.o particle.o

clean:
	rm -f basic circles faster_circles
