#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_STACK 4001
#define MAX_LABEL 1000
#define MAX_INSTRUCTION 3000
#define MAX_ADDRESS 5001
#define INT_LENGTH 11
#define LABEL_BUFFER 2001
#define END (-1)

typedef struct stack {
    int array[MAX_STACK];
    int element_no;
} stack_s;

void init(stack_s *s) {
    s->element_no = 0;
}

void put(stack_s *s, int i) {
    s->array[s->element_no] = i;
    ++s->element_no;
}

int get(stack_s *s) {
    assert(s->element_no > 0);
    return s->array[--s->element_no];
}

int empty(stack_s *s) {
    return s->element_no == 0;
}

struct substraction_s {
    int address1;
    int address2;
};

struct jump_s {
    int address;
    char *label;
    int label_size;
};

struct call_s {
    char *label;
    int label_size;
};

struct return_s {
    char dummy; /*pedantic does not accept empty struct*/
};

struct read_s {
    int address;
};

struct write_s {
    int address;
};


typedef enum instructions_types {
    SUBSTRACTION,
    JUMP,
    CALL,
    RETURN,
    READ,
    WRITE
} INSTRUCTION;

typedef union instruction_u {
    struct substraction_s *substr;
    struct jump_s *jump;
    struct call_s *call;
    struct return_s *ret;
    struct read_s *read;
    struct write_s *write;
} instruction;

typedef struct instruction_element {
    INSTRUCTION i_t; /*typ instrukcji*/
    instruction i; /*instrukcja*/
} instruction_element;

struct label_instruction_pair {
    char *label;
    int label_size;
    int instruction;
};

int register_positive[MAX_ADDRESS];
int register_negative[MAX_ADDRESS];
instruction_element instruction_array[MAX_INSTRUCTION];
struct label_instruction_pair labels[MAX_LABEL];
int instruction_no;
int label_no;
stack_s stack;

int is_meaningful(char a) {
    if (a == '|') return 0;
    if (a == '\n') return 0;
    if (a == ' ') return 0;
    if (a == '\t') return 0;
    return 1;
}

void initialize() {
    instruction_no = 0;
    label_no = 0;
    for (int i = 0; i < MAX_ADDRESS; i++) {
        register_positive[i] = -i - 1;
        register_negative[i] = i;
    }
}

int read_address() {
    char my_string[INT_LENGTH];
    for(int i = 0; i < INT_LENGTH; i++){
        my_string[i] = 0;
    }
    int i = 0;
    char c = getchar();
    if (c != '-' && c != '+') {
        ungetc(c, stdin);
        c = 1;
    } else {
        if (c == '+')c = 1;
        if (c == '-')c = -1;
    }
    do {
        my_string[i] = getchar();
        ++i;
    } while (isdigit(my_string[i - 1]));
    ungetc(my_string[i - 1], stdin);
    my_string[i - 1] = 0;
    return atoi(my_string) * c;
}

char *read_label() {
    char my_string[LABEL_BUFFER];
    my_string[0] = 0;
    int i = 0;
    char c = getchar();
    while(!is_meaningful(c))c = getchar();
    ungetc(c,stdin);
    do {
        my_string[i] = getchar();
        ++i;
        my_string[i] = 0;
    } while (isalpha(my_string[i - 1]) || my_string[i - 1] == '_' || isdigit(my_string[i - 1]));
    ungetc(my_string[i - 1], stdin);
    my_string[i - 1] = 0;
    char *final_string = malloc(sizeof(char) * (i));
    strcpy(final_string, my_string);
    return final_string;
}

void read_getchar() {
    instruction instr;
    instr.read = malloc(sizeof(struct read_s));
    instr.read->address = read_address();
    instruction_array[instruction_no] = (instruction_element) {READ, instr};
    ++instruction_no;
}

void read_label_call() {
    instruction instr;
    instr.call = malloc(sizeof(struct call_s));
    instr.call->label = read_label();
    instr.call->label_size = strlen(instr.call->label);
    instruction_array[instruction_no] = (instruction_element) {CALL, instr};
    ++instruction_no;
}

void read_conditional_jump(int addr) {
    char *label = read_label();
    instruction instr;
    instr.jump = malloc(sizeof(struct jump_s));
    instr.jump->address = addr;
    instr.jump->label = label;
    instr.jump->label_size = strlen(label);
    instruction_array[instruction_no] = (instruction_element) {JUMP, instr};
    ++instruction_no;
}

void read_putchar(int addr) {
    instruction instr;
    instr.write = malloc(sizeof(struct write_s));
    instr.write->address = addr;
    instruction_array[instruction_no] = (instruction_element) {WRITE, instr};
    ++instruction_no;
}

