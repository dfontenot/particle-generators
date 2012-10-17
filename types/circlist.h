//      Redistribution and use in source and binary forms, with or without
//      modification, are permitted provided that the following conditions are
//      met:
//      
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following disclaimer
//        in the documentation and/or other materials provided with the
//        distribution.
//      * Neither the name of the  nor the names of its
//        contributors may be used to endorse or promote products derived from
//        this software without specific prior written permission.
//      
//      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//Author: David Fontenot
//Date: Sept, 2012

/*
 * This circularly doubly linked list deletes nodes by moving them to the end, 
 * in order to cut down on lots of new node mallocs.
 */

#ifndef CIRC_LIST_H
#define CIRC_LIST_H

#include "particle.h"

struct node_t {
    particle_t p;
    struct node_t* next;
    struct node_t* prev;
};
typedef struct node_t node_t;

//stores two circularly linked lists in one
struct circ_lst_t {
    node_t* head; //the sentinel for the active particles
    node_t* empties; //the sentinel for the empty particles
};
typedef struct circ_lst_t circ_lst_t;

circ_lst_t* new_circ_lst();

int add_circ_lst(circ_lst_t* lst, particle_t* p);

int del_circ_list(circ_lst_t* lst, node_t* to_rm);

int clear_circ_list(circ_lst_t* lst);

#endif
