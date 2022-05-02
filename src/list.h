#pragma once

typedef struct List_s *List;
typedef struct ListNode_s *ListNode;

typedef void *ListInfo;

typedef int ListGetIdentifier(ListInfo info);
typedef void ListDestroyInfo(ListInfo info);

#define for_each_list(name, list) \
    for (ListNode name = list_get_head(list); name != NULL; name = list_get_next(name))

List list_create(ListGetIdentifier get_identifier, ListDestroyInfo destroy_info);

ListNode list_insert_head(List list, const ListInfo info);

ListNode list_insert_tail(List list, const ListInfo info);

ListNode list_insert_before(List list, ListInfo info, ListNode node);

ListNode list_insert_after(List list, ListInfo info, ListNode node);

ListInfo list_remove(List list, ListNode node);

ListNode list_search(List list, int id);

void list_swap_info(ListNode no1, ListNode no2);

int list_get_size(List list);

ListNode list_get_head(List list);

ListNode list_get_tail(List list);

ListInfo list_get_info(ListNode node);

ListNode list_get_next(ListNode node);

ListNode list_get_previous(ListNode node);

void list_destroy(List list);
