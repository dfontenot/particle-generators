CC=cc
CFLAGS=-g3 `sdl-config --cflags`
LIBS=-lm `sdl-config --libs`

all:
	basic
	circles
	faster_circles

lst.o: types/lst.h
	$(CC) -c types/lst.c

particle.o: types/particle.h
	$(CC) -c types/particle.c

circlist.o: types/circlist.h
	$(CC) -c types/circlist.c $(CFLAGS)
       
basic: basic.c lst.o particle.o
	$(CC) $(CFLAGS) lst.o particle.o basic.c -o basic $(LIBS)
	rm lst.o particle.o

circles: circles.c lst.o particle.o
	$(CC) $(CFLAGS) lst.o particle.o circles.c -o circles $(LIBS)
	rm lst.o particle.o

faster_circles: circlist.o particle.o
	$(CC) $(CFLAGS) circlist.o particle.o faster_circles.c -o faster_circles $(LIBS)
	rm circlist.o particle.o

clean:
	rm -f basic circles faster_circles
