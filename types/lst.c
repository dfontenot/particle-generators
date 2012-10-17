/*
 *      Redistribution and use in source and binary forms, with or without
 *      modification, are permitted provided that the following conditions are
 *      met:
 *      
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following disclaimer
 *        in the documentation and/or other materials provided with the
 *        distribution.
 *      * Neither the name of the  nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *      
 *      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Author: David Fontenot
 * Date: September, 2012
 */

#include <stdlib.h> //for NULL

#include "lst.h"

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

int contains_lst(lst* list, void* item, int(*cmp)(void*,void*)) {
    int i;
    
    for(i = 0; i < list->sz; i++) {
        if(cmp(item, list->elems[i])) {
            return 1;
        }
    }
    
    return 0;
}
