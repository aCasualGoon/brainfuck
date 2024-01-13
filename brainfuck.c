#define VERSION "0.2.0"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

char lastout = '\n';

void error(const char *msg) {
    if(lastout != '\n') putchar('\n');
    printf("Error: %s\n", msg);
    exit(1);
}
void errif(int cond, const char *msg) {
    if(cond) error(msg);
}
void errorf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    if(lastout != '\n') {
        putchar('\n');
    }
    
    printf("Error: ");
    vprintf(format, args);
    printf("\n");

    va_end(args);
    exit(1);
}

// is string a equal to string b?
#define streq(a, b) (strcmp(a, b) == 0)
// is string a equal to string b or string c?
#define streq2(a, b, c) (streq(a, b) || streq(a, c))

/* ------------------ PROGRAM CODE ------------------ */

char *program;
unsigned long program_size;
unsigned long program_ptr = 0;

void append2program(char *expression) {
    FILE *f = fopen(expression, "r");

    int predicted_size = 0;
    if(f) {
        fseek(f, 0, SEEK_END);
        predicted_size = ftell(f);
        fseek(f, 0, SEEK_SET);
    } else {
        predicted_size = strlen(expression);
    }

    char *buffer = malloc(predicted_size);
    errif(!buffer, "Failed to allocate memory for program (malloc)");
    int actual_size = 0;
    for(int i = 0; i < predicted_size; i++) {
        char c = f ? fgetc(f) : expression[i];
        if(c == '+' || c == '-' || c == '<' || c == '>' || c == '[' || c == ']' || c == '.' || c == ',')
            buffer[actual_size++] = c;
    }
    if(f) fclose(f);

    if(actual_size > 0) {
        program = realloc(program, program_size + actual_size);
        errif(!program, "Failed to allocate memory for program (realloc)");
        memcpy(program + program_size, buffer, actual_size);
        program_size += actual_size;
    }
    free(buffer);
}

/* ------------------ TAPE ------------------ */

typedef struct node {
    unsigned char value;
    struct node *left;
    struct node *right;
} node;

node* new_node(node *left, node *right) {
    node *n = malloc(sizeof(node));
    errif(!n, "Failed to allocate memory for tape");
    n->value = 0;
    n->left = left;
    n->right = right;
    return n;
}

node *current;

/* ------------------ COMMANDS ------------------ */

void command_increment() { current->value++; }
void command_decrement() { current->value--; }
void command_left() {
    if(!current->left) 
        current->left = new_node(NULL, current);
    current = current->left;
}
void command_right() {
    if(!current->right) 
        current->right = new_node(current, NULL);
    current = current->right;
}
void command_loop() {
    if(!current->value) {
        int depth = 1;
        while(depth) {
            program_ptr++;
            depth += (program[program_ptr] == '[') - (program[program_ptr] == ']');
        }
    }
}
void command_endloop() {
    if(current->value) {
        int depth = 1;
        while(depth) {
            program_ptr--;
            depth += (program[program_ptr] == ']') - (program[program_ptr] == '[');
        }
    }
}
void command_out() { 
    lastout = current->value;
    putchar(lastout);
}
void command_in() { 
    if(lastout != '\n') putchar('\n');
    putchar(':');
    current->value = getchar();
    while(getchar() != '\n');
}

void exec() {
    while(program_ptr < program_size) {
        switch(program[program_ptr]) {
            case '+': command_increment(); break;
            case '-': command_decrement(); break;
            case '<': command_left();      break;
            case '>': command_right();     break;
            case '[': command_loop();      break;
            case ']': command_endloop();   break;
            case '.': command_out();       break;
            case ',': command_in();        break;
        }
        program_ptr++;
    }
}

/* ------------------ INTERACTIVE SHELL ------------------ */

