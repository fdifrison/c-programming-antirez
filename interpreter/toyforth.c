// pt1 https://youtu.be/vYODKK8TQGE?list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&t=732
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TF_OK 0
#define TF_ERR 1

#define TFOBJ_TYPE_INT 0
#define TFOBJ_TYPE_STR 1
#define TFOBJ_TYPE_BOOL 2
#define TFOBJ_TYPE_LIST 3
#define TFOBJ_TYPE_SYMB 4
// #define TFOBJ_TYPE_ 3


/* ============ Data Structures */

// base toy forth object
typedef struct tfobj {
    int refcount;
    int type; // TFOBJ_TYPE_*

    union {
        int i;

        // for strings we wont the pointer and the length
        struct {
            char *ptr;
            size_t len;
        } str;

        struct {
            // a pointer to an array of pointers to tfobj elements
            struct tfobj **elements;
            size_t len;
        } list;
    };
} tfobj;

void printTfObj(tfobj *o) {
    switch (o->type) {
        case TFOBJ_TYPE_INT:
            printf("%d", o->i);
            break;
        case TFOBJ_TYPE_SYMB:
            printf("%s", o->str.ptr);
            break;
        case TFOBJ_TYPE_LIST:
            printf("[");
            for (size_t i = 0; i < o->list.len; i++) {
                tfobj *item = o->list.elements[i];
                printTfObj(item);
                if (i != o->list.len - 1) printf(" ");
            }
            printf("]\n");
            break;
        default:
            printf("??");
            break;
    }
};

struct tfctx;

typedef struct FunctionTableEntry {
    tfobj *name;

    int (*callback)(struct tfctx *ctx, char *name);

    // user defined functions
    tfobj *user_func;
} FuncEntry;

typedef struct FunctionTable {
    FuncEntry **table;
    size_t count;
} FuncTable;

typedef struct tfctx {
    tfobj *stack;
    FuncTable *func_table;
} tfctx;

void printStack(tfctx *ctx) {
    printf("Currently in stack:");
    printTfObj(ctx->stack);
};

tfobj *listPop(tfobj *list);

tfobj *stackPop(tfctx *ctx) {
    tfobj *stack = ctx->stack;
    assert(stack->type == TFOBJ_TYPE_LIST);
    return listPop(stack);
}

/* ============ Allocation wrappers */

/* a wrapper around oom issues with malloc*/
void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        fprintf(stderr, "Out of Memory allocating %zu bytes\n", size);
        exit(-1);
    }
    return p;
}

void *xrealloc(void *oldptr, size_t size) {
    void *p = realloc(oldptr, size);
    if (!p) {
        fprintf(stderr, "Out of Memory allocating %zu bytes\n", size);
        exit(-1);
    }
    return p;
};

void release(tfobj *o) {
#ifdef DEBUG
    printf("Releasing %p\n", o);
#endif
    assert(o->refcount > 0);
    o->refcount--;
    if (o->refcount == 0) free(o);
}

void freeObject(tfobj *o) {
    switch (o->type) {
        case TFOBJ_TYPE_LIST:
            printf("[");
            for (size_t i = 0; i < o->list.len; i++) {
                tfobj *item = o->list.elements[i];
                release(item);
                if (i != o->list.len - 1) printf(" ");
            }
            printf("]\n");
            break;
        case TFOBJ_TYPE_SYMB:
        case TFOBJ_TYPE_STR:
            free(o->str.ptr);
            break;
        default: ;
    }
    free(o);
};


void retain(tfobj *o) {
    o->refcount++;
}


/* ============ Object related functions */

/* Allocate and initialize a new Toy Forth object */
tfobj *createObject(int type) {
    tfobj *o = xmalloc(sizeof(tfobj));
    o->refcount = 1;
    o->type = type;
    return o;
}

tfobj *createStringObject(char *s, size_t len) {
    tfobj *o = createObject(TFOBJ_TYPE_STR);
    o->str.ptr = xmalloc(len + 1);
    o->str.len = len;
    memcpy(o->str.ptr, s, len);
    o->str.ptr[len] = 0;
    return o;
}

/* return 0 il string are the same, 1 otherwise*/
int compareStringObject(tfobj *a, tfobj *b) {
    if (a->str.len != b->str.len) return 1;
    return memcmp(a->str.ptr, b->str.ptr, b->str.len);
}

tfobj *createSymbObject(char *s, size_t len) {
    tfobj *o = createStringObject(s, len);
    o->type = TFOBJ_TYPE_SYMB;
    return o;
}


tfobj *createIntObject(int value) {
    tfobj *o = createObject(TFOBJ_TYPE_INT);
    o->i = value;
    return o;
}

