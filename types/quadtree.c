#include "quadtree.h"

quad_tree* init_quad_tree(SDL_Rect field, int ppq) {
    quad_tree* t = NULL;
    t = (quad_tree*)malloc(sizeof(quad_tree));
    if(t == NULL) { return NULL; }
    
    t->field = field;
    t->ppq = ppq;
    
    return t;
}

quad_tree* init_quad_tree(SDL_Rect field) {
    return init_quad_tree(field, DEFAULT_PPQ);
}

int insert(quad_tree* q, particle_t* point) {
    quad_branch* cur_branch = q->trunk;
    SDL_Rect cur_dimension = q->field;
    
    //loop through the branches until the right one is found
    while(cur_branch->points == NULL) {
        if(point->x
    }
}
