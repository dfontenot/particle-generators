#include "SDL.h"

#include <math.h>

#include "types/particle.h"
#include "types/circlist.h"

#define SCR_W 500
#define SCR_H 500
#define SCR_BPP 32
#define PARTICLE_START_SIZE 100
#define SIZE_DECREASE 0.99
#define DEFAULT_EMIT_RATE 1
#define PARTICLE_SIZE_THRESHOLD 2
#define MAX_NEG_X -5
#define MAX_X 5
#define MAX_NEG_Y -5
#define MAX_Y 5

inline int flr(double d) {
    return (int)floor(d);
}

//http://stackoverflow.com/a/5673518/854854
inline double rand_range(int low, int high) {
    return rand() % 2 == 0 ? (low + rand() % (high + 1)) + (rand() / rand()) : -(low + rand() % (high + 1)) + (rand() / rand());
}
 
int draw_pixel(SDL_Surface* surf, Uint32 color, int x, int y) {
    //source from: http://www.libsdl.org/intro.en/usingvideo.html
    if(x > surf->clip_rect.w || y > surf->clip_rect.h) { return -1; }
    
    Uint32* b;
    b = (Uint32*)surf->pixels + y * surf->pitch / 4 + x;
    *b = color;
}

//drawing a cirlce like: x^2 + y^2 = rect_height / 2
int draw_particle(SDL_Surface* screen, particle_t* p, Uint32 color) {
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
    
    return 0;
}
 
/*void move_particles(lst* particles, double elapsed) {
    int i;
    particle_t* cur_particle;
    
    for(i = 0; i < particles->sz; i++) {
        cur_particle = (particle_t*)get_lst(particles, i);
        if(cur_particle == NULL) {
            continue; //null particles represent culled particles
        }
        
        //cull particle if it gets too small
        if(cur_particle->size * SIZE_DECREASE <= PARTICLE_SIZE_THRESHOLD) {
            set_lst(particles, i, NULL);
            continue;
        }
        
        cur_particle->x_apparent += cur_particle->x_speed;
        cur_particle->y_apparent += cur_particle->y_speed;
        cur_particle->size *= SIZE_DECREASE;
        
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
}*/

/*//add a new particle every time the draw function is called
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
}*/

//returns the next node
node_t* update_particle(circ_lst_t* lst, node_t* cur_particle, Uint32 end, Uint32 start) {
    node_t* next = cur_particle->next;
    double (end - start) / 1000.0;
    
    //cull particle if it gets too small
    if(cur_particle->size * SIZE_DECREASE <= PARTICLE_SIZE_THRESHOLD) {
        del_circ_list(lst, cur_particle);
        return next;
    }
    
    cur_particle->x_apparent += cur_particle->x_speed;
    cur_particle->y_apparent += cur_particle->y_speed;
    cur_particle->size *= SIZE_DECREASE;
    
    cur_particle->rect->x = flr(cur_particle->x_apparent);
    cur_particle->rect->y = flr(cur_particle->y_apparent);
    cur_particle->rect->w = flr(cur_particle->size);
    cur_particle->rect->h = flr(cur_particle->size);
    
    //see if the particle left the screen
    if(cur_particle->rect->x + cur_particle->rect->w >= SCR_W || cur_particle->rect->x <= 0 || 
    cur_particle->rect->y + cur_particle->rect->h >= SCR_H || cur_particle->rect->y <= 0) {
        del_circ_list(lst, cur_particle);
        return next;
    }
    
    return next;
}

//inspired by: http://vimeo.com/36278748
int main(int argc, char** argv) {
    SDL_Surface* screen;
    SDL_Init(SDL_INIT_EVERYTHING);
    Uint32 start;
    
    //event processing related
    SDL_Event e;
    int drawing = 0;
    int mouse_x = -1;
    int mouse_y = -1;
    
    circ_lst_t* particles;
    
    if(argc >= 2) {
        srand(time(atoi(argv[1])));
    }
    else {
        srand(time(NULL));
    }

    particles = new_circ_lst();
    if(particles == NULL) {
        fprintf(stderr, "error: `particles array could be initialized'\n");
        return 1;
    }
    
    screen = SDL_SetVideoMode(SCR_W, SCR_H, SCR_BPP, SDL_DOUBLEBUF | SDL_SWSURFACE);
    if(screen == NULL) {
        fprintf(stderr, "error: `%s'\n", SDL_GetError());
        return 1;
    }

    Uint32 p_color = SDL_MapRGB(screen->format, 0, 0, 255);
    node_t* cur_particle = NULL;
    int emit_rate = DEFAULT_EMIT_RATE;
    int emit_counter = 0;

    //ANIMATION LOOP
    while(1) {
        start = SDL_GetTicks();
        
        if(SDL_MUSTLOCK(screen)) {
            if(SDL_LockSurface(screen) < 0) {
                fprintf(stderr, "could unlock screen\nerror: `%s'\n", SDL_GetError());
                return 1;
            }
        }
        
        //ADD A NEW PARTICLE IF DRAWING
        if(drawing == 1 && emit_rate == emit_counter) {
            if(add_circ_lst(particles, 
                new_particle(mouse_x, mouse_y, 
                rand_range(MAX_NEG_X, MAX_X), rand_range(MAX_NEG_Y, MAX_Y), 
            PARTICLE_START_SIZE)) < 0) {
                fprintf(stderr, "error in creating new particle\n");
                return 1;
            }
        }
        
        //DRAW THE PARTICLES
        //draw the particles even when drawing == 0 because there 
        //could still be particles on the screen that haven't shrunk to 0 yet
        cur_particle = particles->head;
        if(cur_particle != NULL) {
            //draw first particle
            
            draw_particle(screen, cur_particle->p, p_color);
            cur_particle = cur_particle->next;
            
            cur_particle = update_particle(particles, cur_particle, SDL_GetTicks(), start);

            //draw the rest
            while(cur_particle != NULL && cur_particle != particles->head) {
                draw_particle(screen, cur_particle->p, p_color);
                
                cur_particle = update_particle(particles, cur_particle, SDL_GetTicks(), start);
            }
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
                clear_circ_list(particles);
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
                        emit_counter = 1; //reset emit counter
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

        emit_counter++;
        if(emit_counter > emit_rate) { emit_counter = 1; }
    }
}
