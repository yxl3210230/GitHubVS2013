#ifndef _DATASTRUCT_H
#define _DATASTRUCT_H



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


#endif