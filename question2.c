#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MEMORY 1024

struct proc {
    char name[256];
    int priority;
    pid_t pid;
    int address;
    int memory;
    int runtime;
    bool suspended;
};

struct node {
    struct proc process;
    struct node *next;
};

struct queue {
    struct node *front;
    struct node *rear;
};

int avail_mem[MEMORY] = {0};

void push(struct queue *q, struct proc process) {
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    new_node->process = process;
    new_node->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = new_node;
        return;
    }
    q->rear->next = new_node;
    q->rear = new_node;
}

struct proc pop(struct queue *q) {
    if (q->front == NULL) {
        struct proc empty_process;
        empty_process.pid = -1;
        return empty_process;
    }
    struct node *temp = q->front;
    struct proc process = temp->process;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    free(temp);
    return process;
}

void execute_process(struct proc process, struct queue *secondary_queue) {
    printf("Executing %s with priority %d, pid %d, memory %d, and runtime %d seconds\n",
        process.name, process.priority, process.pid, process.memory, process.runtime);
    process.pid = fork();
    if (process.pid == -1) {
        fprintf(stderr, "Failed to fork process %s\n", process.name);
        return;
    }
    else if (process.pid == 0) {
        char memory[16];
        sprintf(memory, "%d", process.memory);
        execl(process.name, process.name, memory, NULL);
        fprintf(stderr, "Failed to execute process %s\n", process.name);
        exit(1);
    }
    else {
        process.suspended = false;
        int i, count = 0;
        for (i = process.address; i < process.address + process.memory; i++) {
            avail_mem[i] = 1;
            count++;
        }
        printf("Allocated %d bytes of memory starting at address %d\n", count, process.address);
    }
    while (process.runtime > 0) {
        sleep(1);
        process.runtime--;
        if (process.suspended) {
            printf("Process %s with pid %d has been suspended\n", process.name, process.pid);
            kill(process.pid, SIGCONT);
            process.suspended = false;
        }
        if (process.runtime == 1) {
            printf("Process %s with pid %d has 1 second left to run\n", process.name, process.pid);
        }
    }
    kill(process.pid, SIGTSTP);
    int status;
    waitpid(process.pid, &status, 0);
    printf("Process %s with pid %d has terminated with status %d\n", process.name, process.pid, status);
    int i, count = 0;
    for (i = process.address; i < process.address + process.memory; i++) {
        avail_mem[i] = 0;
        count++;
    }
    printf("Freed %d bytes of memory starting at address %d\n", count, process.address);
    if (process.priority != 0) {
        push(&secondary, process);
        if (process.memory <= find_available_mem()) {
            int pid = fork();
            if (pid == -1) {
                perror("fork");
            } else if (pid == 0) {
                // Child process
                execute_process(&process);
                exit(EXIT_SUCCESS);
            } else {
                // Parent process
                process.pid = pid;
                mark_mem(process.address, process.memory);
                printf("Executing secondary priority process %s, priority %d, pid %d, memory %d, runtime %d\n",
                       process.name, process.priority, process.pid, process.memory, process.runtime);
                sleep(1); // Let the process run for 1 second before suspending it
                suspend_process(&process);
                process.runtime--;
                process.suspended = true;
                push(&secondary, process);
            }
        } else {
            printf("Not enough memory available for secondary priority process %s, priority %d, memory %d, runtime %d\n",
                   process.name, process.priority, process.memory, process.runtime);
            push(&secondary, process);
        }
    }
    while (!is_empty(&secondary)) {
        Process process = pop(&secondary);
        if (process.suspended && process.pid != 0) {
            resume_process(&process);
        }
        if (process.runtime > 0) {
            if (process.runtime == 1) {
                printf("Executing secondary priority process %s, priority %d, pid %d, memory %d, runtime %d\n",
                       process.name, process.priority, process.pid, process.memory, process.runtime);
                run_process(&process, SIGINT);
                waitpid(process.pid, NULL, 0);
                free_mem(process.address, process.memory);
            } else {
                printf("Executing secondary priority process %s, priority %d, pid %d, memory %d, runtime %d\n",
                       process.name, process.priority, process.pid, process.memory, process.runtime);
                run_process(&process, SIGTSTP);
                sleep(1); // Let the process run for 1 second before suspending it
                suspend_process(&process);
                process.runtime--;
                process.suspended = true;
                push(&secondary, process);
            }
        } else {
            free_mem(process.address, process.memory);
        }
    }
    free_queue(&priority);
    free_queue(&secondary);
}