#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct node{
    int returnT;
    int assignT;
    int ogT;
    int books;
    int electronics;
    char *customers;
    struct node *next;
    char worker[];
};

struct worker{
    struct worker *next;
    char name[];
};

static struct worker *workers_head;
static struct worker *workers_tail;
static struct node *assigns_head;
static struct node *orders_head;
static struct node *orders_tail;
static int assigns_size;
static int workers_size;
static int orders_size;

static void worker_add_last(char *name){
    size_t length = strlen(name);
    struct worker *worker = malloc(sizeof *worker + length + 1);
    worker->next = NULL;
    memcpy(worker->name, name, length + 1);
    if (!workers_tail){
        workers_tail = worker;
        workers_head = worker;
    } else {
        workers_tail->next = worker;
        workers_tail = worker;
    }
    workers_size++;
}

static struct worker *worker_remove_first(void){
    struct worker *temp = workers_head;
    workers_head = workers_head->next;
    if(workers_size == 1)
        workers_tail = NULL;
    workers_size--;
    return temp;
}

static struct node *assigns_remove_first(void){
    struct node *temp = assigns_head;
    assigns_head = assigns_head->next;
    assigns_size--;
    return temp;
}

static struct node *orders_remove_first(void){
    struct node *temp = orders_head;
    orders_head = orders_head->next;
    if(orders_size == 1)
        orders_tail = NULL;
    orders_size--;
    return temp;
}

/*
struct worker *make_worker(struct worker *next, char *name){
    size_t length = strlen(name);
    struct worker *worker = malloc(sizeof *worker + length + 1);
    worker->next = next;
    memcpy(worker->name, name, length + 1);
    return worker;
}
*/

static int add_time(int og, int add){
    int num = og % 100 + add;
    return og / 100 * 100 + num / 60 * 100 + num % 60;
}

static int sub_time (int og, int sub){
    if(og % 100 < sub % 100)
        return (og / 100 - 1) * 100 - sub / 100 * 100 + 60 + og % 100 - sub % 100;
    else
        return og - sub;
}

static void start_work(struct node *worker){
    struct node *finger = assigns_head;
    if(!assigns_size || finger->returnT > worker->returnT){
        worker->next = finger;
        assigns_head = worker;
        assigns_size++;
    } else{
        while(finger->next && finger->next->returnT < worker->returnT){
            finger = finger->next;
        }
        worker->next = finger->next;
        finger->next = worker;
        assigns_size++;
    }
}

static struct node *wait_time_check(struct node *floater, int time){
    if(add_time(floater->assignT, 5) < time){
        floater->returnT = add_time(floater->returnT, 5);
        floater->assignT = add_time(floater->assignT, 5);
        start_work(floater);
        floater = NULL;
    }
    return floater;
}

static int completion_check(int time, int max){
    if(assigns_size > 0 && assigns_head->returnT <= time){
        struct node *removed = assigns_remove_first();

        if(sub_time(removed->returnT, removed->ogT) > max)
            max = sub_time(removed->returnT, removed->ogT);
        
        char *temp = strtok(removed->customers, ",");
        do{
            printf("OrderCompletion %04d %s\n", removed->returnT, temp);
        }while ((temp = strtok(NULL, ",")));
        worker_add_last(removed->worker);
        free(removed->customers);
        free(removed);
    }
    return max;
}

static struct node *check_floater(struct node *floater){
    struct node *order = orders_head;

    if(!wait_time_check(floater, order->ogT))
        return NULL;
    else if((order->books > 0 && floater->electronics >0) || (order->electronics > 0 && floater->books > 0) ||
                floater->books + order->books > 10 || order->electronics + floater->electronics > 10){
        floater->returnT = add_time(floater->returnT, sub_time(order->ogT, floater->assignT));
        floater->assignT = add_time(floater->assignT, sub_time(order->ogT, floater->assignT));
        start_work(floater);
        return NULL;
    } else{
        floater->books += order->books;
        floater->electronics += order->electronics;
        floater->returnT = add_time(floater->returnT, order->books + order->electronics + sub_time(order->ogT, floater->assignT));
        floater->assignT = add_time(floater->assignT, sub_time(order->ogT, floater->assignT));
        
        char *temp;
        asprintf(&temp, "%s,%s", floater->customers, order->customers);
        free(floater->customers);
        floater->customers = temp;
        printf("WorkerAssignment %04d %s %s\n", floater->assignT, floater->worker, order->customers);
        orders_remove_first();
        free(order->customers);
        free(order);

        if(floater->books == 10 || floater-> electronics == 10){
            start_work(floater);
            return NULL;
        } else
            return floater;
    }
}

