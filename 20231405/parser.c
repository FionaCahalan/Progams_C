/* ****GRAMMAR****
Expression
term {("+"|"-") term}

term
factor {("*"|"/") factor}

factor
(- | + factor ) | number | "(" expression ")" || variable | trig factor

number
"0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"

variable
Uppercase or lowercase letter

trig
"sin" | "cos" | "tan" | "csc" | "sec" | "cot"
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <setjmp.h>
#include <SDL.h>
#include <signal.h>
#include "linked_list.h"
typedef double intfp;

static jmp_buf jb;                  //errors handling
static struct node *vars['z' + 1];      //array of pointers to the variables, indexed by the character
static struct node *x_var;          //pointer to x, for use in graphing

struct node;
struct node_vtable{
	void (*print)(struct node *curr);
    intfp (*evaluate)(struct node *curr);
};

/*
Add parsing trig functions

fix the wholes, maybe by leaving the NaN values in the linked list 
instead of averaging between two xs, do it a little more randomly
by float->unsigned long long -> float and adding randomly generated
number between the difference of the two


*/



// types of symbols/parts
#define S_NUM 0             // number or variable
#define S_OPER 1            // operator such as * / - +
#define S_L_PAREN 2         // (
#define S_R_PAREN 3         // )
#define S_TRIG 4            // tan, cos, sec, cot, csc, sin

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
	intfp number;
};

struct var{
    struct node super;
    intfp value;
    char ch;
};


// function to evalulate operation or return number
static intfp mul(struct node *curr){
	struct opr *op = (struct opr *)curr;
    intfp a = op->left_c->ops->evaluate(op->left_c);
    intfp b = op->right_c->ops->evaluate(op->right_c);
    return a * b;
}

static intfp divi(struct node *curr){
	struct opr *op = (struct opr *)curr;
    intfp a = op->left_c->ops->evaluate(op->left_c);
    intfp b = op->right_c->ops->evaluate(op->right_c);
    return a / b;
}

static intfp add(struct node *curr){
	struct opr *op = (struct opr *)curr;
    intfp a = op->left_c->ops->evaluate(op->left_c);
    intfp b = op->right_c->ops->evaluate(op->right_c);
    return a + b;
}

static intfp sub(struct node *curr){
	struct opr *op = (struct opr *)curr;
    intfp a = op->left_c->ops->evaluate(op->left_c);
    intfp b = op->right_c->ops->evaluate(op->right_c);
    return a - b;
}

static intfp eval_cos(struct node *curr){
    struct opr *op = (struct opr *)curr;
    return cos(op->left_c->ops->evaluate(op->left_c));
}

static intfp eval_sin(struct node *curr){
    struct opr *op = (struct opr *)curr;
    return sin(op->left_c->ops->evaluate(op->left_c));
}

static intfp eval_tan(struct node *curr){
    struct opr *op = (struct opr *)curr;
    return tan(op->left_c->ops->evaluate(op->left_c));
}

/*
static intfp eval_sec(struct node *curr){
    struct opr *op = (struct opr *)curr;
    return sec(op->left_c->ops->evaluate(op->left_c));
}

static intfp eval_csc(struct node *curr){
    struct opr *op = (struct opr *)curr;
    return csc(op->left_c->ops->evaluate(op->left_c));
}

static intfp eval_cot(struct node *curr){
    struct opr *op = (struct opr *)curr;
    return cot(op->left_c->ops->evaluate(op->left_c));
} */

static intfp eval_num(struct node *curr){
    return ((struct num*)curr)->number;
}

static intfp eval_var(struct node *curr){
    return ((struct var*)curr)->value;
}

// function to print operator, variable, or number
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

static void print_cos(struct node *curr){
    (void) curr;
    printf("cos");
}

static void print_sin(struct node *curr){
    (void) curr;
    printf("sin");
}

static void print_tan(struct node *curr){
    (void) curr;
    printf("tan");
}
/*
static void print_cos(struct node *curr){
    (void) curr;
    printf("sec");
}

static void print_cos(struct node *curr){
    (void) curr;
    printf("csc");
}

static void print_cos(struct node *curr){
    (void) curr;
    printf("cot");
} */

