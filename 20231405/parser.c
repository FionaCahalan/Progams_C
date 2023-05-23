#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct node;

struct node_vtable{
	// void (*print)();
    int (*evaluate)(int, int);
};

struct node{
	struct node *left_c;
	struct node *right_c;
	int type;
    // 0 = number
	// 1 = function
};

struct opr{
	struct node super;
    struct node_vtable *opr;
};

/*
fix naming. 
have operator store the left and right child
Have node store the v_table and num store the number

*/


struct num{
	struct node super;
	int number;
};

static int multiply(int a, int b){
	return a * b;
}

static int divide(int a, int b){
	return a / b;
}

static int add(int a, int b){
	return a + b;
}

static int subtract(int a, int b){
	return a - b;
}

static struct node_vtable multi = {
    // .print = 
    .evaluate = multiply,
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
		printf("%d\n", number);
		return &(tmp->super);
	} else {
		struct opr *tmp = calloc(sizeof *tmp, 1);
		if(ch == '*'){
			tmp->opr = &multi;
		} if(ch == '+'){
			tmp->opr = &multi;
		} if(ch == '-') {
			tmp->opr = &multi;
		} if(ch == '/') {
			tmp->opr = &multi;
		}
		tmp->super.type = 1;
		printf("%s\n", "OP");
		return &tmp->super;
	}
}

static struct node *factor(void){
    if (symbol == num) {
        struct node *temp = symbol;
        symbol = next_part();
        return temp;
    }
}

static struct node *term(void){
    struct node *head = factor();
    while (peak_char() && symbol. ==  * /){
        symbol->left_c = head;
        head = symbol;
        symbol = next_part();
        head->right_c = factor();
    }
    return head;
}

static struct node *expression(void){
    struct node *head = term();
    while (peak_char() && symbol->type == 1 && (symbol+-)){
        symbol->left_c = head;
        head = symbol;
        symbol = next_part();
        head->right_c = term();
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