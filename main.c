#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMPILE_CODEBASE \
"#include <stdlib.h>\n" \
"int printf(const char *__restrict__ __format, ...);\n" \
"int getchar(void);\n" \
"struct node {\n" \
"    char data;\n" \
"    struct node *left;\n" \
"    struct node *right;\n" \
"} *current;\n" \
"\n" \
"char last_output = '\\n';\n" \
"\n" \
"static inline struct node *new_node(struct node *left, struct node *right) {\n" \
"    struct node *new = malloc(sizeof(struct node));\n" \
"    if(new == ((void *)0)) {\n" \
"        printf(\"\\nERROR: Failed to allocate tape memory.\\n\");\n" \
"        exit(1);\n" \
"    }\n" \
"    new->data = 0;\n" \
"    new->left = left;\n" \
"    new->right = right;\n" \
"    return new;\n" \
"}\n" \
"\n" \
"static inline void cleanup() {\n" \
"    while(current->left != ((void *)0))\n" \
"        current = current->left;\n" \
"    struct node *temp;\n" \
"    while(current != ((void *)0)) {\n" \
"        temp = current;\n" \
"        current = current->right;\n" \
"        free(temp);\n" \
"    }\n" \
"    if(last_output != '\\n')\n" \
"        printf(\"\\n\");\n" \
"}\n" \
"\n" \
"static inline void move_left() {\n" \
"    if(current->left == ((void *)0))\n" \
"        current->left = new_node(((void *)0), current);\n" \
"    current = current->left;\n" \
"}\n" \
"\n" \
"static inline void move_right() {\n" \
"    if(current->right == ((void *)0))\n" \
"        current->right = new_node(current, ((void *)0));\n" \
"    current = current->right;\n" \
"}\n" \
"\n" \
"static inline void output() {\n" \
"    last_output = current->data;\n" \
"    printf(\"%c\", last_output);\n" \
"}\n" \
"\n" \
"static inline void input() {\n" \
"    printf(last_output == '\\n' ? \":\" : \"\\n:\");\n" \
"    current->data = getchar();\n" \
"    while(getchar() != '\\n');\n" \
"}\n" \
"\n" \
"int main() {\n" \
"    atexit(cleanup);\n" \
"    current = new_node(((void *)0), ((void *)0));\n"
#define COMPILE_MOVL "    move_left();\n"
#define COMPILE_MOVR "    move_right();\n"
#define COMPILE_INCR "    current->data++;\n"
#define COMPILE_DECR "    current->data--;\n"
#define COMPILE_OUTP "    output();\n"
#define COMPILE_INPT "    input();\n"
#define COMPILE_LOOP "    if(current->data != 0) do{\n"
#define COMPILE_ENDL "    }while(current->data != 0);\n"

// the last character written to the console
char last_output = '\n';

// tape node
struct node {
    char data;
    struct node *left;
    struct node *right;
} *current;

// create a new node with the specified left and right nodes
// returns a pointer to the new node or NULL if malloc fails
struct node *new_node(struct node *left, struct node *right) {
    struct node *new = malloc(sizeof(struct node));
    if(new == NULL) {
        printf("\nERROR: Failed to allocate tape memory.\n");
        return NULL;
    }
    new->data = 0;
    new->left = left;
    new->right = right;
    return new;
}

// free up the tape resources and print a newline if necessary
void cleanup() {
    // go to the leftmost node
    while(current->left != NULL)
        current = current->left;
    // free the tape
    struct node *temp;
    while(current != NULL) {
        temp = current;
        current = current->right;
        free(temp);
    }
    
    // print a newline if the last character written to the console was not a newline
    if(last_output != '\n')
        printf("\n");
}

// initialize the tapes first node
void init_tape() {
    atexit(cleanup);
    current = new_node(NULL, NULL);
    if(current == NULL)
        exit(1);
}

// move current pointer one space to the left
// if the left node does not exist, create it
// returns 1 on success, 0 on failure
int move_left() {
    if(current->left == NULL) {
        current->left = new_node(NULL, current);
        if(current->left == NULL)
            return 0;
    }
    current = current->left;
    return 1;
}

