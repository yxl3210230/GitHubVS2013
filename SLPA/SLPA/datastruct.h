#ifndef _DATASTRUCT_H
#define _DATASTRUCT_H


#define MAX_T 100
#define CONST_P 30

struct MEMORY_SLPA
{
	int nvertices;
	int **memory;
};


struct PROCESSING_QUEUE
{
	int length;
	int *queue;
};


struct NEIGHBOR_VERTICES
{
	int degree;
	int *neighbor;
};


struct LABEL_PROBABILITIES
{
	int numlabel;
	int *index;
	float *probability;
};

struct SL_ELEMENT
{
	int index;
	float weight;
	float weight2;
};

struct SEND_LIST
{
	int num;
	SL_ELEMENT *list;
};


struct RESULT_VERTEX
{
	int nvertices;
	int *numbelong;
	int **communities;
};


struct RESULT_COMMUNITY
{
	int ncommunities;
	int *nvertices;
	int **vertices;
};

struct COEFFICIENT1
{
	int nvertices;
	float *coefficients;
};

struct COEFFICIENT2
{
	int nvertices;
	int *degrees;
	float **coefficients;
};

struct MEMORY_LABEL
{
	int id;
	int num;
	MEMORY_LABEL *next;
};

struct MEMORY_VERTEX
{
	int nvertices;
	int *nlabels;
	MEMORY_LABEL *head;
};

struct DELETE_LIST
{
	int nvertices;
	int *nlabels;
	int **label;
};


#endif