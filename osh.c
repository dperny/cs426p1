#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* maximum length command */


void runCommand(char * args[], int last) {
  pid_t pid;
  // flag for if we should wait
  int should_wait = 0;

  // addToHistory(args, length);

  // see if the last arg is a &
  if(!strcmp(args[last-1], "&")) {
    // if so, remove it and set the wait flag
    should_wait = 1;
    args[last-1] = NULL;
  } else {
    // otherwise, do not wait. just go
    should_wait = 0;
  }

  // After reading user input, the steps are:
  // (1) fork a child process using fork()
  pid = fork();
  if(!pid) {
    // (2) the child process will invoke execvp()
    execvp(args[0], args);
  } else {
    // (3) if command included &, parent will invoke wait()
    if(should_wait) {
      wait(NULL);
    }
  }
}

int main(void) {
  // command line arguments
  char* args[MAX_LINE/2 + 1]; 
  // a buffer to hold commands
  char buffer[80];
  // flag for exiting
  int should_run = 1;

  while(should_run) {
    // print the prompt
    printf("osh> ");
    fflush(stdout);

    // read into the buffer. skip this entire section if the read fails
    if(fgets(buffer, 79, stdin)) { 
      
      // parse the buffer into an arg array
      int count = 0;
      args[0] = strtok(buffer, " \n");
      while(args[count] != NULL) {
        count++;
        args[count] = strtok(NULL, " \n");
      }
      
      // skip this whole section if the first arg is null
      // if we don't, it segfaults. hilarious
      if(args[0] != NULL) {
        if(!strcmp("exit", args[0])) should_run = 0; // exit if exit
        // if(!strcmp("history", args[0])) history();
        // if(args[0][0] == '!') execHistory(args, count);
        runCommand(args, count);
      }
    }
  }

  return 0;
}
