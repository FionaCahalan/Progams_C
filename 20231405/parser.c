#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct node;

struct node_vtable{
	void (*print)(struct node *curr);
    int (*evaluate)(struct node *curr);
};

struct node{
	struct node_vtable *ops;
	int type;
    // 0 = number
	// 1 = function
};

struct opr{
	struct node super;
    struct node *left_c;
	struct node *right_c;
};

struct num{
	struct node super;
	int number;
};

static int mul(struct node *curr){
	struct opr *op = (struct opr *)curr;
    int a = op->left_c->ops->evaluate(op->left_c);
    int b = op->right_c->ops->evaluate(op->right_c);
    return a * b;
}

static int divi(struct node *curr){
	struct opr *op = (struct opr *)curr;
    int a = op->left_c->ops->evaluate(op->left_c);
    int b = op->right_c->ops->evaluate(op->right_c);
    return a / b;
}

static int add(struct node *curr){
	struct opr *op = (struct opr *)curr;
    int a = op->left_c->ops->evaluate(op->left_c);
    int b = op->right_c->ops->evaluate(op->right_c);
    return a + b;
}

static int sub(struct node *curr){
	struct opr *op = (struct opr *)curr;
    int a = op->left_c->ops->evaluate(op->left_c);
    int b = op->right_c->ops->evaluate(op->right_c);
    return a - b;
}

static int eval_num(struct node *curr){
    return ((struct num*)curr)->number;
}

static void print_mul(struct node *curr){
	printf("*");
}

static void print_div(struct node *curr){
	printf("/");
}

static void print_add(struct node *curr){
	printf("+");
}

static void print_sub(struct node *curr){
	printf("-");
}

static void print_num(struct node *curr){
    printf("%d", curr->ops->evaluate(curr));
}

static struct node_vtable multiply = {
    .print = print_mul,
    .evaluate = mul,
};

static struct node_vtable divide = {
    .print = print_div,
    .evaluate = divi,
};

static struct node_vtable addition = {
    .print = print_add,
    .evaluate = add,
};

static struct node_vtable subtract = {
    .print = print_sub,
    .evaluate = sub,
};

static struct node_vtable number_v = {
    .print = print_num,
    .evaluate = eval_num,
};


static int c_idx = 0;
// static char equation[] = "124*34+2*30*5";
// static char equation[] = "124*34-2";
// static char equation[] = "124*34+2*30-5";
static char equation[] = "124*34+2*30-5*100+66/11";
// static char equation[] = "124*34+2*30";
static struct node* symbol;

static char next_char(void){
    if (c_idx < 23)
        return equation[c_idx++];
    else return -1;
}

static char peak_char(void){
	if (c_idx < 23)
        return equation[c_idx];
    else return -1;
}

static void next_part(void){
	int ch = next_char();
	if(ch != -1 && ch >= '0' && ch <= '9'){
		int number = ch - '0';
		while(peak_char() != -1 && peak_char() >= '0' && peak_char() <= '9'){
			ch = next_char();
			number = number * 10 + (ch - '0');
		}
		struct num *tmp = calloc(sizeof *tmp, 1);
		tmp->number = number;
		tmp->super.type = 0;
        tmp->super.ops = &number_v;
		symbol = &(tmp->super);
	} else if (ch){
		struct opr *tmp = calloc(sizeof *tmp, 1);
		if(ch == '*'){
			tmp->super.ops = &multiply;
		} if(ch == '/') {
			tmp->super.ops = &divide;
		} if(ch == '+'){
			tmp->super.ops = &addition;
		} if(ch == '-') {
			tmp->super.ops = &subtract;
		}
		tmp->super.type = 1;
		symbol = &tmp->super;
	} else {
        symbol = NULL;
    }
}

static struct node *factor(void){
    if (symbol->type == 0) {
        struct node *temp = symbol;
        next_part();
        return temp;
    }
    return NULL;
}

static struct node *term(void){
    struct node *head = factor();
    while (peak_char() != -1 && symbol->type == 1 && (symbol->ops == &multiply || symbol->ops == &divide)){
        struct opr *temp = (struct opr *) symbol;
        temp->left_c = head;
        head = symbol;
        next_part();
        temp->right_c = factor();
    }
    return head;
}

static struct node *expression(void){
    struct node *head = term();
    while (peak_char() != -1 && symbol->type == 1 && (symbol->ops == &addition || symbol->ops == &subtract)){
        struct opr *temp = (struct opr *) symbol;
        temp->left_c = head;
        head = symbol;
        next_part();
        temp->right_c = term();
    }
    return head;
}

static void print_tree(struct node *curr){
    if (curr->type == 0){
        curr->ops->print(curr);
    } else {
        print_tree(((struct opr *)curr)->left_c);
        curr->ops->print(curr);
        print_tree(((struct opr *)curr)->right_c);
    }
}

static void print_tree1(struct node *curr, int level){
    printf("%-*s", level*3, "");
    if (curr->type == 0){
        curr->ops->print(curr);
        printf("\n");
    } else {
        curr->ops->print(curr);
        printf("\n");
        print_tree1(((struct opr *)curr)->left_c, level + 1);
        print_tree1(((struct opr *)curr)->right_c, level + 1);
    }
}

int main(int argc, char *argv[]){
	(void)argc;
	(void)argv;
	if(peak_char() != -1){ 
        next_part();
        struct node *head = expression();
        int answer = head->ops->evaluate(head);
        printf("%d\n", answer);
        print_tree(head);
        printf("\n");
        print_tree1(head, 0);
	}
}


/*
Expression
term {("+"|"-") term}

term
factor {("*"|"/") factor}

factor
number

add support for negative/positive numbers, spaces, and paranthesis
use switch statement instead of series of if statements

return \0 instead of EOF
*/