tfobj *createBoolObject(int value) {
    tfobj *o = createIntObject(value);
    o->type = TFOBJ_TYPE_BOOL;
    return o;
}


/* ============ List Object */
tfobj *createListObject() {
    tfobj *o = createObject(TFOBJ_TYPE_LIST);
    o->list.len = 0;
    o->list.elements = NULL;
    return o;
}

/*push an element to a list. the ref count increment is demanded to the client*/
void listPush(tfobj *l, tfobj *element) {
    size_t current_len = l->list.len;
    l->list.elements = xrealloc(l->list.elements, sizeof(tfobj *) * (current_len + 1));
    l->list.elements[current_len] = element;
    l->list.len++;
}

tfobj *listPop(tfobj *l) {
    size_t current_len = l->list.len;
    if (current_len == 0) return NULL;
    tfobj *pop = l->list.elements[current_len - 1];
    if (l->list.len > 1) {
        l->list.elements = xrealloc(l->list.elements, sizeof(tfobj *) * (current_len - 1));
    } else {
        free(l->list.elements);
        l->list.elements = NULL;
    }
    l->list.len--;
    return pop;
}

/* ============ Parser */

typedef struct tfparser {
    char *prg; // program (string) to parse
    char *p; // next token to parse (where we are currently in the parsing)
} tfparser;

#define MAX_INT_LEN 128

tfobj *parseInt(tfparser *parser) {
    char buf[MAX_INT_LEN];
    char *start = parser->p;
    if (*parser->p == '-') parser->p++;
    while (*parser->p && isdigit(*parser->p)) parser->p++;
    // pointers are numbers and i can subtract them to know the space the number occupies
    int numlen = parser->p - start;
    if (numlen > MAX_INT_LEN) return NULL;
    memcpy(buf, start, numlen);
    buf[numlen] = 0; // null term at the end
    return createIntObject(atoi(buf));
}

int is_valid_symbol(int c) {
    char symbols[] = "+-/*%";
    if (isalpha(c)) return 1;
    if (c == 0) return 0;
    if (strchr(symbols, c) != NULL) return 1;
    return 0;
}

tfobj *parseSymbol(tfparser *parser) {
    char *start = parser->p;
    while (parser->p[0] && is_valid_symbol(*parser->p)) parser->p++;
    int numlen = parser->p - start;
    return createSymbObject(start, numlen);
};

/* ============ Compile */
/**
 * The act of compilation is actually turning the input text into a list of tfobj
 */
tfobj *compile(char *prg) {
#ifdef DEBUG
    printf("Beginning compilation\n");
#endif
    // we need to parse the text
    tfparser parser;
    // initialization
    parser.prg = prg;
    parser.p = prg;

    tfobj *tokens = createListObject();

    char *token_start = parser.p;
    // loop until i find the null terminator
    //  each case is responsible for advancing the pointer
    while (parser.p) {
        printf("parsing \"%c\"\n", parser.p[0]);
        if (*parser.p == 0) break;
        tfobj *o;
        if (isspace(*parser.p)) {
            parser.p++;
            continue ;
        }
        if (isdigit(*parser.p) || (*parser.p == '-' && isdigit(parser.p[1]))) {
            o = parseInt(&parser);
        } else if (is_valid_symbol(*parser.p)) {
            o = parseSymbol(&parser);
        } else {
            o = NULL;
        }
        if (o == NULL) {
            printf("Error compiling %s\n", parser.p);
            release(tokens);
            printf("Unknow token at %s \n", token_start);
            parser.p++;
        } else {
            listPush(tokens, o);
            o->refcount++;
        }
    }
    printf("Finished compilation\n");
    return tokens;
}

/* ============ Function Table */

FuncEntry *getFunctionByName(tfctx *ctx, tfobj *name) {
    for (size_t i = 0; i < ctx->func_table->count; i++) {
        FuncEntry *fn = ctx->func_table->table[i];
        if (compareStringObject(fn->name, name) == 0) return fn;
    }
    return NULL;
}

/*Push a function entry into the ctx demanding toi the user to either set
 *a C callback or a list representing a user defined function*/
FuncEntry *registerUserFunction(tfctx *ctx, tfobj *name) {
    ctx->func_table->table = xrealloc(ctx->func_table->table,
                                      sizeof(FuncEntry *) * (ctx->func_table->count + 1));
    FuncEntry *new_fn = xmalloc(sizeof(FuncEntry));
    ctx->func_table->table[ctx->func_table->count] = new_fn;
    ctx->func_table->count++;
    new_fn->name = name;
    retain(name);
    new_fn->callback = NULL;
    new_fn->user_func = NULL;
    return new_fn;
};

