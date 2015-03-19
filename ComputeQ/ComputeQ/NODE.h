//============================================================================
// Name        : NODE.h
// Author      : Jierui Xie (xiej2@rpi.edu)
// Date        : Oct. 2011
// Version     :
// Copyright   : All rights reserved.
// Description : SLPA algorithm for community detection.
// Web Site    : https://sites.google.com/site/communitydetectionslpa/
// Publication:
//             J. Xie, B. K. Szymanski and X. Liu, "SLPA: Uncovering Overlapping Communities in Social Networks via A Speaker-listener Interaction Dynamic Process", IEEE ICDM workshop on DMCCI 2011, Vancouver, CA.
//============================================================================
#ifndef NODE_H_
#define NODE_H_

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

using namespace std;

typedef std::tr1::unordered_set<int> UOrderedSet_INT;
typedef std::tr1::unordered_map<int, double> UOrderedH_INT_DBL;


class NODE {
public:
	int ID;
	int numNbs;
	//vector<int> nbList;
	//vector<NODE *> nbList_In;   //the pointer version
	vector<NODE *> nbList_In;
	//set<int> nbSet_In;
	//UOrderedSet_INT nbSet_In;
	UOrderedSet_INT nbSet_In;

	//The weight corresponding to the incoming neighbors
	vector<double> nbweightList_In;
	//vector<double> nbweightList_In_norm;

	//?---Do not know its role
	double w;   // (that would be used to distribute label along each outgoing link)

	set<int> maxLabels;  //the labels with max pro (could be tie)

	int label;
	int newlabel;

	bool isToUpdate;    //SpeedUp: want to update or not
	//----------------
	//SLPA
	//vector<int> WQueue;
	//vector<pair<int,double> > WQHistMapEntryList;


	//For compute sparse Euclidean distance
	// otherwise, MEMORY
	//map<int,double> WQHistgram;


	vector<pair<int,double> > BCHistMapEntryList;
	vector<pair<int,double> > newBCHistMapEntryList;

	UOrderedH_INT_DBL BCHistgram;
	UOrderedH_INT_DBL newBCHistgram;


	//----------------
	//syn version
	//int newLabel;     //store temporal selected label

	//----------------
	int status;
	int t; 		               //for asynchronous update

	//----------------
	void printHistogram();
	void printBCHistogram();
	void printBCHistMapEntryList();

	void printnewBCHistogram();
	void printnewBCHistMapEntryList();

	//----------------
	NODE();

	NODE(int id){
		ID=id;

		status=0;
		t=0;

		numNbs=0;   //*
	};

	virtual ~NODE();
	//----------------

	void printWQ();

};

#endif /* NODE_H_ */
