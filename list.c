#include "list.h"

devs_list* new_devs_list() {
    devs_list* list = (devs_list*)malloc(sizeof(devs_list));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

bool is_empty(devs_list* list) {
    return list->head == NULL;
}

const devs_list* push_back(void* data, devs_list* list) {
    devs_node* node = (devs_node*)malloc(sizeof(devs_node));
    node->data = data;
    node->prev = NULL;
    node->next = NULL;
    if(is_empty(list)) {
        list->head = list->tail = node;
    }
    else {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    } 
}

