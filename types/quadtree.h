#ifndef QUADTREE_H
#define QUADTREE_H

#define DEFAULT_PPQ 15

#include "SDL.h"

struct quad_branch {
    struct quad_branch nw;
    struct quad_branch ne;
    struct quad_branch sw;
    struct quad_branch se;
    SDL_Rect* points; //set to NULL if this isn't a leaf
    int point_count;
};

typedef struct quad_branch quad_branch;

struct quad_tree {
    quad_branch trunk;
    //points per quad
    //how many points to put into a quad before it branches out
    int ppq;
    SDL_Rect field;
};

typedef struct quad_tree quad_tree;

quad_tree* init_quad_tree(SDL_Rect field);

quad_tree* init_quad_tree(SDL_Rect field, int ppq);

int insert(quad_tree* q, particle_t* point);

#endif