/*Register a new function with a given name. if a name already exist it gets replaced*/
void registerCFunction(tfctx *ctx, char *name,
                       int (*func_callback)(tfctx *ctx, char *name)) {
    printf("Registering function %s\n", name);
    FuncEntry *fn;
    tfobj *func_name = createStringObject(name, strlen(name));
    fn = getFunctionByName(ctx, func_name);
    if (fn) {
        if (fn->user_func) {
            release(fn->user_func);
            fn->user_func = NULL;
        }
        fn->callback = func_callback;
    } else {
        fn = registerUserFunction(ctx, func_name);
        fn->callback = func_callback;
    }
    release(func_name);
}

int basicMathFunction(tfctx *ctx, char *name) {
    tfobj *a = listPop(ctx->stack);
    tfobj *b = listPop(ctx->stack);
    printf("Executing: %d %s %d\n", b->i, &name[0], a->i);

    int result;
    switch (name[0]) {
        case '+': result = b->i + a->i;
            break;
        case '*': result = b->i * a->i;
            break;
        case '-': result = b->i - a->i;
            break;
        default: result = TF_ERR;
            printf("Unknown function name\n");
            return result;
            break;
    }
    release(a);
    release(b);

    listPush(ctx->stack, createIntObject(result));
    return TF_OK;
};

int symbFunction(tfctx *ctx, char *name) {
    if (strcmp(name, "print") == 0) {
        printf("Result: \"%d\"\n", listPop(ctx->stack)->i);
    } else {
        return TF_ERR;
    }
    return TF_OK;
};

void *createContext(void) {
    printf("Creating context\n");
    tfctx *ctx = xmalloc(sizeof(*ctx));
    ctx->stack = createListObject();
    ctx->func_table = xmalloc(sizeof(*ctx->func_table));
    ctx->func_table->table = NULL;
    ctx->func_table->count = 0;
    registerCFunction(ctx, "+", basicMathFunction);
    registerCFunction(ctx, "*", basicMathFunction);
    registerCFunction(ctx, "-", basicMathFunction);
    registerCFunction(ctx, "print", symbFunction);
    return ctx;
}


/* ============ Execute */

/*Tries to resolve the symb associated to the current token and apply it on the currest stack state*/
int execSymbol(tfctx *ctx, tfobj *token) {
    printf("Looking for function \"%s\"\n", token->str.ptr);
    FuncEntry *func = getFunctionByName(ctx, token);
    if (func == NULL) {
        printf("Function not found\n");
        return TF_ERR;
    }
    if (func->user_func) {
        // TODO implement
        return TF_ERR;
    }

    printf("Function \"%s\" found in table\n", func->name->str.ptr);
    return func->callback(ctx, token->str.ptr);
}


int exec(tfctx *ctx, tfobj *prg) {
#ifdef DEBUG
    printf("Executing :");
    printTfObj(prg);
    printf("\n");
#endif
    // invariant of our system, our prg has to be a list
    assert(prg != NULL && prg->type == TFOBJ_TYPE_LIST);
    for (size_t i = 0; i < prg->list.len; i++) {
        tfobj *token = prg->list.elements[i];
        switch (token->type) {
            case TFOBJ_TYPE_SYMB:
                if (execSymbol(ctx, token) == TF_ERR) {
                    perror("Run time error");
                    return TF_ERR;
                };
                retain(token);
                break;
            //all the other token type except of lists ans symb are pushed to the stack
            default:
                listPush(ctx->stack, token);
                retain(token);
                break;
        }
#ifdef DEBUG
        printStack(ctx);
#endif
    }
    return TF_OK;
}


/* ============ Main */


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n>", argv[0]);
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("File not found");
        return 1;
    }
    // move the cursor to end file from 0 position
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    // now that we know the file size we can allocate the required memory
#ifdef DEBUG
    printf("The File is %ld bytes\n", file_size);
#endif
    char *prgtext = xmalloc(file_size + 1); // +1 for the null term
    // return the pointer to the beginning of the file
    fseek(f, 0, SEEK_SET);
    fread(prgtext, file_size, 1, f);
    prgtext[file_size] = 0;
#ifdef DEBUG
    for (int i = 0; i < file_size; i++) {
        putchar(prgtext[i]);
    }
    printf("\n");
#endif

    tfobj *prg = compile(prgtext);
    // print(prg);
    tfctx *ctx = createContext();
    int result = exec(ctx, prg);

    if (result == TF_ERR) {
        perror("Run time error");
    }

    fclose(f);

    return 0;
}
