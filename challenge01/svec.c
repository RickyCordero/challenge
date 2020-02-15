/* This file is lecture notes from CS 3650, Fall 2018 */
/* Author: Nat Tuck */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "svec.h"

svec*
make_svec()
{
    svec* sv = malloc(sizeof(svec));
    sv->data = malloc(2 * sizeof(char*));
    sv->size = 0;
    sv->capacity = 2;
    return sv;
}

void
free_svec(svec* sv)
{
	// TODO: free all allocated data
	for(int ii=0; ii<sv->size; ++ii){
		free(svec_get(sv, ii));	
	}
	free(sv->data);
	free(sv);
}

char*
svec_get(svec* sv, int ii)
{
    assert(ii >= 0 && ii < sv->size);
    return sv->data[ii];
}

void
svec_put(svec* sv, int ii, char* item)
{
    assert(ii >= 0 && ii < sv->size);
    //sv->data[0] = item;
    sv->data[ii] = strdup(item);
    // TODO: insert item into slot ii
    // Consider ownership of string in collection.
}

void
svec_push_back(svec* sv, char* item)
{
    int ii = sv->size;

    // TODO: expand vector if backing erray
    // is not big enough

	if (ii == sv->capacity){
		
		char** d = malloc(2 * sv->capacity * sizeof(char*));
		// create new data array of twice the size
		// of the previous
		for(int jj = 0; jj < sv->size; ++jj){
			// copy new string and assign
			// and free old string
			d[jj] = svec_get(sv, jj);
		}
		// free old array
		free(sv->data);
		//assign new array
		sv->data = d;
		sv->capacity = 2 * sv->capacity;
	}
	sv->size = ii + 1;
	svec_put(sv, ii, item);
}

void
svec_swap(svec* sv, int ii, int jj)
{
    // TODO: Swap the items in slots ii and jj
    char* tmp = strdup(svec_get(sv, ii));
    svec_put(sv, ii, svec_get(sv, jj));
    svec_put(sv, jj, tmp);
    free(tmp);
}

void
svec_rev(svec* sv)
{
	// Reverses a svec
	for (int ii=0; ii < sv->size/2; ++ii){
		svec_swap(sv, ii, sv->size-ii-1);
	}
}

void 
svec_print(svec* sv)
{
	for(int ii=0; ii < sv->size; ++ii){
		printf("%s\n", svec_get(sv, ii));
	}
}

char*
svec_to_string(svec* sv)
{
	int cc = 0;
	for (int ii=0; ii<sv->size; ++ii) {
		for (int jj=0; jj < strlen(sv->data[ii]); ++jj) {
			cc+=1;
		}
	}
	// Create a space separated string
	char* res = malloc((cc + sv->size)*sizeof(char));
	int kk=0;
	for(int ii=0; ii < sv->size; ++ii){
		int jj = 0;
		for (; jj < strlen(sv->data[ii]); ++jj){
			res[kk + jj] = sv->data[ii][jj];
		}
		kk += jj;	
		if (ii != sv->size-1){
			res[kk] = ' ';
			kk+=1;
		}
	}
	res[kk] = 0; // null terminated string
	return res;
}