void read_substraction(int addr) {
    instruction instr;
    instr.substr = malloc(sizeof(struct substraction_s));
    instr.substr->address1 = addr;
    instr.substr->address2 = read_address();
    instruction_array[instruction_no] = (instruction_element) {SUBSTRACTION,
                                                               instr};
    ++instruction_no;
}

void read_address_call() {
    int addr;
    addr = read_address();
    char c = getchar();
    while (!is_meaningful(c)) {
        c = getchar();
    }
    if (isalpha(c)) {
        ungetc(c, stdin);
        read_conditional_jump(addr);
    }
    if (c == '^')read_putchar(addr);
    if (isdigit(c) || c == '-' || c == '+') {
        ungetc(c, stdin);
        read_substraction(addr);
    }
}

void put_checkpoint(char *label, int no) {
    struct label_instruction_pair pair;
    pair.instruction = no - 1;
    pair.label = label;
    pair.label_size = strlen(label);
    labels[label_no] = pair;
    ++label_no;
}

void read_label_checkpoint() {
    put_checkpoint(read_label(), instruction_no);
}

void read_return() {
    instruction instr;
    instr.ret = malloc(sizeof(struct return_s));
    instruction_array[instruction_no] = (instruction_element) {RETURN,
                                                               instr};
    ++instruction_no;
}

void parse() {
    char c = getchar();
    while (c != EOF && c != '&') {
        if (is_meaningful(c) == 1) {
            if (c == '^') {
                read_getchar();
            }
            if (isalpha(c) || c == '_') {
                ungetc(c, stdin);
                read_label_call();
            }
            if (isdigit(c) || c == '-' || c == '+') {
                ungetc(c, stdin);
                read_address_call();
            }
            if (c == ':') {
                read_label_checkpoint();
            }
            if (c == ';')read_return();
        }
        c = getchar();
    }
}

int find_instruction_no(char *label) {
    for (int i = 0; i < label_no; i++) {
        if (strcmp(labels[i].label, label) == 0) return labels[i].instruction;
    }
    assert(1 == 0);
}

int move_to(char label[]) {
    return find_instruction_no(label);
}

int get_register(int address) {
    if (address > 5000 || address < -5000)return ((-1) * address - 1);
    if (address >= 0)return register_positive[address];
    return register_negative[(-1) * address - 1];
}

void set_register(int address, int value) {
    if (address >= 0)register_positive[address] = value;
    else register_negative[(-1) * address - 1] = value;
}

void substraction_f(struct substraction_s *s) {
    set_register(get_register(s->address1),
                 get_register(get_register(s->address1)) -
                 get_register(get_register(s->address2)));
}

int jump_f(struct jump_s *s, int i) {
    if (get_register(get_register(s->address)) > 0) {
        return move_to(s->label);
    }
    return i;
}

int call_f(struct call_s *s, int i) {
    put(&stack, i);
    return move_to(s->label);
}

int return_f() {
    if (empty(&stack))return END;
    return get(&stack);
}

void read_f(struct read_s *s) {
    int c;
    c = getchar();
    if (c == EOF)c = -1;
    set_register(get_register(s->address), c);
}

void write_f(struct write_s *s) {
    putchar(get_register(get_register(s->address)));
}

void run() {
    int i = 0;
    init(&stack);
    instruction_element instr;
    int a;
    while (i < instruction_no) {
        instr = instruction_array[i];
        switch (instr.i_t) {
            case READ:
                a = getchar();
                ungetc(a, stdin);
                read_f(instr.i.read);
                break;
            case WRITE:
                write_f(instr.i.write);
                break;
            case JUMP:
                i = jump_f(instr.i.jump, i);
                break;
            case RETURN:
                i = return_f();
                if (i == END)return;
                break;
            case CALL:
                i = call_f(instr.i.call, i);
                break;
            case SUBSTRACTION:
                substraction_f(instr.i.substr);
                break;
        }
        ++i;
    }
}

void destroy_instructions() {
    instruction_element instr;
    for (int i = 0; i < instruction_no; ++i) {
        instr = instruction_array[i];
        switch (instr.i_t) {
            case READ:
                free(instr.i.read);
                break;
            case WRITE:
                free(instr.i.write);
                break;
            case JUMP:
                free(instr.i.jump->label);
                free(instr.i.jump);
                break;
            case RETURN:
                free(instr.i.ret);
                break;
            case CALL:
                free(instr.i.call->label);
                free(instr.i.call);
                break;
            case SUBSTRACTION:
                free(instr.i.substr);
                break;
        }
    }
}

void destroy_labels() {
    for (int i = 0; i < label_no; ++i) {
        free(labels[i].label);
    }
}

void destroy() {
    destroy_instructions();
    destroy_labels();
}

int main() {
    initialize();
    parse();
    run();
    destroy();
    return 0;
}