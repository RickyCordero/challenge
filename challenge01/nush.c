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
#include <sys/stat.h>
#include <fcntl.h>
#include "hashmap.h"


int
execute(svec* cmd, hashmap* env)
{
    int cpid;

    if ((cpid = fork())) {
        // parent process
	//printf("Parent pid: %d\n", getpid());
	//printf("Parent knows child pid: %d\n", cpid);
	
        // Child may still be running until we wait.
       	int status;
	waitpid(cpid, &status, 0);
	
	/*
       	printf("== executed program complete ==\n");
        printf("child returned with wait code %d\n", status);
	
        if (WIFEXITED(status)) {
		printf("child exited with exit code (or main returned) %d\n", WEXITSTATUS(status));
        }
	*/
	return status;
    }
    else {
        // child process
        //printf("Child pid: %d\n", getpid());
        //printf("Child knows parent pid: %d\n", getppid());
	
	int bb = strlen(cmd->data[0]);
	char* cmd_root = malloc((bb+1)*sizeof(char));
	memcpy(cmd_root, cmd->data[0], bb);
	cmd_root[bb] = 0;

        // The argv array for the child.
        // Terminated by a null pointer.
	cmd->data[cmd->size] = 0;

        //printf("== executed program's output: ==\n");
	int e;
	if (env) {
		// TODO: Implement environment

		execvpe(cmd_root, cmd->data, 0);
	}
        e = execvp(cmd_root, cmd->data);
	if (e==-1) {
		//perror("failed to execute command");
		free(cmd_root);
		exit(1);
	}
	free(cmd_root);
    }
}

void
check_rv(int rv)
{
	if (rv < 0) {
		perror("pipe error");
		exit(1);
	}
}

int
eval(cmd_ast* cmd_ast, hashmap* env)
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
			return chdir(path_string);
		}
		// base case: "exit ..."
		if (strcmp(svec_get(cmd_ast->cmd, 0), "exit") == 0) {
			exit(0);
		}
		// base case: "command arg1 arg2 ..."
		// fork and exec
		return execute(cmd_ast->cmd, env);
	} else {
		// "command1 OP command2"
		if (strcmp(cmd_ast->op, ";") == 0) {
			// "command1 ; command2"
			int s1 = eval(cmd_ast->arg0, 0);
			int s2 = eval(cmd_ast->arg1, 0);
			if ((s1 == 0) && (s2 == 0)) {
				return 0;	
			}
			return -1;
		}
		if (strcmp(cmd_ast->op, "&&") == 0) {
			// "command1 && command2"
			int status = eval(cmd_ast->arg0, 0);
			if (status == 0) {
				return eval(cmd_ast->arg1, 0);
			}
			return status;
		}
		if (strcmp(cmd_ast->op, "||") == 0) {
			// "command1 || command2"
			int status = eval(cmd_ast->arg0, 0);
			if (status != 0) {
				return eval(cmd_ast->arg1, 0);
			}
			return status;
		}
		if (strcmp(cmd_ast->op, "&") == 0) {
			// "command1 &" or "command1 & command2" = "command1 &; command2"
			
			int cpid;

			if ((cpid = fork())) {
				int s1 = eval(cmd_ast->arg1, 0);
				return s1;
			}
			else {
				// do work in child
				int status = eval(cmd_ast->arg0, 0);
				// kill child
				if (status != 0) {
					exit(1);
				}
				exit(0);
			}	
		}
		if (strcmp(cmd_ast->op, ">") == 0) {
			// "command1 > file1"
			int cpid;
			if ((cpid = fork())) {
				int status;
				waitpid(cpid, &status, 0);
				return status;
			} else {
				char* file_path = svec_get(cmd_ast->arg1->cmd, 0);
				int fd_out = open(file_path, O_WRONLY|O_CREAT, 0666);
				if (fd_out < 0) {
					perror("open error");
				}
				dup2(fd_out, 1);
				int status = eval(cmd_ast->arg0, 0);// write to fd in stdout slot
				// kill child
				if (status != 0) {
					exit(1);
				}
				exit(0);
			}
		}
		if (strcmp(cmd_ast->op, "<") == 0) {
			// "command1 < file1"
			int cpid;	
			if ((cpid = fork())) {
				int status;
				waitpid(cpid, &status, 0);
				return status;
			} else {
				//close(0); // close stdin
				char* file_path = svec_get(cmd_ast->arg1->cmd, 0);
				int fd_in = open(file_path, O_RDONLY, 0666);
				if (fd_in < 0) {
					perror("open");
				}
				dup2(fd_in, 0);
				int status = eval(cmd_ast->arg0, 0); // read from fd in stdin slot
				// kill child
				if (status != 0) {
					exit(1);
				}
				exit(0);
			}
		}
		if (strcmp(cmd_ast->op, "|") == 0) {
			int fd[2]; // pipe file descriptors
			int rv = pipe(fd);
			check_rv(rv);

			// the output of fd[1] becomes the input for fd[0]
			int cpid_1;
			int cpid_2;

			if ((cpid_1 = fork())) {
				close(fd[1]);
				close(fd[0]);
				int c1_status;
				waitpid(cpid_1, &c1_status, 0);
				return c1_status;
			} else {
				dup2(fd[1], 1); // replace stdout with writing end of pipe
				close(fd[1]);
				close(fd[0]);
				int c1_status = eval(cmd_ast->arg0, 0); // evaluate thing on left and write to writing end of pipe
				
				if (c1_status != 0) {
					perror("first child failure");
					exit(1); // kill child 1
				}
				
				// written to writing end of pipe by now
				// fork to exec arg1 remembering updated fd table
				if ((cpid_2 = fork())) {
					close(fd[1]);
					close(fd[0]);
					// parent of second child
					int c2_status;
					waitpid(cpid_2, &c2_status, 0);
					exit(c2_status); // kill child 1
				} else {
					dup2(fd[0], 0); // replace stdin with reading end of pipe
					close(fd[0]);
					close(fd[1]);
					int c2_status = eval(cmd_ast->arg1, 0);
					// kill child 2
					if (c2_status != 0) {
						exit(1);
					}
					exit(0);
				}

			}
			
		} 

		if (strcmp(cmd_ast->op, "()") == 0) {
			// TODO: Implement subshell
		}
	}
}

void
chomp(char* text)
{
	long l = strlen(text);
	if (text[l-1] == '\n') {
		text[l-1] = 0;
	}
}

int
main(int argc, char* argv[])
{
	if (argv[1]) {
		FILE* fh = fopen(argv[1], "r");
		if (!fh) {
			perror("open failed");
			return 1;
		}
		svec* tokens = make_svec();
		char temp[128];
		while (1) {
			char* line = fgets(temp, 128, fh);
			if (!line) {
				break;
			}
			chomp(line);
			svec* t = tokenize(line);
			
			for (int ii=0; ii < t->size; ++ii) {
				svec_push_back(tokens, svec_get(t, ii));
			}
			svec_push_back(tokens, ";");
		}
		cmd_ast* cmd_ast = parse(tokens);
		hashmap* env = make_hashmap();
		eval(cmd_ast, env);
		fclose(fh);
	} else {
		char cmd[256];
		while (1) {
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
			//cmd_ast_print(cmd_ast);
			hashmap* env = make_hashmap();
			eval(cmd_ast, env);
		}
		printf("\n");
	}
	return 0;
}
