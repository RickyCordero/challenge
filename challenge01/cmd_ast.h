#ifndef CMD_AST_H
#define CMD_AST_H

#include <stdlib.h>

// A cmd_ast leaf contains a command name or argument.
// A cmd_ast inner node contains an operator.

typedef struct cmd_ast {
	char* op; 
	// op is either:
	// one of: >, <, |, &, &&, ||, ; / for an operator
	// 	or: = for a leaf
	struct cmd_ast* arg0;
	struct cmd_ast* arg1;
	char* cmd;
} cmd_ast;

cmd_ast* make_cmd_ast_cmd(char* cmd);
cmd_ast* make_cmd_ast_op(char* op, cmd_ast* a0, cmd_ast* a1);
void free_cmd_ast(cmd_ast* cmd_ast);
void cmd_ast_eval(cmd_ast* cmd_ast);
void cmd_ast_print(cmd_ast* cmd_ast);

#endif
