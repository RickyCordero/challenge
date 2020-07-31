#include <stdio.h>

#include "svec.h"
#include "tokenize.h"

void
rev_svec(svec* sv)
{
	// Reverses a svec	
	for(int ii=0; ii < sv->size/2; ++ii){
		svec_swap(sv, ii, sv->size-ii-1);
	}
}

void
print_svec(svec* sv)
{
	for(int ii=0; ii < sv->size; ++ii){
		printf("%s\n", svec_get(sv, ii));
	}
}
/*
int
main(int _ac, char* _av[])
{
	char line[100];
	while(1) {
		char* l = fgets(line, 96, stdin);
		if(!l){
			break;
		}
		svec* tokens = tokenize(line);
		rev_svec(tokens);
		print_svec(tokens);
	}
	return 0;
}
*/
