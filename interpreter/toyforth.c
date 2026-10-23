// pt1 https://youtu.be/vYODKK8TQGE?list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&t=732
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
#ifdef DEBUG
    printf("List len is %lu\n", current_len + 1);
#endif
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
    if (strchr(symbols, c) != NULL) return 1;
    return 0;
}

tfobj *parseSymbol(tfparser *parser) {
    char *start = parser->p;
    while (parser->p && is_valid_symbol(*parser->p)) parser->p++;
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

    // loop until i find the null terminator
    while (parser.p) {
        tfobj *o;
        char *token_start = parser.p;
        if (*parser.p == 0) break;
        if (isspace(*parser.p)) continue;
        if (isdigit(*parser.p) || (*parser.p == '-' && isdigit(parser.p[1]))) {
            o = parseInt(&parser);
        } else if (is_valid_symbol(*parser.p)) {
            o = parseSymbol(&parser);
        } else {
            o = NULL;
        }
        if (o == NULL) {
            release(tokens);
            printf("Unknow token at %s \n", token_start);
        } else {
            listPush(tokens, o);
            o->refcount++;
        }

        parser.p++;
    }
    return tokens;
}

/* ============ Utilities */

void print(tfobj *o) {
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
                print(item);
                if (i != o->list.len - 1) printf(" ");
            }
            printf("]\n");
            break;
        default:
            printf("??");
            break;
    }
};

/* ============ Function Table */



typedef struct tfctx {
    tfobj *stack;
} tfctx;


struct FunctionTableEntry {
    tfobj *name;
    void (*callback)(tfctx *ctx, tfobj *);
};


/*Tries to resolve the symb associated to the current token and apply it on the currest stack state*/
void execSymbol(tfctx *ctx, tfobj *tfobj) {
};


/* ============ Execute */

void *createContext(void) {
    tfctx *ctx = xmalloc(sizeof(*ctx));
    ctx->stack = createListObject();
    return ctx;
}

void exec(tfctx *ctx, tfobj *prg) {
#ifdef DEBUG
    printf("Executing :");
    print(prg);
    printf("\n");
#endif
    // invariant of our system, our prg has to be a list
    assert(prg != NULL && prg->type == TFOBJ_TYPE_LIST);
    for (size_t i = 0; i < prg->list.len; i++) {
        tfobj *token = prg->list.elements[i];
        switch (token->type) {
            case TFOBJ_TYPE_SYMB:
                execSymbol(ctx, token);
                retain(token);
                break;
            //all the other token type except of lists ans symb are pushed to the stack
            default:
                listPush(ctx->stack, token);
                break;
        }
#ifdef DEBUG
        printf("Currently in stack:");
        print(ctx->stack);
        printf("\n");
#endif
    }
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
    exec(ctx, prg);


    fclose(f);

    return 0;
}
