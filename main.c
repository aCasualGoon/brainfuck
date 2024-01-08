#include <stdio.h>
#include <stdlib.h>

// the last character written to the console
char last_output = '\n';

// tape node
struct node {
    char data;
    struct node *left;
    struct node *right;
} *current;

// create a new node with the specified left and right nodes
struct node *new_node(struct node *left, struct node *right) {
    struct node *new = malloc(sizeof(struct node));
    if(new == NULL) {
        printf("\nERROR: Failed to allocate tape memory.\n");
        exit(1);
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
    current = new_node(NULL, NULL);
    atexit(cleanup);
}

// move current pointer one space to the left
// if the left node does not exist, create it
// exits the program if malloc fails (probably out of memory)
void move_left() {
    if(current->left == NULL)
        current->left = new_node(NULL, current);
    current = current->left;
}

// move current pointer one space to the right
// if the right node does not exist, create it
// exits the program if malloc fails (probably out of memory)
void move_right() {
    if(current->right == NULL)
        current->right = new_node(current, NULL);
    current = current->right;
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
void execute(char *program) {
    while(*program != '\0') {
        switch(*program) {
            case '>': move_right(); break;
            case '<': move_left(); break;
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
}

int main(int argc, char *argv[]) {
    init_tape();
    execute(argv[1]);
}