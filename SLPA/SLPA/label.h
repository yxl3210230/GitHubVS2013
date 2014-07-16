#ifndef _LABEL_H
#define _LABEL_H




typedef struct {
	int id;
	double probability;
	MEMORY *ptr;
}MEMORY;




typedef struct {
  int nvertices;     // Number of vertices in network
  
  MEMORY *mem;    // Array of VERTEX structs, one for each vertex
}LABEL;




#endif