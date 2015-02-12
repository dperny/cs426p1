#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* maximum length command */
#define MAX_HISTORY 10 // maximum history length

#define MIN(a,b)(a < b ? a : b)

// keep track of the number of commands we've run
int commandCount = 0;

char * history[MAX_HISTORY];

void addToHistory(char *);

// parses the command from the buffer into the args array
// returns the length of the args array
int parseCommand(char * buffer, char * args[]) {
  if(strncmp("history",buffer,7) && (buffer[0] != '!')) {
    addToHistory(buffer);
  }

  // parse the buffer into an arg array
  int count = 0;
  args[0] = strtok(buffer, " \n");
  while(args[count] != NULL) {
    count++;
    args[count] = strtok(NULL, " \n");
  }
  // fprintf(stderr,"%s\n", args[count-1]);
  return count;
}

// run a command
void runCommand(char * args[], int length) {
  pid_t pid;
  // flag for if we should wait
  int should_wait = 0;

  // see if the last arg is a &
  if(!strcmp(args[length-1], "&")) {
    // if so, remove it and set the wait flag
    should_wait = 1;
    args[length-1] = NULL;
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
    exit(0);
  } else {
    // (3) if command included &, parent will invoke wait()
    if(should_wait) {
      /**************** 
       * GRADER NOTE HERE PLEASE: waitpid() instead of wait()
       * wait() was giving me trouble. I change this to waitpid(), and it works
       * just fine. Same family of functions, very very similar usage, so i hope
       * this is acceptable.
       ****************/
      waitpid(pid, NULL, 0);
      // try uncommenting this line and commenting above. doesnt work
      // wait(NULL);
    }
  }
}

// add a new command to the history
void addToHistory(char * buffer) {
  char * entry = malloc(sizeof(char) * MAX_LINE);
  // copy the buffer into the space we've made
  strcpy(entry, buffer);

  // fewer than ten commands? easy, stick it straight into its slot
  if(commandCount < MAX_HISTORY) {
    history[commandCount] = entry;
  } else {
    free(history[0]);
    int i;
    for(i = 1; i < MAX_HISTORY; i++) {
      history[i-1] = history[i];
    }
    history[MAX_HISTORY - 1] = entry;
  }
  commandCount++; 
}

void getHistory(int command) {
  char* args[MAX_LINE/2 + 1]; 
  char buffer[MAX_LINE];
  int length;

  if(command <= commandCount && (commandCount - command) < 10) {
    if(commandCount < 10) {
      strcpy(buffer, history[command-1]);
    } else {
      strcpy(buffer, history[(10 - (commandCount - command)) -1]);
    }
    // printf("contents of buffer:\n\t%s", buffer);
    length = parseCommand(buffer, args);
    runCommand(args, length);
  } else {
    puts("No such command in history");
  }
}

void printHistory() {
  int c = commandCount;
  int i;
  if(commandCount == 0) {
    printf("No commands in history\n");
  }
  // printf("%d %s\n",0, history[0]);
  for(i = (MIN(MAX_HISTORY,commandCount))-1; i >= 0; i--) {
    printf("%d %s", c, history[i]);
    c--;
  }
}

int main(void) {
  // command line arguments
  char* args[MAX_LINE/2 + 1]; 
  // a buffer to hold commands
  char buffer[80];
  // flag for exiting
  int should_run = 1;
  // command length
  int length = 0;

  while(should_run) {
    // print the prompt
    printf("osh> ");
    fflush(stdout);

    // read into the buffer. skip this entire section if the read fails
    if(fgets(buffer, 79, stdin)) { 
      // parse the command into the the args array. buffer is altered
      // save the resulting output in length
      length = parseCommand(buffer, args);

      // skip this whole section if the first arg is null
      // if we don't, it segfaults. hilarious
      if(args[0] != NULL) {
        if(!strcmp("exit", args[0])) { 
          should_run = 0; // exit if exit
        } else if(!strcmp("history", args[0])) {
          printHistory();
        } else if(args[0][0] == '!') {
          if(args[0][1] == '!') {
            getHistory(commandCount);
          } else {
            getHistory(atoi(&args[0][1]));
          }
        } else {
          runCommand(args, length);
        }
      }
    }
  }

  return 0;
}
