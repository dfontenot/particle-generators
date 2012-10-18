#include <math.h>

#include "SDL.h"

#include "types/particle.h"
#include "types/lst.h"

#define SCR_W 500
#define SCR_H 500
#define SCR_BPP 32
#define LIST_START_SIZE 30

//how big the width is of each starting particle
#define PARTICLE_START_SIZE 20

//how many pixels a second the particle decreases at
#define SIZE_DECREASE 5

//how big the width of a particle can before it gets culled
#define PARTICLE_SIZE_THRESHOLD 2

#define MAX_NEG_X -100 //maximum negative speed in the x direction
#define MAX_X 100
#define MAX_NEG_Y -100
#define MAX_Y 100

inline void free_particles(lst* particles) {
    particle_t* p;
    int i;
    
    for(i = 0; i < particles->sz; i++) {
        p = (particle_t*)get_lst(particles, i);
        if(p == NULL) {
            continue;
        }
        
        free(p);
    }
    
    free_lst(particles);
}
 
inline int flr(double d) {
    return (int)floor(d);
}

//http://stackoverflow.com/a/5673518/854854
inline double rand_range(int low, int high) {
    return rand() % 2 == 0 ? (low + rand() % (high + 1)) + (rand() / rand()) : -(low + rand() % (high + 1)) + (rand() / rand());
}
 
inline void draw_pixel(SDL_Surface* surf, Uint32 color, int x, int y) {
    //source from: http://www.libsdl.org/intro.en/usingvideo.html
    Uint32* b;
    b = (Uint32*)surf->pixels + y * surf->pitch / 4 + x;
    *b = color;
}

void draw_particle(SDL_Surface* screen, particle_t* p, Uint32 color) {
    int x, y;
    
    for(x = flr(p->x_apparent); x < flr(p->x_apparent) + flr(p->size); x++) {
        for(y = flr(p->y_apparent); y < flr(p->y_apparent) + flr(p->size); y++) {
            draw_pixel(screen, color, x, y);
        }
    }
}
 
void move_particles(lst* particles, double elapsed) {
    int i;
    particle_t* cur_particle;
    
    for(i = 0; i < particles->sz; i++) {
        cur_particle = (particle_t*)get_lst(particles, i);
        if(cur_particle == NULL) {
            continue; //null particles represent culled particles
        }
        
        //cull particle if it gets too small
        if(cur_particle->size <= PARTICLE_SIZE_THRESHOLD) {
            set_lst(particles, i, NULL);
            continue;
        }
        
        cur_particle->x_apparent += elapsed * cur_particle->x_speed;
        cur_particle->y_apparent += elapsed * cur_particle->y_speed;
        cur_particle->size -= elapsed * SIZE_DECREASE;
        
        //see if the particle left the screen
        if(flr(cur_particle->x_apparent) + flr(cur_particle->size) >= SCR_W || flr(cur_particle->x_apparent) <= 0 || 
        flr(cur_particle->y_apparent) + flr(cur_particle->size) >= SCR_H || flr(cur_particle->y_apparent)<= 0) {
            set_lst(particles, i, NULL);
        }
        else {
            set_lst(particles, i, (void*)cur_particle);
        }
    }
}

//add a new particle every time the draw function is called
int draw(SDL_Surface* screen, lst* particles, Uint32 color, int* open_space) {
    int i;
    particle_t* cur_particle;
    
    cur_particle = new_particle(SCR_W / 2, SCR_H / 2, rand_range(MAX_NEG_X, MAX_X), rand_range(MAX_NEG_Y, MAX_Y), PARTICLE_START_SIZE);
    if(cur_particle == NULL) {
        return -1;
    }
    
    if(*open_space != -1) {
        set_lst(particles, *open_space, (void*)cur_particle);
        *open_space = -1; //reset
    }
    else {
        push_lst(particles, (void*)cur_particle);
    }
    
    for(i = 0; i < particles->sz; i++) {
        cur_particle = (particle_t*)get_lst(particles, i);
        if(cur_particle != NULL) {
            draw_particle(screen, cur_particle, color);
        }
        else {
            *open_space = i;
        }
    }
}

//inspired by: http://vimeo.com/36278748
int main(int argc, char** argv) {
    SDL_Surface* screen;
    SDL_Init(SDL_INIT_EVERYTHING);
    Uint32 color;
    Uint32 start;
    Uint32 end;
    double elapsed;
    SDL_Event e;
    
    lst* particles;
    particle_t* cur_particle;
    int open_space = -1; //-1 means no open space, otherwise where a particle can be written over a culled one
    int i;
    
    if(argc >= 2) {
        srand(time(atoi(argv[1])));
    }
    else {
        srand(time(NULL));
    }
    
    //PUT IN FIRST PARTICLES
    particles = new_lst(LIST_START_SIZE);
    if(particles == NULL) {
        fprintf(stderr, "error: `particles array could be initialized'\n");
        return 1;
    }
    
    screen = SDL_SetVideoMode(SCR_W, SCR_H, SCR_BPP, SDL_DOUBLEBUF | SDL_SWSURFACE);
    if(screen == NULL) {
        fprintf(stderr, "error: `%s'\n", SDL_GetError());
        return 1;
    }
    
    
    
    color = SDL_MapRGB(screen->format, 0, 0, 255);
    //ANIMATION LOOP
    while(1) {
        start = SDL_GetTicks();
        
        if(SDL_MUSTLOCK(screen)) {
            if(SDL_LockSurface(screen) < 0) {
                fprintf(stderr, "could unlock screen\nerror: `%s'\n", SDL_GetError());
                return 1;
            }
        }
        
        //DRAW PARTICLES
        draw(screen, particles, color, &open_space);
        
        if(SDL_MUSTLOCK(screen)) {
            SDL_UnlockSurface(screen);
        }
        
        SDL_Flip(screen);
        SDL_FillRect(screen, NULL, 0);
        
        //PROCESS EVENTS
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                SDL_FreeSurface(screen);
                free_particles(particles);
                return 0;
            }
        }
        
        //CALCULATE NEXT FRAME
        end = SDL_GetTicks();
        elapsed = (end - start) / 1000.0;
        
        move_particles(particles, elapsed);
    }
}
