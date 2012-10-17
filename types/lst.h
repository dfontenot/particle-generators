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

#ifndef LST_H
#define LST_H

typedef struct {
    int sz;
    int cap;
    void** elems;
}lst;

lst* new_lst(int size);

//loc is zero-based
void* get_lst(lst* list, int loc);

//loc is zero-based
int set_lst(lst* list, int loc, void* val);

//doubles the list (adds another layer for the heap)
int expand_lst(lst* list);

//loc is zero-indexed
int ins_lst(lst* list, void* item, int loc);

//add item to the end
int push_lst(lst* list, void* item);

//loc is zero indexed
int del_lst(lst* list, int loc);

int pop_lst(lst* list);

int clear_lst(lst* list);

int swap_lst(lst* list, int loc1, int loc2);

int free_lst(lst* list);

int print_lst(lst* list, char* (*print_fn)(void*));

int contains_lst(lst* list, void* item, int(*cmp)(void*,void*));

#endif
