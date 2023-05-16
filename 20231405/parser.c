#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct node{
	struct node *children;
};

struct opr{
	struct node super;
	int (*opr)(int, int);
};

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

static int c_idx = 0;
static char next_char(void){
	char equation[] = "124*34";
	return equation[c_idx++];
} 

int main(int argc, char *argv[]){
	(void)argc;
	(void)argv;
	int ch = next_char();
	int number = 0;
	while(ch) {
		if(ch > 47 && ch < 58) {
			number = number * 10 + (ch - 48);
		} else {
			printf("%d\n", number);
			number = 0;
			struct opr *temp = malloc(sizeof *temp);
			if(ch == 42){
				temp->opr = multiply;
			} if(ch == 43){
				temp->opr = add;
			} if(ch == 45) {
				temp->opr = subtract;
			} if(ch == 47) {
				temp->opr = divide;
			}
		}
		ch = next_char();
	}
	printf("%d\n", number);
}

/*
Possible approaches:

- Split the lexer and parser
- Recursion
- use the tree to simulate recursion

Add part
if node is non-delimiter,
- if node doesn't have two children, call recursively on right child, left child should already be made?
- else call recursively, creating parent?

if delimiter
- next part is the parent of the delimiter, UNLESS delimeter is right child (then return).



Ex: 142 + 36

	+
       / \
     142  36
*/