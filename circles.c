#include "SDL.h"

#include <math.h>

#include "types/particle.h"
#include "types/lst.h"

#define SCR_W 500
#define SCR_H 500
#define SCR_BPP 32
#define LIST_START_SIZE 30

//how big the width is of each starting particle
#define PARTICLE_START_SIZE 100

//how many pixels a second the particle decreases at
#define SIZE_DECREASE 30

//how big the width of a particle can before it gets culled
#define PARTICLE_SIZE_THRESHOLD 2

#define MAX_NEG_X -50 //maximum negative speed in the x direction
#define MAX_X 50
#define MAX_NEG_Y -50
#define MAX_Y 50
 
inline void free_particles(lst* particles) {
    particle_t* p;
    int i;
    
    for(i = 0; i < particles->sz; i++) {
        p = (particle_t*)get_lst(particles, i);
        if(p == NULL) {
            continue;
        }
        
        free(p->rect);
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
 
void draw_pixel(SDL_Surface* surf, Uint32 color, int x, int y) {
    //source from: http://www.libsdl.org/intro.en/usingvideo.html
    if(x > surf->clip_rect.w || y > surf->clip_rect.h) { return; }
    
    Uint32* b;
    b = (Uint32*)surf->pixels + y * surf->pitch / 4 + x;
    *b = color;
}

//drawing a cirlce like: x^2 + y^2 = rect_height / 2
void draw_particle(SDL_Surface* screen, particle_t* p, Uint32 color) {
    //x and y represnt translated positions for a surface with (0, 0) in the middle
    //they are translated back when given to draw_pixel()
    int x, y;
    
    for(x = -1 * flr(p->rect->w / 2); x < flr(p->rect->w / 2); x++) {
        for(y = flr(p->rect->h / 2); y > -1 * flr(p->rect->h / 2); y--) {
            if((x * x) + (y * y) <= flr(p->rect->h / 2)) {
                draw_pixel(screen, color, x + flr(p->rect->w / 2) + p->rect->x, y + flr(p->rect->h / 2) + p->rect->y);
            }
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
        
        //multiply speed (pixels / second) by how much time has elapsed
        cur_particle->x_apparent += elapsed * cur_particle->x_speed;
        cur_particle->y_apparent += elapsed * cur_particle->y_speed;
        cur_particle->size -= elapsed * SIZE_DECREASE;
        
        cur_particle->rect->x = flr(cur_particle->x_apparent);
        cur_particle->rect->y = flr(cur_particle->y_apparent);
        cur_particle->rect->w = flr(cur_particle->size);
        cur_particle->rect->h = flr(cur_particle->size);
        
        //see if the particle left the screen
        if(cur_particle->rect->x + cur_particle->rect->w >= SCR_W || cur_particle->rect->x <= 0 || 
        cur_particle->rect->y + cur_particle->rect->h >= SCR_H || cur_particle->rect->y <= 0) {
            set_lst(particles, i, NULL);
        }
        else {
            set_lst(particles, i, (void*)cur_particle);
        }
    }
}

//add a new particle every time the draw function is called
int draw(SDL_Surface* screen, lst* particles, Uint32 color, int drawing, int mouse_x, int mouse_y, int* open_space) {
    int i;
    particle_t* cur_particle;
    
    if(drawing == 0 && particles->sz == 0) { return 0; }
    
    if(drawing == 1) {
        cur_particle = new_particle(mouse_x, mouse_y, rand_range(MAX_NEG_X, MAX_X), rand_range(MAX_NEG_Y, MAX_Y), PARTICLE_START_SIZE);
        if(cur_particle == NULL) {
            return -1;
        }
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
    
    return 0;
}

//inspired by: http://vimeo.com/36278748
int main(int argc, char** argv) {
    SDL_Surface* screen;
    SDL_Init(SDL_INIT_EVERYTHING);
    Uint32 color;
    Uint32 start;
    Uint32 end;
    double elapsed;
    
    //event processing related
    SDL_Event e;
    int drawing = 0;
    int mouse_x = -1;
    int mouse_y = -1;
    
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
        if(draw(screen, particles, color, drawing, mouse_x, mouse_y, &open_space) < 0) {
            fprintf(stderr, "error in draw()\n");
            return 1;
        }
        
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
            else if(e.type == SDL_MOUSEMOTION) {
                mouse_x = e.motion.x;
                mouse_y = e.motion.y;
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN) {
                if(e.button.button == SDL_BUTTON_LEFT) {
                    if(e.button.state == SDL_PRESSED) {
                        printf("setting the button to be pressed\n");
                        drawing = 1;
                    }
                }
            }
            else if(e.type == SDL_MOUSEBUTTONUP) {
                if(e.button.button == SDL_BUTTON_LEFT) {
                    if(e.button.state == SDL_RELEASED) {
                        printf("setting the button to be released\n");
                        drawing = 0;
                    }
                }
            }
        }
        
        //CALCULATE NEXT FRAME
        end = SDL_GetTicks();
        elapsed = (end - start) / 1000.0;
        
        move_particles(particles, elapsed);
    }
}