static struct node *assign_work(int time){
    struct node *assignment = orders_remove_first();
    struct worker *worker = worker_remove_first();
    assignment = realloc(assignment, sizeof *assignment + strlen(worker->name) + 1);
    strcpy(assignment->worker, worker->name);
    free(worker);
    assignment->assignT = time;
    assignment->returnT = add_time(time, assignment->books + assignment->electronics + 10 + (assignment->books > 0 && assignment->electronics > 0 ? 5 : 0));
    printf("WorkerAssignment %04d %s %s\n", assignment->assignT, assignment->worker, assignment->customers);

    if((assignment->books > 0 && assignment->electronics > 0) || assignment->books == 10 || assignment->electronics == 10){
        start_work(assignment);
        return NULL;
    } else
        return assignment;
}

static struct node *orders_to_workers(struct node *floater, int time){
    while(orders_size > 0 && (workers_size > 0 || floater)){
        if(floater)
            floater = check_floater(floater);
        else if(workers_size > 0){
            floater = assign_work(time);
        }
    }
    return floater;
}

static void print_available_workers(void){
    struct worker *finger = workers_head;

    if(finger){
        while(finger->next){
            printf(" %s", finger->name);
            finger = finger->next;
        }
        printf(" %s", finger->name);
    }
    printf("\n");
}

static void print_worker_assignments(struct node *floater){
    struct node *finger = assigns_head;

    if(floater){
        if(!finger || finger->returnT > floater->returnT)
            printf(" %s:%s", floater->worker, floater->customers);
        else{
            while(finger->next && finger->next->returnT > floater->returnT){
                printf(" %s:%s", finger->worker, finger->customers);
                finger = finger->next;
            }
            printf(" %s:%s", finger->worker, finger->customers);
            printf(" %s:%s", floater->worker, floater->customers);
            finger = finger->next;
        }
    }

    if(finger){
        while(finger->next){
            printf(" %s:%s", finger->worker, finger->customers);
            finger = finger->next;
        }
        printf(" %s:%s", finger->worker, finger->customers);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    (void)argc;
    worker_add_last("Alice");
    worker_add_last("Bob");
    worker_add_last("Carol");
    worker_add_last("David");
    worker_add_last("Emily");

    struct node *floater = NULL;
    int max_time = 0;
    int time = 0;

    freopen(argv[1], "r", stdin);
    char buf[1000];
    while(fgets(buf, sizeof buf, stdin)){
        char *temp = strchr(buf, '\n');
        if(temp)
            temp[0] = '\0';
        temp = strchr(buf, '\r');
        if(temp)
            temp[0] = '\0';

        char input[1000];
        int time;
        char customer_name[100];
        int books;
        int electronics;
        sscanf(buf, "%s %d %s %d %d", input, &time, customer_name, &books, &electronics);

        if(floater)
            floater = wait_time_check(floater, time);
// printf("%d %p %d\n", assigns_size, assigns_head, time);
        while(assigns_size > 0 && assigns_head->returnT <= time){
            int finish = assigns_head->returnT;
            max_time = completion_check(time, max_time);
            floater = orders_to_workers(floater, finish);

            if(floater)
                floater = wait_time_check(floater, finish);
        }
        if(!strcmp(input, "CustomerOrder")){
            struct node *order = malloc(sizeof *order);
            order->ogT = time;
            order->books = books;
            order->electronics = electronics;
            order->customers = strdup(customer_name);
            order->next = NULL;
            if (!orders_tail){
                orders_tail = order;
                orders_head = order;
            } else {
                orders_tail->next = order;
                orders_tail = order;
            }
            orders_size++;
            printf("CustomerOrder %04d %s %d %d\n", order->ogT, order->customers, order->books, order->electronics);
        } else if(!strcmp(input, "PrintAvailableWorkerList")){
            printf("AvailableWorkerList %04d", time);
            print_available_workers();
        } else if(!strcmp(input, "PrintWorkerAssignmentList")){
            printf("WorkerAssignmentList %04d", time);
            print_worker_assignments(floater);
        } else if(!strcmp(input, "PrintMaxFulfillmentTime")){
            printf("MaxFulfillmentTime %04d %d\n", time, max_time);
        }

        floater = orders_to_workers(floater, time);
    }

    while(orders_size > 0){
        if(floater)
            floater = check_floater(floater);
        else if (workers_size > 0)
            floater = assign_work(time);
        else {
            time = assigns_head->returnT;
            completion_check(time, max_time);
        }
    }

    if(floater)
        floater = wait_time_check(floater, add_time(floater->assignT, 6));
    
    while(assigns_size){
        completion_check(assigns_head->returnT, max_time);
    }
}