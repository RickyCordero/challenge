#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "parse.h"

int
first_op_index(svec* tokens, const char* op)
{
	for (int ii=0; ii < tokens->size; ++ii) {
		if (strcmp(op, svec_get(tokens, ii)) == 0) {
			return ii;
		}
	}
	return -1;
}

int
contains(svec* tokens, const char* op)
{
	return first_op_index(tokens, op) >= 0;
}

svec*
slice(svec* sv, int lower, int upper)
{
	svec* res = make_svec();
	for (int ii = lower; ii < upper; ++ii) {
		svec_push_back(res, svec_get(sv, ii));
	}
	return res;
}

cmd_ast*
parse(svec* tokens)
{
	/*
	if (tokens->size == 1) {
		return make_cmd_ast_cmd(svec_get(tokens, 0));
	}*/

	// TODO: This array determines order of operations
	// -> Figure out what the order should be
	const char* ops[] = {"<", ">", "|", "&", "&&", "||", ";"};

	for (int ii=0; ii < 7; ++ii) {
		const char* op = ops[ii];

		if (contains(tokens, op)) {
			int jj = first_op_index(tokens, op);
			svec* l = slice(tokens, 0, jj);
			svec* r = slice(tokens, jj+1, tokens->size);
			cmd_ast* cmd_ast = make_cmd_ast_op(op, parse(l), parse(r));
			free_svec(l);
			free_svec(r);
			return cmd_ast;
		}
	}
	// Must be command string
	cmd_ast* cmd_ast = make_cmd_ast_cmd(tokens);
	return cmd_ast;
	//fprintf(stderr, "Invalid token stream");
	//exit(1);
}