// move current pointer one space to the right
// if the right node does not exist, create it
// returns 1 on success, 0 on failure
int move_right() {
    if(current->right == NULL) {
        current->right = new_node(current, NULL);
        if(current->right == NULL)
            return 0;
    }
    current = current->right;
    return 1;
}

// increment the current node by one
void increment() { current->data++; }
// decrement the current node by one
void decrement() { current->data--; }

// print the value of the current node
void output() { 
    last_output = current->data;
    printf("%c", last_output);
}
// read a character from the console and store it in the current node
void input() {
    printf(last_output == '\n' ? ":" : "\n:");
    current->data = getchar();
    // discard any extra characters
    while(getchar() != '\n');
}

// executes a brainfuck program
// returns 1 on success, 0 on failure
int execute(char *program) {
    while(*program != '\0') {
        switch(*program) {
            case '>': if(!move_right()) return 0; break;
            case '<': if(!move_left())  return 0; break;
            case '+': increment(); break;
            case '-': decrement(); break;
            case '.': output(); break;
            case ',': input(); break;
            case '[': if(current->data == 0) {
                          int depth = 1;
                          while(depth > 0) {
                              program++;
                              if(*program == '[') depth++;
                              else if(*program == ']') depth--;
                          }
                      }
                      break;
            case ']': if(current->data != 0) {
                          int depth = 1;
                          while(depth > 0) {
                              program--;
                              if(*program == '[') depth--;
                              else if(*program == ']') depth++;
                          }
                      }
                      break;
        }
        program++;
    }
    return 1;
}

char* prompt() {
    if(last_output != '\n')
        printf("\n");
    printf("$ ");
    char *input = malloc(256);
    if(input == NULL) {
        printf("\nERROR: Failed to allocate memory for input.\n");
        exit(1);
    }
    fgets(input, 256, stdin);
    return input;
}

// function for the interactive brainf shell
// runs until the user enters "exit"
void intercative() {
    char *program;
    while(1) {
        program = prompt();
        if(strcmp(program, "exit\n") == 0) {
            free(program);
            break;
        }
        if(!execute(program)) {
            free(program);
            exit(1);
        }
        free(program);
    }
}

// if the argument is a file, return the contents of the file
// otherwise, return the argument
char* parseArg(char *arg, int *isnew) {
    *isnew = 0;
    FILE *file = fopen(arg, "r");
    if(file == NULL)
        return arg;
    *isnew = 1;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    char *program = malloc(size + 1);
    if(program == NULL) {
        printf("\nERROR: Failed to allocate memory for program.\n");
        exit(1);
    }
    fread(program, 1, size, file);
    program[size] = '\0';
    fclose(file);
    return program;
}

int main(int argc, char *argv[]) {
    init_tape();

    if(argc == 1) { // run brainfuck as an interactive shell
        intercative();
        exit(0);
    }

    int do_compile = 1;

    if(!do_compile) { // execute the program directly
        for(int i = 1; i < argc; i++) {
            int isnew;
            char *program = parseArg(argv[i],&isnew);
            if(!execute(program)) {
                if(isnew) free(program);
                exit(1);
            }
            if(isnew) free(program);
        }
        exit(0);
    }

    // compile the program to C and then compile the C code

    // transpile the program to file "__intermediate_c_2_brainfuck__.c"
    FILE *file = fopen("__intermediate_c_2_brainfuck__.c", "w");
    fputs(COMPILE_CODEBASE,file);
    for(int i = 1; i < argc; i++) {
        int isnew;
        char *program = parseArg(argv[i],&isnew);
        for(int i = 0; i < strlen(program); i++) {
            switch(program[i]) {
                case '>': fputs(COMPILE_MOVR, file); break;
                case '<': fputs(COMPILE_MOVL, file); break;
                case '+': fputs(COMPILE_INCR, file); break;
                case '-': fputs(COMPILE_DECR, file); break;
                case '.': fputs(COMPILE_OUTP, file); break;
                case ',': fputs(COMPILE_INPT, file); break;
                case '[': fputs(COMPILE_LOOP, file); break;
                case ']': fputs(COMPILE_ENDL, file); break;
            }
        }
        if(isnew) free(program);
    }
    fprintf(file, "}\n");
    fclose(file);
}