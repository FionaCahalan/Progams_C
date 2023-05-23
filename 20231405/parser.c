#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct node;

struct node_vtable{
	void (*print)(void);
    int (*evaluate)(int, int);
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

static int mul(int a, int b){
	return a * b;
}

static int divi(int a, int b){
	return a / b;
}

static int add(int a, int b){
	return a + b;
}

static int sub(int a, int b){
	return a - b;
}

// This one is probably wrong, but I wasn't sure how to make it work with scope
static int eval_num(int a, int b){
    return a;
}

static void print_mul(void){
	printf("*");
}

static void print_div(void){
	printf("/");
}

static void print_add(void){
	printf("+");
}

static void print_sub(void){
	printf("-");
}

static void print_num(void){
    printf("NOT FINISHED NUMBER PRINT LOL");
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
static char equation[] = "124*34";
static struct node* symbol;

static char next_char(void){
	return equation[c_idx++];
}

static char peak_char(void){
	return equation[c_idx];
}

static struct node *next_part(void){
	int ch = next_char();
	// could probably remove if statement
	if(ch && ch >= '0' && ch <= '9'){
		int number = ch - '0';
		while(peak_char() && peak_char() >= '0' && peak_char() <= '9'){
			ch = next_char();
			number = number * 10 + (ch - '0');
		}
		struct num *tmp = calloc(sizeof *tmp, 1);
		tmp->number = number;
		tmp->super.type = 0;
        tmp->super.ops = &number_v;
printf("%d\n", number);
		return &(tmp->super);
	} else {
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
printf("%s\n", "OP");
		return &tmp->super;
	}
}

static struct node *factor(void){
    if (symbol->type == 0) {
        struct node *temp = symbol;
        symbol = next_part();
        return temp;
    }
    return NULL;
}

static struct node *term(void){
    struct node *head = factor();
    while (peak_char() && symbol->type == 1 && (symbol->ops == &multiply || symbol->ops == &divide)){
        struct opr *temp = (struct opr *) symbol;
        temp->left_c = head;
        head = symbol;
        symbol = next_part();
        temp->right_c = factor();
    }
    return head;
}

static struct node *expression(void){
    struct node *head = term();
    while (peak_char() && symbol->type == 1 && (symbol->ops == &addition || symbol->ops == &subtract)){
        struct opr *temp = (struct opr *) symbol;
        temp->left_c = head;
        head = symbol;
        symbol = next_part();
        temp->right_c = term();
    }
    return head;
}

int main(int argc, char *argv[]){
	(void)argc;
	(void)argv;
	if(peak_char()){
        symbol = next_part();
        expression();
	}
}

/*
Expression
term {("+"|"-") term}

term
factor {("*"|"/") factor}

factor
number


Ex: 142 + 36

	    +
       / \
     142  36
*/