#define SHELLMAXINPUTLENGTH 1024
void shell() { // we're not filtering for commands here
    program = malloc(SHELLMAXINPUTLENGTH);
    errif(!program, "Failed to allocate memory for program");
    while(1) {
        // prompt
        if(lastout != '\n') putchar('\n');
        printf("$");
        lastout = '\n';

        // read program from stdin
        fgets(program, SHELLMAXINPUTLENGTH, stdin);

        // parse special commands
        if(streq(program, "exit\n")) exit(0);
        if(streq(program, "help\n")) {
            printf("Commands:\n");
            printf("  +    - increment the current cell\n");
            printf("  -    - decrement the current cell\n");
            printf("  <    - move the pointer left\n");
            printf("  >    - move the pointer right\n");
            printf("  [    - start a loop while current cell != 0\n");
            printf("  ]    - end a loop\n");
            printf("  .    - output the current cell\n");
            printf("  ,    - input a value into the current cell\n");
            printf("  exit - exit the shell\n");
            printf("  help - display this help message\n");
            continue;
        }

        // set program size and pointer
        program_size = strlen(program);
        program_ptr = 0;

        // execute
        exec();
    }
}

/* ------------------ MAIN ------------------ */

void compile(char *executable_name);

// blank a string (useful for clearing args that are not to be interpreted as code)
void blank(char *str) {
    for(int i = 0; i < strlen(str); i++)
        str[i] = '\0';
}

void cleanup() {
    if(program) free(program);
    
    while(current->left)
        current = current->left;
    while(current->right) {
        node *tmp = current;
        current = current->right;
        free(tmp);
    }
    free(current);

    if(lastout != '\n') putchar('\n');
}

#define ARGFLAG_COMPILE 0b00000001

int isvalidfilename(char *filename) {
    if(strlen(filename) == 0) return 0;
    for(int i = 0; i < strlen(filename); i++)
        if(filename[i] == '/' || filename[i] == '\\') return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    atexit(cleanup);
    current = new_node(NULL, NULL);

    if(argc == 1) {
        shell();
        return 0;
    }

    // parse args
    int compile_exeidx = -1;
    for(int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if(streq(arg, "--version")) {
            printf("version %s\n", VERSION);
            blank(arg);
            continue;
        }

        if(streq2(arg, "-h", "--help")) {
            printf("Usage: brainfuck [options] [program]\n");
            printf("Options:\n");
            printf("  -h,        --help           - display this help message\n");
            printf("             --version        - display the version number\n");
            printf("  -c <file>, --compile <file> - compile the program to the specified an executable\n");
            printf("\n");
            printf("If no program is specified, an interactive shell will be started.\n");
        }

        if(streq2(arg, "-c", "--compile")) {
            compile_exeidx = i + 1;
            if(compile_exeidx >= argc)
                error("Compilation requires an executable name");
            if(!isvalidfilename(argv[compile_exeidx]))
                errorf("Invalid executable name: \"%s\"", argv[compile_exeidx]);
            blank(arg);
            continue;
        }
    }

    for(int i = 1; i < argc; i++)
        if(i != compile_exeidx)
            append2program(argv[i]);

    if(compile_exeidx != -1)
        compile(argv[compile_exeidx]);
    else
        exec();
}

/* ------------------ COMPILER ------------------ */

