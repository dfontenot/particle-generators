#include "../types/circlist.h"
#include "../types/particle.h"

#include<stdio.h>
#include<stdlib.h>

int singleton_add(circ_lst_t* lst, particle_t* part) {           /* add one particle to a list */
    if(lst == NULL) {
        fprintf(stderr, "Error in %s: list is null\n", __FUNCTION__);
        return -1;
    }

    if(add_circ_lst(lst, part) < 0) { /* was the item added? */
        fprintf(stderr, "Error in %s: could not add particle to list\n", __FUNCTION__);
        return -1;
    }

    if(memcmp(&(lst->head->p), part, sizeof(particle_t)) != 0) {  /* see if the particle was actually inserted */
        fprintf(stderr, "Error in %s: incorrect particle data copied over\n", __FUNCTION__);
        return -1;
    }

    if(lst->head->next != lst->head) { /* see if the list wraps around */
        fprintf(stderr, "Error in %s: extra items in singleton list\n", __FUNCTION__);
        return -1;
    }

    if(clear_circ_list(lst) < 0) { /* clear the list */
        fprintf(stderr, "Error in %s: could not clear lst\n", __FUNCTION__);
        return -1;
    }

    return 0;
}

int main() {
    circ_lst_t* lst;            /* common variables that will be re-used to pass to tests */
    particle_t* particle;

    int failed = 0;             /* how many tests were failed */
    int total_tests = 0;        /* how many tests were there */

    lst = new_circ_lst(); /* variables for the first test */
    particle = new_particle(1.0,1.0,1.0,1.0,1.0);

    ++total_tests;
    printf("Test: singleton_add\n");
    if(singleton_add(lst, particle) < 0)
        failed++;

    free(lst);                  /* cleanup after first test */
    free(particle);

    printf("Total tests: %d. Failed tests: %d. Pass rate: %1.2lf %%\n", 
           total_tests, failed, (double)(100* ((total_tests - failed) / total_tests)));

    return 0;
}
