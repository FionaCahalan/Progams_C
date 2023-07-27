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

#define LIST_FOR_EACH(head) \
        for(list_node *curr = head->next; head != curr; curr = curr->next)

struct list_node {
    list_node *next;
    list_node *prev;
}

static void list_init_head(struct list_node *head){
    head->next = head;
    return;
}

static void list_add(struct list_node *head, struct list_node *new){
    new->next = head->next;
    head->next = new;
    return;
}

static struct list_node *list_remove_next(struct list_node *head){
    struct list_node *tmp = head->next;
    if (tmp) {
        head->next = tmp->next;
        tmp->next = NULL;
        return tmp;
    }
    return NULL;
}

static struct list_node *list_next(struct list_node *head){
    return head->next;
}

static int list_empty(struct list_node *head){
    if (head->next == head)
        return 1;
    return 0;
}

static int count(struct list_node *head){
    int total = 1;
    LIST_FOR_EACH(head){
        total++;
    }
    return total;
}