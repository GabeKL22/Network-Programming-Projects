#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#define MAXLINE 512
#define READ      0
#define WRITE     1

/*
Function name: child
Description:   write to the parent process, and read from the 
               parent process.
Parameters:    char *line - malloc() to get a void pointer allocate on heap
               int p2c[] - parent to child file descripter
               int c2p[] - child to parent fd
Return Value:  none, void function 
*/
void child(char *line, int p2c[], int c2p[]) {
    close(p2c[WRITE]);
    close(c2p[READ]);
    int x, y;
    if (read(p2c[READ], &x, sizeof(x)) == -1) { 
        perror("Error reading from pipe in child.\n");
        exit(1); 
    }
    printf("Child received from pipe: %d\n", x);
    if (read(p2c[READ], &y, sizeof(x)) == -1) { 
        perror("Error reading from pipe in child.\n");
        exit(1); 
    }
    printf("Child received from pipe: %d\n", y);
    sleep(1);

    if (y == (x * getpid())) { // check if approved or denied
        if (write(c2p[WRITE], "Approved", MAXLINE) == -1) { 
            perror("Error writing to pipe in child.\n");
            exit(1); 
        }
        printf("Child sending to pipe: Approved\n");
    } else {
        if (write(c2p[WRITE], "Denied", MAXLINE) == -1) { 
            perror("Error writing to pipe in child.\n");
            exit(1); 
        }
        printf("Child sending to pipe: Denied\n");
    }
    
    if (read(p2c[READ], line, MAXLINE) == -1) { 
        perror("Error reading from pipe in child.\n");
        exit(1); 
    }
    printf("Child received from pipe: %s\n", line);
    close(p2c[READ]);
    close(c2p[WRITE]);
    if (line = "BYE") {
        exit(1);
    }
}
/*
Function name: parent
Description:   write to the child process, and read from the 
               child process.
Parameters:    char *line - malloc() to get a void pointer allocate on heap
               pid_t pid - process id
               int p2c[] - parent to child file descripter
               int c2p[] - child to parent fd
Return Value:  none, void function 
*/
void parent(char *line, pid_t pid, int p2c[], int c2p[]) {
    close(p2c[READ]);
    close(c2p[WRITE]);
    srand(time(NULL));
    int rand_num = rand() % 100;
    if (write(p2c[WRITE], &rand_num, sizeof(rand_num)) == -1) { 
        perror("Error writing to pipe from parent.\n");
        exit(1); 
    }
    printf("Parent sending to pipe: %d\n", rand_num);
    sleep(1); // sleep to send and receive first
    int product = rand_num * pid;
    if (write(p2c[WRITE], &product, sizeof(product)) == -1) { 
        perror("Error writing to pipe from parent.\n");
        exit(1); 
    }
    printf("Parent sending to pipe: %d\n", product);
    char size[10];
    if (read(c2p[READ], line, MAXLINE) == -1) { 
        perror("Error reading from pipe in child.\n");
        exit(1); 
    }
    printf("Parent received from pipe: %s\n", line);
    //determine output based on value from child
    if (line = "Approved") {
        printf("Parent: Thanks for playing!\n");
    } else {
        printf("Parent: Wrong. Please play again\n");
    }
    //send BYE
    if (write(p2c[WRITE], "BYE", MAXLINE) == -1) { 
        perror("Error writing to pipe from parent.\n");
        exit(1); 
    }
    printf("Parent sending to pipe: BYE\n");
    int rv = wait(NULL);
    if (rv == -1) {
      perror("Error in wait: ");
      exit(-1);
    }
    close(p2c[WRITE]);
    close(c2p[READ]);

}

/*
Function name: main
Description:   create twp pipes and fork() a process
               calling parent() and child() functions
Parameters:    int argc: amount of command line arguments
               char *argv[]: charecter array of inputs 
Return Value:  0
*/
int main(int argc, char* argv[]) {

    int p2c[2], c2p[2]; // two pipes, for each direction
    pid_t pid;
    char *line=(char *)malloc(sizeof(char)*MAXLINE);

    if (pipe(p2c) == -1) {  //parent to child
        perror("An error occured creating the parent to child pipe.\n");
        exit(1);
    }
    if (pipe(c2p) == -1) {  //child to parent
        perror("An error occured creating the child to parent pipe.\n");
        exit(1);
    }    
    if ((pid = fork()) < 0) {
        perror("An error occured creating a process with fork().\n");
    } else if (pid == 0) {  //do child stuff
        child(line, p2c, c2p);
    } else {  //do parent stuff
        parent(line, pid, p2c, c2p);
    }

    free(line); //free heap
    return 0;
}
