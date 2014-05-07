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
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //for memcpy and memset

#include "circlist.h"
#include "particle.h"

int count_lst = 0;
int count_empties = 0;

circ_lst_t* new_circ_lst() {
    circ_lst_t* new_lst = malloc(sizeof(circ_lst_t));
    if(new_lst == NULL) 
        return NULL;
    
    new_lst->head = NULL;
    new_lst->empties = NULL;
    
    return new_lst;
}

int add_circ_lst(circ_lst_t* lst, particle_t* p) {
    if(lst == NULL) 
        return -1;
    
    if(lst->head == NULL) {
        //first added particle
        
        lst->head = malloc(sizeof(node_t));
        printf("ALLOCATING first PARTICLE %p\n", lst->head);
        lst->head->next = lst->head;
        lst->head->prev = lst->head;
        count_lst++;
    }
    else {
        /*
         * empties is null when there are no 
         * recycled nodes to claim
         */
        if(lst->empties != NULL) {
            //reclaim an item from empties and move it over to the list
            printf("reusing old particle\n");
            printf("number in list (being printed): %d, number in free list: %d\n", count_lst, count_empties);
            /* node_t* cur = lst->head->next; */
            /* printf("list item: %p\n", lst->head); */

            /* while(cur != lst->head) { */
            /*     printf("list item: %p\n", lst->head); */
            /*     cur = cur->next; */
            /* } */

            printf("no loop detected in lst\n");

            //check if last entry to remove
            if(lst->empties->next == lst->empties) {
                printf("last entry in empties\n");
                //make the head point to NULL to signal all the empties have been claimed

                lst->empties->next = lst->head;
                lst->empties->prev = lst->head->prev;
                lst->head->prev = lst->empties;

                lst->head = lst->empties;
                lst->empties = NULL; //set empties to null
            }
            else {
                //take an entry off the head of the list

                printf("at least 2 entries in empties\n");

                node_t* old_empties = lst->empties; //will be removing the head from the empties list

                lst->empties->prev->next = lst->empties->next; //pave over element
                lst->empties->next->prev = lst->empties->prev;
                lst->empties = lst->empties->next; //start empties list at next element

                old_empties->prev = lst->head->prev;
                old_empties->next = lst->head;
                lst->head->prev = old_empties;

                lst->head = old_empties; //head now points to this re-used node
            }
            
            count_lst++;
            count_empties--;
        }
        else {
            //allocate a new node at the head
            
            node_t* new_node = malloc(sizeof(node_t));
            printf("ALLOCATING PARTICLE %p\n", new_node);
            new_node->prev = lst->head->prev;
            new_node->next = lst->head;
            lst->head->prev->next = new_node;
            lst->head->prev = new_node;

            lst->head = new_node;

            count_lst++;
        }
    }
    
    //store the new data in it
    memcpy(&lst->head->p, p, sizeof(particle_t));
    
    return 0;
}

int del_circ_list(circ_lst_t* lst, node_t* to_rm) {
    if(to_rm == NULL || lst == NULL) 
        return -1;
    
    //pave over this node in the list of active particles
    to_rm->prev->next = to_rm->next;
    to_rm->next->prev = to_rm->prev;
    
    //clear the particle data
    memset(&to_rm->p, 0, sizeof(particle_t));
    
    //put this node at the start of the empties list
    
    //check if the empties list has been initialized yet
    if(lst->empties == NULL) {
        lst->empties = to_rm;
        lst->empties->next = lst->empties;
        lst->empties->prev = lst->empties;
    }
    else {
        //prepend to the head
        to_rm->next = lst->empties;
        to_rm->prev = lst->empties->prev;
        lst->empties->prev->next = to_rm;
        lst->empties->prev = to_rm;

        lst->empties = to_rm;
    }

    count_lst--;
    count_empties++;
    
    return 0;
}

int clear_circ_list(circ_lst_t* lst) {
    if(lst == NULL || lst->head == NULL) 
        return -1;

    node_t* cur_node = lst->head->next;
    node_t* tmp;
    printf("FREEING HEAD %p\n", cur_node->prev);
    free(cur_node->prev); //free the head
    while(cur_node != lst->head) {
        cur_node = tmp;
        cur_node = cur_node->next;
        printf("FREEING PARTICLE %p\n", tmp);
        free(tmp->prev);
    }

    printf("PARTICLES COMPLETE\n");
    
    if(lst->empties == NULL) 
        return 0;

    cur_node = lst->empties->next;
    printf("FREEING HEAD OF EMPTIES %p", cur_node->prev);
    free(cur_node->prev);
    while(cur_node != NULL) {
        cur_node = tmp;
        cur_node = cur_node->next;
        printf("FREEING PARTICLE %p\n", tmp);
        free(tmp->prev);
    }

    return 0;
}
