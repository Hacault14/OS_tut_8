#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the process structure
struct proc {
    char parent[256];
    char name[256];
    int priority;
    int memory;
};

// Define the binary tree node structure
struct node {
    struct proc p;
    struct node *left;
    struct node *right;
};

// Insert a new node into the binary tree
void insert(struct node **root, struct proc p) {
    // If the root is empty, create a new node and insert the process data
    if (*root == NULL) {
        *root = (struct node*) malloc(sizeof(struct node));
        (*root)->p = p;
        (*root)->left = NULL;
        (*root)->right = NULL;
    }
    // Otherwise, compare the name of the process to the current node's name
    else {
        int cmp = strcmp(p.name, (*root)->p.name);
        // If the process name is less than the current node's name, insert into the left subtree
        if (cmp < 0) {
            insert(&((*root)->left), p);
        }
        // If the process name is greater than the current node's name, insert into the right subtree
        else if (cmp > 0) {
            insert(&((*root)->right), p);
        }
    }
}

// Traverse the binary tree in-order and print the process information
void inorder_traversal(struct node *root) {
    if (root != NULL) {
        inorder_traversal(root->left);
        printf("Parent: %s, Name: %s, Priority: %d, Memory: %d MB\n", root->p.parent, root->p.name, root->p.priority, root->p.memory);
        inorder_traversal(root->right);
    }
}

int main() {
    // Open the process_tree.txt file for reading
    FILE *fp = fopen("process_tree.txt", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // Initialize the root of the binary tree
    struct node *root = NULL;

    // Read each line of the file and create a new process node for each line
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        // Parse the comma-separated values in the line
        char parent[256];
        char name[256];
        int priority, memory;
        sscanf(line, "%[^,],%[^,],%d,%d", parent, name, &priority, &memory);

        // Create a new process structure with the parsed values
        struct proc p;
        strcpy(p.parent, parent);
        strcpy(p.name, name);
        p.priority = priority;
        p.memory = memory;

        // Insert the new process node into the binary tree
        insert(&root, p);
    }

    // Close the file
    fclose(fp);

    // Traverse the binary tree in-order and print the process information
    inorder_traversal(root);

    return 0;
}
