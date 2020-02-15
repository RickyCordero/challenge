#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "svec.h"
#include "tokenize.h"
#include "cmd_ast.h"
#include "parse.h"

void
execute(char* cmd)
{
    int cpid;

    if ((cpid = fork())) {
        // parent process
        printf("Parent pid: %d\n", getpid());
        printf("Parent knows child pid: %d\n", cpid);

        // Child may still be running until we wait.

        int status;
        waitpid(cpid, &status, 0);

        printf("== executed program complete ==\n");

        printf("child returned with wait code %d\n", status);
        if (WIFEXITED(status)) {
            printf("child exited with exit code (or main returned) %d\n", WEXITSTATUS(status));
        }
    }
    else {
        // child process
        printf("Child pid: %d\n", getpid());
        printf("Child knows parent pid: %d\n", getppid());

        for (int ii = 0; ii < strlen(cmd); ++ii) {
	    //if (cmd[ii] == ' ') {
            if (cmd[ii] == '\n') {
                cmd[ii] = 0;
                break;
            }
        }

        // The argv array for the child.
        // Terminated by a null pointer.
        char* args[] = {cmd, "one", 0};

        printf("== executed program's output: ==\n");

        execvp(cmd, args);
        printf("Can't get here, exec only returns on error.");
    }
}

int
main(int argc, char* argv[])
{
    char cmd[256];
    while(1) {
        printf("nush$ ");
        fflush(stdout);
        char* rv = fgets(cmd, 256, stdin);
	if(!rv){
		break;
	}
	svec* tokens = tokenize(cmd);
	//svec_rev(tokens);
	//svec_print(tokens);
	char* token_string = svec_to_string(tokens);
	printf("%s\n", token_string);

	//cmd_ast* cmd_ast = parse(tokens);
	//cmd_ast_print(cmd_ast);
	
    }

    //execute(cmd);

    return 0;
}
