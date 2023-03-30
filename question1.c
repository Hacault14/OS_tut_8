#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

struct proc {
    char parent[256];
    char name[256];
    int priority;
    int memory;
};

struct proc_tree {
    struct proc;
    int key_value;
    struct proc_tree *left;
    struct proc_tree *right;
};

struct proc_tree *search(int key, struct proc_tree *leaf) {
    if (leaf != 0) {
        if(key==leaf->key_value)
        {
            return leaf;
        }
        else if(key<leaf->key_value)
        {
            return search(key, leaf->left);
        }
        else
        {
            return search(key, leaf->right);
        }
    }
    else return 0;
}

insert(int key, struct proc_tree **leaf) {
    if (*leaf == 0)
    {
        *leaf = (struct proc_tree*) malloc(sizeof(struct proc_tree));
        (*leaf)->key_value = key;
        (*leaf)->left = 0;    
        (*leaf)->right = 0;  
    }
    else if(key < (*leaf)->key_value)
    {
        insert(key, &(*leaf)->left);
    }
    else if(key > (*leaf)->key_value)
    {
        insert(key, &(*leaf)->right);
    }
}

int main (void) {
    struct proc proc1, proc2, proc3, proc4, proc5, proc6, proc7;
    struct proc_tree tree1;
    FILE *file = fopen("process_tree.txt", "r");
    char input[30];
    while(fscanf(file,"%s",input) != EOF){
        char *token;
        token = strtok(input,",");
        int loop = 0;
        while (token != NULL) {
            if (loop == 0){
                strcpy(proc1.parent,token);
                loop++;
            }
            else if (loop == 1){
                strcpy(proc1.name,token);
                loop++;
            }
            else if (loop == 2){
                proc1.priority = atoi(token);
                loop++;
            }
            else if (loop == 3){
                proc1.memory = atoi(token);
                loop++;
            }
            token = strtok(NULL, ",");
        }
        insert(0,proc1.parent);
        insert(0,proc1.name);
        insert(0,proc1.priority);
        insert(0,proc1.memory);
    }
}