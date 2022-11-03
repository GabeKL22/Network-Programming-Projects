
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int x;


/*
Function name: check_input
Description:   check the command line arguments (if between 1-10 and only one argument)
Parameters:    int num: amount of command line arguments
               char *a: amount of processes
Return Value:  amt: amount of processes to create
*/
int check_input(int num, char *a) {

    int amt = atoi(a);
    //check if between 1 and 10
    if (amt < 1 || amt > 10) {
        printf("Number must be between 1 and 10.\n");
    }
    //check command line arguments
    if (num == 2) {
        return amt;
    } else if (num > 2) {
        printf("Too many arguments.\n");
    } else {
        printf("Please enter a number of processes 1-10.\n");
    }

    return amt;
}

/*
Function name: child
Description:   print output when child is executed, increment variables
Parameters:    int num: variable to increment
               int *nump: pointer variable to increment
               int i: the current process index
Return Value:  none - void function
*/
void child(int num, int *nump, int i) {
    printf("  BEFORE increment: Child number %d with PID of %d: x = %d, num = %d, nump = %d\n", i+1, getpid(), x, num, *nump);
    x = x + 50;
    num++;
    *nump=(*nump)+1;
    //sleep(1);
    printf("  --AFTER increment: Child number %d with PID of %d: x = %d, num = %d, nump = %d\n", i+1, getpid(), x, num, *nump);

}
/*
Function name: parent
Description:   waits for child to finish, then prints when it has ended
Parameters:    int status: unassigned variable (NULL)
Return Value:  none - void function 
*/
void parent(int status) {
    int rv = wait(&status); //wait for child
    if (rv == -1) {
        perror("wait");
        exit(1);
    }
    printf("    [Child PID: %d has ended.]\n", rv);
}
/*
Function name: main
Description:   call functions, and initialize variables, also start
               a certain number of sequential, and concurrent processes
Parameters:    int argc: amount of command line arguments
               char *argv[]: charecter array of inputs 
Return Value:  0
*/
int main (int argc, char *argv[]) {

    //create varibles to increment
    int numProcs = check_input(argc, argv[1]);
    x = 500;
    int num = 5;
    int *nump = (int *)malloc(sizeof(int));
    *nump = 10;
    int status;

    printf("\nThe program is creating sequential processes -- numProcs: %d | x: %d | num: %d | nump: %d\n\n", numProcs, x, num, *nump);

    //create numProcs sequential processes
    for (int i = 0; i < numProcs; i++) {

        int pid = fork(); // call fork pid is processID

        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            child(num, nump, i); //call child
            exit(0);
        }
        parent(status); //call parent function
    }

    //reset
    x = 500;
    num = 5;
    *nump = 10;

    //create numProcs concurrent processes
    printf("\nThe program is creating concurrent processes -- numProcs: %d | x: %d | num: %d | nump: %d\n\n", numProcs, x, num, *nump);
    for (int i = 0; i < numProcs; i++){

        int pid = fork(); // call fork pid is processID

        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            child(num, nump, i); // call child 
            sleep(1);
            exit(0);
        }
    }

    for (; numProcs > 0; numProcs--) {
        parent(status); //call parents
    }

    printf("Program finished.\n");

    return 0;
}
