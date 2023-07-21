/*PLAN
Struct the hold a pointer to the next node
hold a void pointer that will hold the item

functions to return the head, if it is empty,
grab the next, and move to the next (return pointer to new 'head')

make it circular 

Look up linux kernal linked list documentation but not implementation.
*/

/*
initialize list head
list add
*/

struct list_node {
    void *item;
    list_node *next;
    // maybe store identifier of head here?
}

static void list_init_head(struct list_node *head){
    head->next = head;
    return;
}

static void list_add_next(struct list_node *curr, struct list_node *next){
    next->next = curr->next;
    curr->next = curr;
    return;
}

static struct list_node *list_remove_next(struct list_node *curr){
    struct list_node *tmp = curr->next;
    if (tmp) {
        curr->next = tmp->next;
        tmp->next = NULL;
        return tmp;
    }
    return NULL;
}

static struct list_node *list_next(struct list_node *curr){
    return curr->next;
}
