#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./utils.h"

struct ListNode_s {
    ListInfo info;
    struct ListNode_s *next;
    struct ListNode_s *previous;
};

struct List_s {
    ListGetIdentifier *get_identifier;
    ListDestroyInfo *destroy_info;
    int size;
    struct ListNode_s *head;
    struct ListNode_s *tail;
};

List list_create(ListGetIdentifier get_identifier, ListDestroyInfo destroy_info) {
    List list = malloc(sizeof *list);
    if (!list) {
        return NULL;
    }
    list->get_identifier = get_identifier;
    list->destroy_info = destroy_info;
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

ListNode list_insert_head(List list, const ListInfo info) {
    if (!info)
        return NULL;
    ListNode new_node = malloc(sizeof *new_node);
    if (!new_node)
        return NULL;

    new_node->info = info;
    new_node->previous = NULL;

    if (!list->head) {
        list->head = new_node;
        new_node->next = NULL;
    } else {
        list->head->previous = new_node;
        new_node->next = list->head;
    }

    list->head = new_node;
    list->size++;
    return new_node;
}

ListNode list_insert_tail(List list, const ListInfo info) {
    if (!info)
        return NULL;
    ListNode new_node = malloc(sizeof *new_node);
    if (!new_node)
        return NULL;
    new_node->info = info;
    new_node->next = NULL;

    if (!list->head) {
        list->head = new_node;
        new_node->previous = NULL;
    } else {
        list->tail->next = new_node;
        new_node->previous = list->tail;
    }

    list->tail = new_node;
    list->size++;
    return new_node;
}

ListNode list_insert_before(List list, ListInfo info, ListNode node) {
    if (!node || !info)
        return NULL;
    ListNode previous_node = NULL;
    ListNode new_node = malloc(sizeof *new_node);
    if (new_node)
        return NULL;

    new_node->info = info;
    if (node == list->head) {  // caso seja inserido antes do head
        node->previous = new_node;
        new_node->next = node;
        new_node->previous = NULL;

        list->head = new_node;
    } else {
        previous_node = node->previous;
        node->previous = new_node;
        previous_node->next = new_node;

        new_node->next = node;
        new_node->previous = previous_node;
    }
    list->size++;

    return new_node;
}

ListNode list_insert_after(List list, ListInfo info, ListNode node) {
    if (!node | !info)
        return NULL;

    ListNode no_next = NULL;
    ListNode new_node = malloc(sizeof *new_node);
    if (!new_node)
        return NULL;

    new_node->info = info;

    if (node == list->tail) {  // Caso seja inserido depois do último.
        node->next = new_node;
        list->tail = new_node;

        new_node->previous = node;
        new_node->next = NULL;
    } else {
        no_next = node->next;
        node->next = new_node;
        no_next->previous = new_node;

        new_node->next = no_next;
        new_node->previous = node;
    }
    list->size++;

    return new_node;
}

ListInfo list_remove(List list, ListNode node) {
    if (!node)
        return NULL;
    ListNode previous_node = node->previous;
    ListNode no_next = node->next;

    if (node == list->head) {  // Se for o head elemento da list.
        list->head = no_next;
        // Caso a list só tenha um elemento não é necessário mudar o ponteiro do próximo elemento.
        if (no_next != NULL)
            no_next->previous = NULL;
    } else if (node == list->tail) {  // Se for o último elemento da list.
        list->tail = previous_node;
        previous_node->next = NULL;
    } else {  // Se estiver node meio da list.
        previous_node->next = node->next;
        no_next->previous = node->previous;
    }

    ListInfo info = node->info;
    free(node);
    list->size--;
    return info;
}

ListNode list_search(List list, int id) {
    if (!list->get_identifier)
        return NULL;

    for_each_list(node, list) {
        int current_id = list->get_identifier(node->info);
        if (current_id == id)
            return node;
    }
    return NULL;
}

// Troca as informações armazenadas em dois nós.
void list_swap_info(ListNode no1, ListNode no2) {
    ListInfo temp = list_get_info(no1);
    no1->info = list_get_info(no2);
    no2->info = temp;
}

int list_get_size(List list) {
    return list->size;
}

ListNode list_get_head(List list) {
    return list->head;
}

ListNode list_get_tail(List list) {
    return list->tail;
}

ListInfo list_get_info(ListNode node) {
    return node->info;
}

ListNode list_get_next(ListNode node) {
    return node->next;
}

ListNode list_get_previous(ListNode node) {
    return node->previous;
}

void list_destroy(List list) {
    ListNode current_node = list->head;
    while (current_node != NULL) {
        ListNode no_next = current_node->next;
        if (list->destroy_info != NULL)
            list->destroy_info(current_node->info);
        free(current_node);
        current_node = no_next;
    }
    free(list);
}
