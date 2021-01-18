#ifndef _LIST_H_
#define _LIST_H_

typedef struct st_devs_node {
    void* data;
    struct st_node* prev;
    struct st_node* next;
} devs_node;

typedef struct st_devs_list {
    devs_node* head;
    devs_node* tail;
} devs_list;

#endif
