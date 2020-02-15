
/* TODO:

   while (1) {
     printf("tokens$ ");
     fflush(stdout);
     line = read_line()
     if (that was EOF) {
        exit(0);
     }
     tokens = tokenize(line);
     foreach token in reverse(tokens):
       puts(token)
   }

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ctype.h>

#include "svec.h"
#include "tokenize.h"


// kinds of tokens:
// - command names 
// - command arguments
// - shell operators
// spaces that appear in commands are not tokens, but can separate them

// seven operators:
// - redirect input:  sort         <  foot.txt
// - redirect output: sort foo.txt >  output.txt
// - pipe:            sort foo.txt |  uniq
// - background:      sleep 10     &
// - and:             true         && echo one
// - or:	      true         || echo one
// - semicolon:       echo one     ;  echo two

int
is_shell_operator(const char* text, int jj)
{
	//return 0;
	
	return text[jj] == '<' || 
		text[jj] == '>'||
		text[jj] == '|'||
		text[jj] == '&'||
		text[jj] == ';';
}

char*
read_number(const char* text, long ii)
{
	int nn = 0;
	while (isdigit(text[ii + nn])) {
		++nn;
	}

	char* number = malloc(nn + 1);
	memcpy(number, text+ii, nn);
	number[nn] = 0;
	return number;	
}

char*
read_shell_operator(const char* text, long ii)
{
	int nn = 0;
	while(is_shell_operator(text, ii + nn)){
		++nn;
	}
	char* shell_operator = malloc(nn+1);
	memcpy(shell_operator, text+ii, nn);
	shell_operator[nn] = 0;
	return shell_operator;
}

char*
read_arg_or_name(const char* text, long ii)
{
	int nn = 0;
	while(!isspace(text[ii + nn]) && !is_shell_operator(text, ii + nn)) {
		++nn;
	}

	char* arg_or_name = malloc(nn+1);
	memcpy(arg_or_name, text + ii, nn);
	arg_or_name[nn] = 0;
	return arg_or_name;
}

svec*
tokenize(const char* text)
{
	svec* sv = make_svec();
	long nn = strlen(text);
	long ii = 0;

	while (ii < nn) {
		if (isspace(text[ii])) {
			++ii;
			continue;
		}

		if (is_shell_operator(text, ii)) {
			char* shell_operator = read_shell_operator(text, ii);
			svec_push_back(sv, shell_operator);
			ii += strlen(shell_operator);
			free(shell_operator);
			continue;
		}

		if(isdigit(text[ii])) {
			char* number = read_number(text, ii);
			svec_push_back(sv, number);
			ii += strlen(number);
			free(number);
			continue;
		}

		// Else, it must be a command name or argument
		char* arg_or_name = read_arg_or_name(text, ii);
		svec_push_back(sv, arg_or_name);
		ii += strlen(arg_or_name);
		free(arg_or_name);
	}
	return sv;
}
