#ifndef CMD_AST_PARSE_H
#define CMD_AST_PARSE_H

#include "cmd_ast.h"
#include "svec.h"

cmd_ast* parse(svec* tokens);
svec* slice(svec* tokens, int lower, int upper);

#endif
