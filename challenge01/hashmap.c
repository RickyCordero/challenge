
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// TODO: sudo apt install libbsd-dev
// This provides strlcpy
// See "man strlcpy"
#include <bsd/string.h>
#include <string.h>

#include "hashmap.h"


int
hash(char* key)
{
    // TODO: Produce an appropriate hash value.
    //return 0;
    //return strdup(key);
    int h = 0;
    for (int ii = 0; key[ii]; ++ii){
	h = h * 67 + key[ii];
    }
    return h;
}

hashmap_pair*
make_pair()
{
	hashmap_pair* p = malloc(sizeof(hashmap_pair));
	return p;
}

hashmap*
make_hashmap_presize(int nn)
{
    hashmap* hh = calloc(1, sizeof(hashmap));
    // TODO: Allocate and initialize a hashmap with capacity 'nn'.
    // Double check "man calloc" to see what that function does.
    hh->data = malloc(nn * sizeof(hashmap_pair*));
    for(int ii=0; ii<nn; ++ii){
	 // allocate a new hashmap_pair for each entry
	 hh->data[ii] = make_pair();
	 // initialize all hashmap_pairs as unused and alive
	 hh->data[ii]->used = 0;
	 hh->data[ii]->tomb = 0;
    }
    hh->size = 0;
    hh->capacity = nn;
    return hh;
}

hashmap*
make_hashmap()
{
    return make_hashmap_presize(4);
}

hashmap_pair
hashmap_get_pair(hashmap* hh, int ii)
{
    // TODO: Get the {k,v} pair stored in index 'ii'.
    return *(hh->data[ii]);
}

void
free_hashmap(hashmap* hh)
{
    // TODO: Free all allocated data.
    if (hh) {
   	for(int ii=0; ii < hh->size; ++ii){
		if (hh->data[ii]){
			//free_hashmap_pair(hh->data[ii]);
		}
	}
	free(hh->data); // free array of hashmap_pair
	free(hh); // free hashmap struct
    }
}

int
hashmap_has(hashmap* hh, char* kk)
{
    return hashmap_get(hh, kk) != -1;
}

int
hashmap_get(hashmap* hh, char* kk)
{
    // TODO: Return the value associated with the
    // key kk.
    // Note: return -1 for key not found.
    int start_index = hash(kk) % hh->capacity;
    int looped = 0;
    for (int ii = start_index; !looped || ii != start_index; ii = (ii+1) % hh->capacity){
	looped = 1;
    	    if(hashmap_get_pair(hh,ii).used){
		if(strcmp(hashmap_get_pair(hh,ii).key, kk) == 0){ 
			// key found => return val for this hashmap_pair
			return hashmap_get_pair(hh,ii).val;
		}
	} else { // reached "empty" hashmap_pair => kk not in hashmap
		return -1;
	}
    }
    return -1; // made full table loop => kk not in hashmap
}

void
hashmap_put(hashmap* hh, char* kk, int vv)
{
	if (((double)(hh->size)/(double)(hh->capacity)) >= 0.5){
		// resize the data array, then put again
		// => shouldn't get here again
		//resize(hh);
	}

    // TODO: Insert the value 'vv' into the hashmap
    // for the key 'kk', replacing any existing value
    // for that key.
    int start_index = hash(kk) % hh->capacity;
    int looped = 0;
    for (int ii = start_index; !looped || ii != start_index; ii = (ii+1) % hh->capacity){
	looped = 1;
	if(hashmap_get_pair(hh,ii).used){
	// reached used hashmap_pair
		if(strcmp(hashmap_get_pair(hh,ii).key, kk) == 0){ 
		// existing hashmap_pair hash matches key
		// => update existing hashmap_pair
			hh->data[ii]->val = vv;
			hh->data[ii]->tomb = 0;
		} else {
		// existing hashmap_pair doesn't match key
		// => skip and check next
			continue;
		}
	} else {
	// reached "empty" hashmap_pair
	// => put new hashmap_pair

		// update fields in unused hashmap_pair
		// and set used flag to true
		strcpy(hh->data[ii]->key, kk);
		hh->data[ii]->val = vv;
		hh->data[ii]->used = 1; // set flag to true
		hh->data[ii]->tomb = 0; // what does this do?
		
		hh->size = hh->size+1;
		return;
	}
    }
    // shouldn't get here because of load factor policy 
}
/*
void
resize(hashmap* hh)
{
    // resizes a hashmap
    // allocate a new hashmap_pair array with twice the previous capacity
    hashmap_pair** new_data = malloc(2 * hh->capacity * sizeof(hashmap_pair*));
    for(int ii=0; ii < 2*hh->capacity; ++ii){
	 // allocate a new hashmap_pair for each entry
	 new_data[ii] = make_pair();
	 // initialize all pre-existing hashmap_pairs
	  if (ii < hh->capacity){
	  //new_data[ii]->key = strdup(hashmap_get_pair(hh,ii)->key);
	  	strcpy(new_data[ii]->key, hashmap_get_pair(hh,ii).key);
		new_data[ii]->val = hashmap_get_pair(hh,ii).val;
		new_data[ii]->used = hashmap_get_pair(hh,ii).used; // true
		new_data[ii]->tomb = hashmap_get_pair(hh,ii).tomb; // ?
	 } else {
	 // initialize all new hashmap_pairs
		 new_data[ii]->used = 0;
		 new_data[ii]->tomb = 0;
	 }
    }
    
    // free all hashmap_pairs
    for(int ii=0; ii < hh->size; ++ii){
	    if(hh->data[ii]){
		//free_hashmap_pair(hh->data[ii]);
	    }
    }
    //free(hh->data); // free old array of hashmap_pairs

    hh->data = new_data; // assign expanded array to hashmap field

    // hashmap->size doesn't change
    hh->capacity = 2*hh->capacity;
}
*/
/*
void
free_hashmap_pair(hashmap_pair* p)
{
	if (p) {
		free(p->key);
		free(p);
	}

}
*/

void
hashmap_del(hashmap* hh, char* kk)
{
    // TODO: Remove any value associated with
    // this key in the map.
    int start_index = hash(kk) % hh->capacity;
    int looped = 0;
    for (int ii = start_index; !looped || ii != start_index ; ii = (ii + 1) % hh->capacity){
	looped = 1;
	if(hashmap_get_pair(hh,ii).used){
	// reached used hashmap_pair
		if(strcmp(hashmap_get_pair(hh,ii).key, kk) == 0){ 
		// existing hashmap_pair hash matches key
		// => update existing hashmap_pair
			//hh->data[ii]->key = strdup(kk);
			hh->data[ii]->val = 0;
			hh->data[ii]->used = 0;
			hh->data[ii]->tomb = 1;
			return;
		} else {
		// existing hashmap_pair doesn't match key
		// => skip and check next
			//hh->data[ii]->tomb = 1; // indicate we've seen 
			continue;
		}
	} else {
	// reached "empty" hashmap_pair
	// => pair didn't exist in hashmap
		return;		
	}

    }
}

void
hashmap_dump(hashmap* hh)
{
    printf("== hashmap dump ==\n");
    // TODO: Print out all the keys and values currently
    // in the map, in storage order. Useful for debugging.
    for (int ii = 0; ii < hh->size; ++ii){
	printf("<%s, %d>", hashmap_get_pair(hh,ii).key, hashmap_get_pair(hh,ii).val);
    }
}
