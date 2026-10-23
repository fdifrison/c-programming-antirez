// Binary Search tree basic implementation
// https://www.youtube.com/watch?v=soiBgJjAmP8&list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&index=19


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct node {
    int val;
    // left node has lower value, right node has higher value
    struct node *left, *right;
};


struct node *find_node_location(struct node *root, struct node *new) {
    struct node *rel_top = root;
    while (1) {
        if (rel_top->val > new->val) {
            if (rel_top->left == NULL) {
                rel_top->left = new;
                return root;
            }
            rel_top = rel_top->left;
        } else {
            if (rel_top->right == NULL) {
                rel_top->right = new;
                return root;
            }
            rel_top = rel_top->right;
        }
    }
}


/**
 * Add a node to the bst, if root is null, will add the root
 */
struct node *add(struct node *root, int val) {
    struct node *new = malloc(sizeof(*new));
    new->val = val;
    new->left = new->right = NULL;
    // if there is no root the new node become the root
    if (root == NULL) return new;
    // otherwise ned to find the correct position in the tree
    return find_node_location(root, new);
}

void print_sorted(struct node *root) {
    if (root == NULL) return;
    print_sorted(root->left); // values smaller than the root
    printf("%d ", root->val);
    print_sorted(root->right); // values bigger than the root
}

int main() {
    struct node *root = NULL;
    root = add(root, 10);
    add(root, 5);
    add(root, 20);
    add(root, 100);
    add(root, 40);
    add(root, 33);
    print_sorted(root);
    return 0;
}
