/*PLAN
Struct the hold a pointer to the next node
hold a void pointer that will hold the item

functions to return the head, if it is empty,
grab the next, and move to the next (return pointer to new 'head')


Referenced: 
* https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/linux/list.h
* https://www.kernel.org/doc/html/latest/core-api/kernel-api.html

*/

/*
initialize list head
list add
*/

/*
{.jump=case_foo, .name="foo"}
JT(foo)
#define JT(x) {.jump=case_##x, .name=#x}
static LIST_HEAD(foo);

struct bar{
strust list_head baz;
};

fun(){
INIT_LIST_HEAD(uuuu->lh);
}
*/

/*
iterates through each item in the list except the head
@head: head of the list
@pos: current position
*/
#define LIST_FOR_EACH(pos, head) \
        for(pos = (head)->next; pos != head; pos = (pos)->next)

struct list_node {
    struct list_node *next;
    struct list_node *prev;
};

/*
initializes circular linked list with head as the only node
*/
static void list_init_head(struct list_node *head){
    head->next = head;
    head->prev = head;
    return;
}

/*
adds new after head
*/
static void list_add(struct list_node *head, struct list_node *new){
    head->next->prev = new;
    new->next = head->next;
    head->next = new;
    new->prev = head;
    return;
}

/* I NEED TO FIX THIS EMPTY CONDITONS, COULDN'T FIGURE OUT REFERENCE CODE*/
static int list_empty(struct list_node *head){
    if (head->next == head)
        return 1;
    return 0;
}


/*
deletes node
@head: node to be deleted
*/
static void list_del(struct list_node *head){
    if(!list_empty(head)){
        head->prev->next = head->next;
        head->next->prev = head->prev;
    } else {

    }
    // MEMORY STILL NEEDS TO BE FREED
    /*
    if (!list_empty(head)){
        struct list_node *tmp = head->next;
        if (tmp) {
            head->next = tmp->next;
            head->next->prev = head;
            tmp->next = NULL;
            return tmp;
        }
        return NULL;
    }
    */
}

static struct list_node *list_next(struct list_node *head){
    return head->next;
}

/*
returns the number of nodes in the list
@head: head of the list
*/
static int list_count(struct list_node *head){
    if (list_empty(head))
        return 0;
    int total = 1;
    struct list_node *pos;
    LIST_FOR_EACH(pos, head){
        total++;
    }
    return total;
}