#include "SDL.h"
#include <math.h>

#define SCR_W 500
#define SCR_H 500
#define SCR_BPP 32
#define LIST_START_SIZE 30
#define PARTICLE_START_SIZE 20
#define SIZE_DECREASE 0.99
#define PARTICLE_SIZE_THRESHOLD 2
#define MAX_NEG_X -2
#define MAX_X 2
#define MAX_NEG_Y -2
#define MAX_Y 2

typedef struct {
    double x_apparent;
    double y_apparent;
    double x_speed;
    double y_speed;
    double size;
}particle_t;

typedef struct {
    int sz;
    int cap;
    void** elems;
}lst;

particle_t* new_particle(double x_apparent, double y_apparent, double x_speed, double y_speed, double size) {
    particle_t* retval = NULL;
    retval = (particle_t*)malloc(sizeof(particle_t));
    if(retval == NULL) { return NULL; }
    
    retval->x_apparent = x_apparent;
    retval->y_apparent = y_apparent;
    retval->x_speed  = x_speed;
    retval->y_speed = y_speed;
    retval->size = size;
    
    return retval;
}

/*
 * LIST PROCESSESING CODE
 */
lst* new_lst(int size) {
    if(size <= 0) { return NULL; }
    
    lst* list = (lst*)malloc(sizeof(lst));
    if(list == NULL) { return NULL; }
    
    list->cap = size;
    list->sz = 0;
    if((list->elems = malloc(size * sizeof(void*))) == NULL) {
        return NULL;
    }
    else {
        return list;
    }
}

//loc is zero-based
void* get_lst(lst* list, int loc) {
    if(loc >= list->cap || loc < 0) { return NULL; }
    else { return list->elems[loc]; }
}

//loc is zero-based
int set_lst(lst* list, int loc, void* val) {
    if(loc >= list->cap || loc < 0) { return -1; }
    
    list->elems[loc] = val;
    
    return 0;
}

//doubles the list (adds another layer for the heap)
int expand_lst(lst* list) {
    int i;
    void** new_lst = (void**)malloc(list->cap * 2 * sizeof(void*));
    
    if(new_lst == NULL) { return -1; }
    
    for(i = 0; i < list->cap * 2; i++) {
        if(i <= list->sz - 1) {
            new_lst[i] = get_lst(list, i);
        }
        else {
            new_lst[i] = 0;
        }
    }
    
    free(list->elems);
    list->elems = new_lst;
    list->cap *= 2;
    
    return 0;
}

//loc is zero-indexed
int ins_lst(lst* list, void* item, int loc) {
    int i;
    
    if(loc < 0 || item == NULL) { return -1; }
    
    //resize
    if(list->sz == list->cap) {
        expand_lst(list);
    }
    
    //check to see if adding to the end
    //(no need to move things around)
    if(loc == list->sz) {
        list->elems[list->sz] = item;
    }
    else {
        for(i = list->sz - 1; i >= loc; i--) {
            list->elems[i + 1] = list->elems[i];
        }
        
        set_lst(list, loc, item);
    }
    
    list->sz++;
    return 0;
}

//add item to the end
int push_lst(lst* list, void* item) {
    return ins_lst(list, item, list->sz);
}

//loc is zero indexed
int del_lst(lst* list, int loc) {
    int i;
    
    if(list == NULL || loc < 0 || list->sz <= 0) { return -1; }
    
    //see if removing from the end
    if(loc == list->sz - 1) {
        list->elems[list->sz - 1] = NULL;
    }
    else {
        for(i = loc; i < list->sz; i++) {
            list->elems[i] = list->elems[i + 1];
        }
    }
    
    list->sz--;
    return 0;
}

int pop_lst(lst* list) {
    return del_lst(list, list->sz - 1);
}

int clear_lst(lst* list) {
    while(list->sz > 0) {
        if(pop_lst(list) < 0) { return -1; }
    }
    
    return 0;
}

int swap_lst(lst* list, int loc1, int loc2) {
    if(loc1 < 0 || loc2 < 0 || loc1 >= list->sz || loc2 >= list->sz) {
        return -1;
    }
     
    void* item1 = list->elems[loc1];
    void* item2 = list->elems[loc2];
    
    list->elems[loc2] = item1;
    list->elems[loc1] = item2;
    
    return 0;
}

int free_lst(lst* list) {
    free(list->elems);
    free(list);
    
    return 0;
}

int cleanup_nulls(lst* list) {
    int i, j;
    int elem_count = 0; //non-null values
    int new_cap = 0;
    void** new_elems;
    
    for(i = 0; i < list->sz; i++) {
        if(list->elems[i] != NULL) { elem_count++; }
    }
    
    new_cap = elem_count + ((1/3) * list->sz);
    new_elems = (void**)malloc(new_cap * sizeof(void*));
    if(new_elems == NULL) { return -1; }
    
    for(i = 0, j = 0; i < list->sz; i++) {
        if(list->elems[i] != NULL) {
            new_elems[j] = list->elems[i];
            j++;
        }
    }
    
    free(list->elems);
    list->elems = new_elems;
    list->sz = elem_count;
    list->cap = new_cap;
    return 0;
}

int print_lst(lst* list, char* (*print_fn)(void*)) {
    int i;
    
    if(list == NULL) { printf("%p\n", list); return -1; }
    
    printf("[");
    for(i = 0; i < list->sz; i++) {
        printf("%s", print_fn(list->elems[i]));
        
        if(i < list->sz - 1) {
            printf(", ");
        }
    }
    printf("]\n");
    
    return 0;
}

/*
 * END
 */
 
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
        if(cur_particle->size * SIZE_DECREASE <= PARTICLE_SIZE_THRESHOLD) {
            set_lst(particles, i, NULL);
            continue;
        }
        
        cur_particle->x_apparent += cur_particle->x_speed;
        cur_particle->y_apparent += cur_particle->y_speed;
        cur_particle->size *= SIZE_DECREASE;
        
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
