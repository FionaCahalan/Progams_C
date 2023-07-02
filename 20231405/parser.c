#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <setjmp.h>

static jmp_buf jb;                  //errors handling
static struct node *vars[256];      //array of pointers to the variables, indexed by the character
static struct node *x_var;          //pointer to x, for use in graphing

struct node;
struct node_vtable{
	void (*print)(struct node *curr);
    int (*evaluate)(struct node *curr);
};

// types of symbols/parts
#define S_NUM 0             // number or variable
#define S_OPER 1            // operator such as * / - +
#define S_L_PAREN 2         // (
#define S_R_PAREN 3         // )

struct node{
	struct node_vtable *ops;
	int type;
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

struct var{
    struct node super;
    int value;
    char ch;
};


// function to evalulate operation or return number
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

static int eval_var(struct node *curr){
    return ((struct var*)curr)->value;
}

static void print_mul(struct node *curr){
	(void) curr;
    printf("*");
}

static void print_div(struct node *curr){
    (void) curr;
	printf("/");
}

static void print_add(struct node *curr){
    (void) curr;
	printf("+");
}

static void print_sub(struct node *curr){
    (void) curr;
	printf("-");
}

static void print_num(struct node *curr){
    printf("%d", curr->ops->evaluate(curr));
}

static void print_var(struct node *curr){
    (void) curr;
    printf("%c", ((struct var *) curr)->ch);
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

static struct node_vtable variable_v = {
    .print = print_var,
    .evaluate = eval_var,
};


static unsigned c_idx = 0;
//static char equation[] = "124*34+2*30*5";             //4516
//static char equation[] = "12*34 -2* (3-6)";           //414
//static char equation[] = "4*(6-3)+24-5";              //31
//static char equation[] = "(((2*4)*3))";               //24
//static char equation[] = "--2+36";                    //38
//static char equation[] = "-(2*3-+8)";                 //2
//static char equation[] = "3*-2";                      //-6
//static char equation[] = "()";                        //
//static char equation[] = "3+*7";                      //21
//static char equation[] = "3*4+6a";                    //no implicite multiplication
//statis char equation[] = "a6"                         //no implicite multiplication
//static char equation[] = "3(45)";                     //no implicite multiplication
//static char equation[] = "(3)4";                      //no implicite multiplication
//static char equation[] = "a*a";
//static char equation[] = "3*4+6*a";                   // x = 0, x = 12
static char equation[] = "a+b+2";
static struct node* symbol;

// returns current char, increments curr char
static char next_char(void){
    if (c_idx < sizeof equation - 1)
        return equation[c_idx++];
    else return -1;
}

// returns current char
static char peak_char(void){
	if (c_idx < sizeof equation - 1)
        return equation[c_idx];
    else return -1;
}

// errors types
#define E_NO_NUM 1              // no number when expected
#define E_UNKNOWN_SYM 2         // unknown symbol
#define E_MISS_PAREN 3          // odd number of parenthesis, no parenthesis when expected
#define E_IMPLI_MUL 4           // implicite multiplication

// grabs characters to form next symbol (i.e. number or operator)
static void next_part(void){
	int ch = next_char();
    // skip spaces
    while (peak_char() != -1 && ch == ' ') {
        ch = next_char();
    }
    // if number
	if(ch != -1 && ch >= '0' && ch <= '9'){
		int number = ch - '0';
		while(peak_char() != -1 && peak_char() >= '0' && peak_char() <= '9'){
			ch = next_char();
			number = number * 10 + (ch - '0');
		}
		struct num *tmp = calloc(sizeof *tmp, 1);
		tmp->number = number;
		tmp->super.type = S_NUM;
        tmp->super.ops = &number_v;
		symbol = &(tmp->super);
    // if letter (variable)
	} else if (ch != -1 && ch >= 'A' && ch <= 'z' && !(ch > 'Z' && ch < 'a')) {
        if(vars[ch] == NULL) {
            struct var *tmp = calloc(sizeof *tmp, 1);
            tmp->ch = ch;
            tmp->super.type = S_NUM;
            tmp->super.ops = &variable_v;
            symbol = &(tmp->super);
            x_var = symbol;
            vars[ch] = symbol;
        } else {
            symbol = vars[ch];
        }
    } else if (ch != -1){   // else must be other symbol, operator, or unknown symbol
		struct opr *tmp = calloc(sizeof *tmp, 1);
		tmp->super.type = S_OPER;
        switch(ch){
        case '*':
			tmp->super.ops = &multiply;
            break;
		case '/':
			tmp->super.ops = &divide;
            break;
        case '+':
			tmp->super.ops = &addition;
            break;
		case '-':
			tmp->super.ops = &subtract;
            break;
        case '(':
            tmp->super.type = S_L_PAREN;
            break;
        case ')':
            tmp->super.type = S_R_PAREN;
            break;
		default:
            longjmp(jb, E_UNKNOWN_SYM);
        }
		symbol = &tmp->super;
	} else {
        symbol = NULL;
    }
}

static struct node *expression(void);

// a factor is a number or an expression
static struct node *factor(void){
    struct node *temp;
    if(symbol->ops == &addition){
        next_part();
    }

    if (symbol->type == S_NUM) {
        temp = symbol;
        next_part();
        return temp;
    }else if(symbol->ops == &subtract){
        struct opr *neg = calloc(sizeof *neg, 1);
        neg->super.type = S_OPER;
        neg->super.ops = &multiply;

        next_part();
        struct num *neg_1 = calloc(sizeof *neg_1, 1);
        neg_1->number = -1;
        neg_1->super.type = S_NUM;
        neg_1->super.ops = &number_v;
        neg->left_c = &neg_1->super;
        neg->right_c = factor();
        return &neg->super;
    }else if (symbol->type == S_L_PAREN) {
        next_part();
        temp = expression();
        if(symbol->type == S_R_PAREN){
            next_part();
            return temp;
        } else{
            longjmp(jb, E_MISS_PAREN);
        }
    }
    longjmp(jb, E_NO_NUM);
}

// a term can be any number of factors seperated by * or /
// returns the head of the binary tree for the term
static struct node *term(void){
    struct node *head = factor();
    while (symbol != NULL && (symbol->ops == &multiply || symbol->ops == &divide)){
        struct opr *temp = (struct opr *) symbol;
        temp->left_c = head;
        head = symbol;
        next_part();
        temp->right_c = factor();
    }
    if (symbol != NULL && (symbol->type == S_NUM || symbol->type == S_L_PAREN)) {      // checking for implicite multiplication
        longjmp(jb, E_IMPLI_MUL);
    }
    return head;
}

// an expression can be any number of terms seperated by + or -
// returns the head of the binary tree for an expression
static struct node *expression(void){
    struct node *head = term();
    while (symbol != NULL && (symbol->ops == &addition || symbol->ops == &subtract)){
        struct opr *temp = (struct opr *) symbol;
        temp->left_c = head;
        head = symbol;
        next_part();
        temp->right_c = term();
    }
    if (symbol != NULL && (symbol->type == S_NUM || symbol->type == S_L_PAREN)) {      // checking for implicite multiplication
        longjmp(jb, E_IMPLI_MUL);
    }
    return head;
}

// prints tree inorder, effectively printing original input
static void print_tree(struct node *curr){
    if (curr->type == S_NUM){
        curr->ops->print(curr);
    } else {
        print_tree(((struct opr *)curr)->left_c);
        curr->ops->print(curr);
        print_tree(((struct opr *)curr)->right_c);
    }
}

// prints tree indented, preorder
static void print_tree1(struct node *curr, int level){
    printf("%-*s", level*3, "");
    if (curr->type == S_NUM){
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
        // error handling
        switch(setjmp(jb)){
            case 0:
                printf("happiness\n");
                struct node *head = expression();
                //int answer = head->ops->evaluate(head);
                //printf("%d\n", answer);

                for(int i = 0; i <= 10; i++) {
                    ((struct var *)x_var)->value = i;
                    printf("var = %d, y = %d\n", i, head->ops->evaluate(head));
                }
                
                print_tree(head);
                printf("\n");
                print_tree1(head, 0);
                break;
            case E_NO_NUM:
                printf("number expected, no num before %c\n", peak_char());
                exit(1);
                break;
            case E_UNKNOWN_SYM:
                printf("unknown symbol before %c\n", peak_char());
                exit(1);
                break;
            case E_MISS_PAREN:
                printf("missing a parenthesis \n");
                exit(1);
                break;
            case E_IMPLI_MUL:
                printf("no implicite multiplcation\n");
                exit(1);
                break;
            default:
                printf("unknown error\n");
                exit(1);
                break;
        }
	}
}

// struct variables hold the integer, put them in a pointer array, assume the first variable
// in the pointer array is x. When we encounter a variable, check the pointer array, if it's not in there then
// create a new one, else return that one. Just use 256.

/* ****GRAMMAR****
Expression
term {("+"|"-") term}

term
factor {("*"|"/") factor}

factor
(- | + factor ) | number | "(" expression ")"

number
"0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"

*/