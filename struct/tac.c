// reimplementing tac shell command (the inverse of cat)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct line {
    char *current_line;
    // a pointer to the next line
    struct line *next_line;
};


/**
 * @param argc number of arguments
 * @param argv is a pointer to an array of pointers of char arrays:
 * hence the pointer to the container of all the strings (char array) in the text
 * that we want to reverse. hence argv[0] will be a char*
 */
int main(int argc, char **argv) {
    // we want  2 arguments: the program name and the file to print
    if (argc <= 1) {
        printf("Missing filename\n");
        return 1;
    }

    // create a file pointer to the input argument that should be a file
    // FILE is a special struct defined with typedef to give it a reserved name
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        printf("File %s not found\n", argv[1]);
        return 1;
    }


    struct line *head = NULL;
    char buffer[1024];
    // how to parse the text of a file with the c library
    // we need to passa buffer: the number of bytes that we are going to parse
    // and the pointer to the file
    // when the parsing is finished the call to fgets will return null
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        struct line *l = malloc(sizeof(*l)); // 16byte, 8 for each pointers
        const size_t line_len = strlen(buffer);
        // the +1 is for the terminal operator
        l->current_line = malloc(line_len + 1);
        for (size_t i = 0; i <= line_len; i++) {
            // im copying 1 by 1 the byte of the buffer into my current line
            l->current_line[i] = buffer[i];
        }
        // im creating a linked list
        l->next_line = head;
        head = l;
    }

    fclose(fp);

    // now the linked list is full and we can travers it backwards
    while (head != NULL) {
        printf("%s", head->current_line);
        struct line *next = head->next_line; // save next node in a copy
        free(head->current_line); // free the current string
        free(head); // free the node
        head = next; // override the address in head with the previously saved pointer to the next node
    }


    return 0;
}
