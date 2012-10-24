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
#include <stdlib.h>
#include <string.h> //for memcpy and memset

#include "circlist.h"
#include "particle.h"

circ_lst_t* new_circ_lst() {
    circ_lst_t* new_lst = malloc(sizeof(circ_lst_t));
    if(new_lst == NULL) { return NULL; }
    
    new_lst->head = NULL;
    new_lst->empties = NULL;
    
    return new_lst;
}

int add_circ_lst(circ_lst_t* lst, particle_t* p) {
    if(lst == NULL) { return -1; }
    
    if(lst->head == NULL) {
        //first added particle
        
        lst->head = malloc(sizeof(node_t));
        lst->head->next = lst->head;
        lst->head->prev = lst->head;
    }
    else {
        if(lst->empties != NULL) {
            //reuse the first already malloc'd node
            node_t* old_next = lst->empties->next;
            old_next->prev = lst->empties->prev;
            
            lst->empties->next = lst->head;
            lst->empties->prev = lst->head->prev;
            lst->head->prev = lst->empties;
            lst->empties = old_next;
        }
        else {
            //malloc a new node
            
            node_t* new_node = malloc(sizeof(node_t));
            node_t* old_last = lst->head->prev;
            old_last->next = new_node;
            new_node->prev = old_last;
            new_node->next = lst->head;
        }
    }
    
    //store the new data in it
    memcpy(&lst->head->p, p, sizeof(particle_t));
    
    return 0;
}

int del_circ_list(circ_lst_t* lst, node_t* to_rm) {
    if(to_rm == NULL || lst == NULL) { return -1; }
    
    //pave over this node in the list of active particles
    to_rm->prev->next = to_rm->next;
    to_rm->next->prev = to_rm->prev;
    
    //clear the particle data
    memset(&to_rm->p, 0, sizeof(particle_t));
    
    //put this node at the end of the empties list
    
    //check if the empties list has been initialized yet
    if(lst->empties == NULL) {
        lst->empties = to_rm;
        lst->empties->next = lst->empties;
        lst->empties->prev = lst->empties;
    }
    else {
        node_t* old_last = lst->empties->prev;
        to_rm->prev = old_last;
        to_rm->next = lst->empties;
        old_last->next = to_rm;
    }
    
    return 0;
}

int clear_circ_list(circ_lst_t* lst) {
    if(lst == NULL || lst->head == NULL) { return -1; }

    node_t* cur_node = lst->head;
    while(cur_node->next != NULL) {
        cur_node = cur_node->next;
        free(cur_node->prev);
    }
    
    if(lst->empties == NULL) { return 0; } //no empty nodes to delete
    cur_node = lst->empties;
    while(cur_node->next != NULL) {
        cur_node = cur_node->next;
        free(cur_node->prev);
    }
    
    free(lst);
    
    return 0;
}