#define COMPILE_BASE \
"#include <stdlib.h>\n" \
"int printf(const char *__restrict__ __format, ...);\n" \
"int getchar(void);\n" \
"int putchar(int);\n" \
"\n" \
"char lastout = '\\n';\n" \
"\n" \
"struct node {\n" \
"    unsigned char value;\n" \
"    struct node *left;\n" \
"    struct node *right;\n" \
"} *current;\n" \
"\n" \
"struct node* new_node(struct node *left, struct node *right) {\n" \
"    struct node *n = malloc(sizeof(struct node));\n" \
"    if(!n) {\n" \
"        if(lastout != '\\n') putchar('\\n');\n" \
"        printf(\"Error: Failed to allocate memory for tape\\n\");\n" \
"        exit(1);\n" \
"    }\n" \
"    n->value = 0;\n" \
"    n->left = left;\n" \
"    n->right = right;\n" \
"    return n;\n" \
"}\n" \
"\n" \
"void left() {\n" \
"    if(!current->left) \n" \
"        current->left = new_node(NULL, current);\n" \
"    current = current->left;\n" \
"}\n" \
"void right() {\n" \
"    if(!current->right) \n" \
"        current->right = new_node(current, NULL);\n" \
"    current = current->right;\n" \
"}\n" \
"void out() { \n" \
"    lastout = current->value;\n" \
"    putchar(lastout);\n" \
"}\n" \
"void in() { \n" \
"    if(lastout != '\\n') putchar('\\n');\n" \
"    putchar(':');\n" \
"    current->value = getchar();\n" \
"    while(getchar() != '\\n');\n" \
"}\n" \
"\n" \
"void cleanup() {\n" \
"    while(current->left)\n" \
"        current = current->left;\n" \
"    while(current->right) {\n" \
"        struct node *tmp = current;\n" \
"        current = current->right;\n" \
"        free(tmp);\n" \
"    }\n" \
"    free(current);\n" \
"\n" \
"    if(lastout != '\\n') putchar('\\n');\n" \
"}\n" \
"\n" \
"int main() {\n" \
"    atexit(cleanup);\n" \
"    current = new_node(NULL, NULL);\n"
#define COMPILE_INCR(N)  "    current->value+=%d;\n", N
#define COMPILE_DECR(N)  "    current->value-=%d;\n", N
#define COMPILE_LEFT(N)  "    for(int i = 0; i < %d; i++) left();\n", N
#define COMPILE_RITE(N)  "    for(int i = 0; i < %d; i++) right();\n", N
#define COMPILE_LOOP     "    while(current->value) {\n"
#define COMPILE_ENDL     "    }\n"
#define COMPILE_OUT(N)   "    for(int i = 0; i < %d; i++) out();\n", N
#define COMPILE_IN(N)    "    for(int i = 0; i < %d; i++) in();\n", N
#define COMPILE_END      "}"

#define INTERMEDIATE_FILE "__BFINTERM__"

// determines how many identical chars immediately follow the current program pointer
// also increments the program pointer to the last char in the block
int len_charblock() {
    int len = 1;
    while(program[program_ptr] == program[program_ptr + len])
        len++;
    program_ptr += len - 1;
    return len;
}

void compile(char *executable_name) {
    // transpile to C
    FILE *f = fopen(INTERMEDIATE_FILE, "w");
    errif(!f, "Build failed: Failed to create intermediate file");
    fprintf(f, COMPILE_BASE);
    for(program_ptr = 0; program_ptr < program_size; program_ptr++)
        switch(program[program_ptr]) {
            case '+': fprintf(f, COMPILE_INCR(len_charblock())); break;
            case '-': fprintf(f, COMPILE_DECR(len_charblock())); break;
            case '<': fprintf(f, COMPILE_LEFT(len_charblock())); break;
            case '>': fprintf(f, COMPILE_RITE(len_charblock())); break;
            case '[': fprintf(f, COMPILE_LOOP); break;
            case ']': fprintf(f, COMPILE_ENDL); break;
            case '.': fprintf(f, COMPILE_OUT(len_charblock())); break;
            case ',': fprintf(f, COMPILE_IN(len_charblock())); break;
        }
    fprintf(f, COMPILE_END);
    fclose(f);

    // compile to executable
    char cmd[strlen("gcc -x c -O3 -o  ") + strlen(executable_name) + strlen(INTERMEDIATE_FILE)];
    sprintf(cmd, "gcc -x c -O3 -o %s %s", executable_name, INTERMEDIATE_FILE);
    system(cmd);

    remove(INTERMEDIATE_FILE);
}