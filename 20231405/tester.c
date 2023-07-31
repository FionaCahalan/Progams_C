#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "linked_list.h"

struct list_item{
    struct list_node super;
    int my_int;
};

int main(int argc, char *argv[]){
    (void)argv;
    (void)argc;
    
    struct list_item *my_list = calloc(sizeof *my_list, 1);

    list_init_head(&my_list->super);
    
    struct list_item *curr = my_list;

    printf("total: %d\n", list_count(&my_list->super));

    for(int i = 0; i < 10; i++){
        //list_add(&curr->super, &(calloc(sizeof *curr, 1)->super));
        list_add(&curr->super, (struct list_node *) calloc(sizeof *curr, 1));
        curr = (struct list_item *) list_next(&curr->super);
        curr->my_int = i;
    }

    struct list_node *position;
    LIST_FOR_EACH(position, &my_list->super){
        printf("#: %d\n", ((struct list_item *)position)->my_int);
    }

    printf("total: %d\n", list_count(&my_list->super));

    list_del(list_next(&my_list->super));
    list_del(list_next(&my_list->super));
   
    LIST_FOR_EACH(position, &my_list->super){
        printf("#: %d\n", ((struct list_item *)position)->my_int);
    }

    printf("total: %d\n", list_count(&my_list->super));
}