static void print_num(struct node *curr){
    printf("%f", curr->ops->evaluate(curr));
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

static struct node_vtable cosine = {
    .print = print_cos,
    .evaluate = eval_cos,
};

static struct node_vtable sine = {
    .print = print_sin,
    .evaluate = eval_sin,
};

static struct node_vtable tangent = {
    .print = print_tan,
    .evaluate = eval_tan,
};

/*
static struct node_vtable secant = {
    .print = print_sec,
    .evaluate = eval_sec;
};

static struct node_vtable cosecant = {
    .print = print_csc,
    .evaluate = eval_csc;
};
static struct node_vtable cotangent = {
    .print = print_cot,
    .evaluate = eval_cot;
};*/

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
//static char equation[] = "a+b+2";
//static char equation[] = "ab";                        // should throw exepction
//static char equation[] = "?";                         // should throw exception  
//static char equation[] = "(23)";                      // should be fine                      
//static char equation[] = "sin(23)";
static char equation[] = "sin(x)";
//static char equation[] = "1/x";
//static char equation[] = "x";
//static char equation[] = "x*x/400";
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
#define E_RND_LETTERS 5         // multiple letters string together but not trig function, no implicite multiplication

#define IS_LETTER(ch) ch >= 'A' && ch <= 'z' && !( ch > 'Z' && ch < 'a')

// grabs characters to form next symbol (i.e. number or operator)
static void next_part(void){
	int ch = next_char();
    // skip spaces
    while (peak_char() != -1 && ch == ' ') {
        ch = next_char();
    }
    // if number
	if(ch != -1 && ch >= '0' && ch <= '9'){
		intfp number = ch - '0';
		while(peak_char() != -1 && peak_char() >= '0' && peak_char() <= '9'){
			ch = next_char();
			number = number * 10 + (ch - '0');
		}
		struct num *tmp = calloc(sizeof *tmp, 1);
		tmp->number = number;
		tmp->super.type = S_NUM;
        tmp->super.ops = &number_v;
		symbol = &(tmp->super);
    // if letter (variable) or trig function
	} else if(ch != -1 && IS_LETTER(ch)) {
        if(peak_char() != -1 && IS_LETTER(peak_char())){
            char letters[4];
            letters[3] = '\0';
            letters[0] = ch;
            letters[1] = next_char();
            if(peak_char() != -1 && IS_LETTER(peak_char())){
                letters[2] = next_char();
            } else{
                longjmp(jb, E_RND_LETTERS);
            }
            if (!strcmp("sin", letters)) {
                struct opr *tmp = calloc(sizeof *tmp, 1);
                tmp->super.type = S_TRIG;
                tmp->super.ops = &sine;
                symbol = &(tmp->super);
            } else if (!strcmp("cos", letters)) {
                struct opr *tmp = calloc(sizeof *tmp, 1);
                tmp->super.type = S_TRIG;
                tmp->super.ops = &cosine;
                symbol = &(tmp->super);
            } else if (!strcmp("tan", letters)) {
                struct opr *tmp = calloc(sizeof *tmp, 1);
                tmp->super.type = S_TRIG;
                tmp->super.ops = &tangent;
                symbol = &(tmp->super);
            } /* else if (strcmp("cot", letters)) {

            } else if (strcmp("csc", letters)) {

            } else if (strcmp("sec", letters)) {

            }*/ else {
                longjmp(jb, E_RND_LETTERS);
            }
        } else if(vars[ch] == NULL) {
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

// a factor is a number, variable, expression, or a trig function
static struct node *factor(void){
    struct node *temp;
    if(symbol->ops == &addition){
        next_part();
    }

    if (symbol->type == S_NUM){
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
    }else if (symbol->type == S_L_PAREN){
        next_part();
printf("test 3\n");
        temp = expression();
        if(symbol->type == S_R_PAREN){
            next_part();
            return temp;
        } else{
            longjmp(jb, E_MISS_PAREN);
        }
    }else if (symbol->type == S_TRIG){
        temp = symbol;
        next_part();
        ((struct opr *)temp)->left_c = factor();
        return temp;
    }
    longjmp(jb, E_NO_NUM);
}

// a term can be any number of factors seperated by * or /
// returns the head of the binary tree for the term
static struct node *term(void){
printf("test2\n");
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
printf("test1\n");
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
//printf("printing\n");
    if (curr->type == S_NUM){
        curr->ops->print(curr);
    } else if (curr->type == S_TRIG){
        curr->ops->print(curr);
        print_tree(((struct opr *)curr)->left_c);
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
    } else if (curr->type == S_TRIG){
        curr->ops->print(curr);
        print_tree1(((struct opr *)curr)->left_c, level + 1);
    } else {
        curr->ops->print(curr);
        printf("\n");
        print_tree1(((struct opr *)curr)->left_c, level + 1);
        print_tree1(((struct opr *)curr)->right_c, level + 1);
    }
}


#define W_HEIGHT 500
#define W_WIDTH 500
#define ZOOM 20.0
static int quitting;
static SDL_Window *graph;
static SDL_Renderer *rend;
static SDL_Texture *tex;
static sigjmp_buf slj;

// handles floating point exception
static void fpe_handler(int sig){
    siglongjmp(slj, sig);
}

struct list_item{
    struct list_node super;
    intfp x;
    int y;
};

static void display_graph(struct node *head){
    // declares handler to OS
    signal(SIGFPE, fpe_handler);
    if (!graph)
        return;

    unsigned *pixels_for_window = calloc(W_WIDTH * sizeof (unsigned), W_HEIGHT);

    struct list_item *ans_head = calloc(sizeof *ans_head, 1);
    list_init_head(&ans_head->super);

    for (volatile int i = -W_WIDTH/2; i < W_WIDTH/2; i++){
        // effectively colors window black
        for (int j = 0; j < W_HEIGHT; j++){
            pixels_for_window[j*W_WIDTH + i + W_WIDTH/2] = 0x000000; 
        }
        // sets value of variable representing x axis for calculations, "zooming" in
        ((struct var *)x_var)->value = i*(1/ZOOM);
        switch(sigsetjmp(slj, 1)){
            case 0:
                intfp eval_ans = head->ops->evaluate(head);

                int ans = nearbyint(eval_ans*(ZOOM)) + W_HEIGHT/2;

                // place white point if in range
                if (ans < W_HEIGHT && ans >= 0) {
                    pixels_for_window[(W_HEIGHT - ans)*W_WIDTH + i + W_WIDTH/2] = 0xffffff;
                    struct list_item *ans_new = calloc(sizeof *ans_new, 1);
                    ans_new->x =  i;
                    ans_new->y = ans;
                    list_add((&ans_head->super)->prev, &ans_new->super);
                }
                break;
            default:    // if floating point exception (i.e dividing by 0), don't plot that point
                signal(SIGFPE, fpe_handler);
                break;
        }
    }

    // Fill in holes in graph
    struct list_node *pos = (&(ans_head->super))->next;
    while(pos != &ans_head->super){
        struct list_item *first = (struct list_item *) pos;
        struct list_item *second = (struct list_item *) pos->next;
        // if too far apart
        if (pos->next != &ans_head->super && (first->y - second->y > 1 || second->y - first->y > 1)){    
            // new x value for new point
            double new_x = (first->x + second->x)/2;
            ((struct var *)x_var)->value = new_x*(1/ZOOM);
            intfp eval_ans = head->ops->evaluate(head);
            int ans = nearbyint(eval_ans*(ZOOM)) + W_HEIGHT/2;

            // place white (RED) point if in range
            if (ans < W_HEIGHT && ans >= 0) {
                int x_val = nearbyint(second->x);
                pixels_for_window[(W_HEIGHT - ans)*W_WIDTH + x_val + W_WIDTH/2] = 0xff0000;
                struct list_item *ans_new = calloc(sizeof *ans_new, 1);
                ans_new->x = new_x;
                ans_new->y = ans;
                // add point to linked list
                list_add(pos, &ans_new->super);
//printf("Added new point between %f %f y values: %d %d %d count: %d\n", first->x, second->x, first->y, second->y, ans_new->y, list_count(&ans_head->super));
                // if point is equal to the first or second point, move on
                if (ans_new->y == first->y){
                    pos = pos->next;
                } else if (ans_new->y == second->y){
                    pos = pos->next->next;
                } 
            } else { // point out of range, move on
                pos = pos->next;
            }
        } else {
            pos = pos->next;
        }
        // ADD ERROR CHECKING!
    }
    void *pixels;
    int pitch;

    SDL_Rect recent_texrect = (SDL_Rect){.x=0, .y=0, .w = W_WIDTH, .h = W_HEIGHT};
    // lock the whole texture
    SDL_LockTexture(tex, &recent_texrect, &pixels, &pitch);
    memcpy(pixels, pixels_for_window, pitch*W_HEIGHT);
    SDL_UnlockTexture(tex);

    /*
    SDL_Rect recent_srcrect = (SDL_Rect){.x = 0, .y = 0, .w = W_WIDTH, .h = W_HEIGHT};
	SDL_Rect recent_dstrect = (SDL_Rect){.x = 0, .y = 0, .w = W_HEIGHT, .h = W_HEIGHT};
	SDL_RenderCopy(rend, tex, &recent_srcrect, &recent_dstrect);
    */
    // paste all of the texture onto all of the renderer
    SDL_RenderCopy(rend, tex, NULL, NULL);

    SDL_RenderPresent(rend);
    free(pixels_for_window);
}

static void closeWindow(void){
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(graph);
    SDL_DestroyTexture(tex);
}

static void handle_event(SDL_Event *event){
    switch(event->type){
        case SDL_WINDOWEVENT:
            if(event->window.event == SDL_WINDOWEVENT_CLOSE){
                closeWindow();
            }

            if (event->window.event == SDL_WINDOWEVENT_EXPOSED){
                // for some reason this event is called "exposed" and not "maximized"
                // could maybe be some crazy stuff that happens if this event is dealt with for a closed window.
                if(graph){
                    SDL_RenderCopy(rend, tex, NULL, NULL);
                    SDL_RenderPresent(rend);
                }
            }

            break;
        case SDL_QUIT:
            quitting = 1;
            break; 
    }
}

int main(int argc, char *argv[]){
	(void)argc;
	(void)argv;
	if(peak_char() != -1){ 
        struct node *head;
        next_part();
        // error handling
        switch(setjmp(jb)){
            case 0:
                head = expression();
//printf("%f\n", sin(23.0));
                intfp answer = head->ops->evaluate(head);
                printf("%f\n", answer);

                /*                
                for(int i = 0; i <= 10; i++) {
                    ((struct var *)x_var)->value = i;
                    printf("%c = %f, y = %f\n", ((struct var *)x_var)->ch, i, head->ops->evaluate(head));
                }
                */
                
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
            case E_RND_LETTERS:
                printf("multiple letters string together but not trig function, no implicite multiplication\n");
                exit(1);
                break;
            default:
                printf("unknown error\n");
                exit(1);
                break;
        }

        // NEED TO ADD ERROR THAT IT CAN ONLY GRAPH IS X VAR EXISTS
        	
        // initializing stuff
        if(SDL_Init(SDL_INIT_EVERYTHING)<0){
            printf("Failed SDL_Init %s\n", SDL_GetError());
            return 1;
        }

        // creating the graph
        graph = SDL_CreateWindow("Graph", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W_WIDTH, W_HEIGHT, 0);
        rend = SDL_CreateRenderer(graph, -1, 0);
        tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, W_WIDTH, W_HEIGHT);

        display_graph(head);
        // while graph has not been closed
        while(!quitting) {
            SDL_Event e;

            while(!quitting && SDL_WaitEvent(&e)){
                handle_event(&e);
            }
        }
    }
}


