#include <stdlib.h>

#include "particle.h"

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
