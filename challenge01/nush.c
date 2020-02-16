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
execute(svec* cmd)
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
	
	int bb = strlen(cmd->data[0]);
	char* cmd_root = malloc((bb+1)*sizeof(char));
	memcpy(cmd_root, cmd->data[0], bb);
	cmd_root[bb] = 0;

        // The argv array for the child.
        // Terminated by a null pointer.
        //char* args[] = {cmd, "one", 0};

	cmd->data[cmd->size] = 0;

        printf("== executed program's output: ==\n");

        int e = execvp(cmd_root, cmd->data);
	printf("%d\n", e);
	if (e==-1) {
		perror("execvp");
	}
    }
}

int
eval(cmd_ast* cmd_ast)
{
	if (strcmp(cmd_ast->op, "=") == 0) {
		if (!cmd_ast->cmd->data[0]) {
			// user hit enter key, no input
			return 0;
		}
		// base case: "cd ..."
		if (strcmp(svec_get(cmd_ast->cmd, 0),"cd") == 0) {
			//char* cmd_string = svec_to_string(cmd_ast->cmd);
			svec* path = slice(cmd_ast->cmd, 1, cmd_ast->cmd->size);
			char* path_string = svec_to_string(path);
			chdir(path_string);
			return 0;
		}
		// base case: "exit ..."
		if (strcmp(svec_get(cmd_ast->cmd, 0), "exit") == 0) {
			exit(0);
		}
		// base case: "command arg1 arg2 ..."
		// fork and exec
		execute(cmd_ast->cmd);
	} else {
		// "command1 OP command2"
		if (strcmp(cmd_ast->op, ";") == 0) {
			// "command1 ; command2"
			eval(cmd_ast->arg0);
			eval(cmd_ast->arg1);
			return 0;
		}
		if (strcmp(cmd_ast->op, "&&") == 0) {
			// "command1 && command2"
			int status = eval(cmd_ast->arg0);
			if (status == 0) {
				eval(cmd_ast->arg1);
			}
		}
		if (strcmp(cmd_ast->op, "||") == 0) {
			// "command1 || command2"
			int status = eval(cmd_ast->arg0);
			if (status != 0) {
				eval(cmd_ast->arg1);
			}
		}
		if (strcmp(cmd_ast->op, "&") == 0) {

			
			// "command1 &" or "command1 & command2" = "command1 &; command2"
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
			} else {
				// child process
			        printf("Child pid: %d\n", getpid());
			        printf("Child knows parent pid: %d\n", getppid());
				/*
	        		for (int ii = 0; ii < strlen(cmd); ++ii) {
			            if (cmd[ii] == ' ') {
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
				*/
			}
		}
		if (strcmp(cmd_ast->op, ">") == 0) {
			
		}
		if (strcmp(cmd_ast->op, "<") == 0) {
			
		}
		if (strcmp(cmd_ast->op, "|") == 0) {
				
		} 
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
	//svec_print(tokens);
	//char* token_string = svec_to_string(tokens);
	//printf("%s\n", token_string);

	cmd_ast* cmd_ast = parse(tokens);
	cmd_ast_print(cmd_ast);
	eval(cmd_ast);
    }
	printf("\n");

    return 0;
}
