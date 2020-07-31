#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cmd_ast.h"

cmd_ast*
make_cmd_ast_cmd(svec* cmd)
{
	cmd_ast* cmd_ast       = malloc(sizeof(*cmd_ast));
	cmd_ast->op            = "=";
	cmd_ast->arg0          =  0;
	cmd_ast->arg1          =  0;
	cmd_ast->cmd           = make_svec();
	//memcpy(cmd_ast->cmd->data, cmd->data, cmd->size*sizeof(char*));
	for (int ii=0; ii < cmd->size; ++ii) {
		svec_push_back(cmd_ast->cmd, strdup(svec_get(cmd, ii)));
	}
	return cmd_ast;
}

cmd_ast*
make_cmd_ast_op(const char* op, cmd_ast* a0, cmd_ast* a1)
{
	cmd_ast* cmd_ast = malloc(sizeof(*cmd_ast));
	cmd_ast->op    = strdup(op);
	cmd_ast->arg0  = a0;
	cmd_ast->arg1  = a1;
	cmd_ast->cmd   = 0;
	return cmd_ast;
}

void
free_cmd_ast(cmd_ast* cmd_ast)
{
	if (cmd_ast->arg0) {
		free_cmd_ast(cmd_ast->arg0);
	}
	if (cmd_ast->arg1) {
		free_cmd_ast(cmd_ast->arg1);
	}
	/* TODO: Implement free cmd_ast function
	if (cmd_ast->cmd) {
		free(cmd_ast->cmd);
	}*/
}

void // should this be int for exit status?
cmd_ast_eval(cmd_ast* cmd_ast)
{
	// Do something
}

char*
cmd_ast_to_string(cmd_ast* cmd_ast)
{
	if (cmd_ast->op == "=") {
		// is command
		char* res = malloc(16);
		sprintf(res, "%s", svec_to_string(cmd_ast->cmd));
		return res;
	} else {
		// is operator
		char* l = cmd_ast_to_string(cmd_ast->arg0);
		char* r = cmd_ast_to_string(cmd_ast->arg1);
		char* res = malloc(128);
		sprintf(res, "(%s %s %s)", l, cmd_ast->op, r);
		
		if (l) {
			free(l);
		}
		if (r) {
			free(r);
		}
		return res;
	}
}

void
cmd_ast_print(cmd_ast* cmd_ast)
{
	char* cmd_tree_string = cmd_ast_to_string(cmd_ast);
	printf("%s\n", cmd_tree_string);
	free(cmd_tree_string);
}
