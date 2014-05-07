CC=cc
CFLAGS=`sdl-config --cflags`
DEBUG=-g3
LIBS=-lm `sdl-config --libs`

all: basic circles faster_circles tests

lst.o: types/lst.h
	$(CC) -c types/lst.c $(DEBUG)

particle.o: types/particle.h
	$(CC) -c types/particle.c $(DEBUG)

circlist.o: types/circlist.h
	$(CC) -c types/circlist.c $(CFLAGS) $(DEBUG)

basic: basic.c lst.o particle.o
	$(CC) $(CFLAGS) lst.o particle.o basic.c -o basic $(LIBS) $(DEBUG)

circles: circles.c lst.o particle.o
	$(CC) $(CFLAGS) lst.o particle.o circles.c -o circles $(LIBS) $(DEBUG)

faster_circles: circlist.o particle.o
	$(CC) $(CFLAGS) circlist.o particle.o faster_circles.c -o faster_circles $(LIBS) $(DEBUG)

tests: circlist_test

circlist_test: circlist.o particle.o
	$(CC) $(CFLAGS) circlist.o particle.o tests/circlist_test.c -o circlist_test $(LIBS)

clean:
	rm -f basic circles faster_circles *.o
