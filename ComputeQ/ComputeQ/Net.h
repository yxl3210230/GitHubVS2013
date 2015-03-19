//============================================================================
// Name        : Net.h
// Author      : Jierui Xie (xiej2@rpi.edu)
// Date        : Oct. 2011
// Version     : v1.0
// Copyright   : All rights reserved.
// Description : SLPA algorithm for community detection.
// Web Site    : https://sites.google.com/site/communitydetectionslpa/
// Publication:
//             J. Xie, B. K. Szymanski and X. Liu, "SLPA: Uncovering Overlapping Communities in Social Networks via A Speaker-listener Interaction Dynamic Process", IEEE ICDM workshop on DMCCI 2011, Vancouver, CA.
//============================================================================


#ifndef NET_H_
#define NET_H_

#include <cfloat>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string>

#include "NODE.h"
#include <unordered_set>

using namespace std;

typedef std::tr1::unordered_set<int> UOrderedSet_INT;
typedef std::tr1::unordered_map<string, double> UOrderedH_STR_DBL;

class Net {
public:
	int N;	  //number of nodes
	int M;    //number of edges
	map<int,NODE *> NODESTABLE;
	UOrderedH_STR_DBL edgesTable;

	string networkPath;
	string netName;
	string fileName; //full path
	//---------------
	//---------------

	//---------------
	Net();
	Net(string path,string name,string fname);
	virtual ~Net();
	//---------------
	int getNumberofEdges();
	void readNet(string fileName, bool isSymmetrize, int weight_version);
	void readNetwork_EdgesList(string,bool isUseLargestComp, bool isSymmerize, int weight_version);
	void pre_ReadInOneEdge(int fromID,int toID, double weight);
	void write2txt_Net2EdgesList(string fileName);

	void pre_convert_nbSet_In2_nbList();
	void showVertices();
	void showVertices_Table();

	void post_addSelfloop(int weight_version);
	void post_removeSelfloop();


	//void post_UseLargestComponent();
	//void post_UseLargestComponent_UnorderSet_cpmpointer();
	//vector<vector<int> > pre_findAllConnectedComponents_InOneCluster_CPM(vector<vector<int> >& cpm);
	vector<vector<int> > pre_findAllConnectedComponents_InOneCluster_CPMpointer_UnorderSet(vector<vector<int> >& cpm);

	vector<vector<int>* >  pre_findAllConnectedComponents_InOneCluster_CPM_cpmpointer(vector<vector<int>* >& cpm);

	void pre_findGiantComponents();
	vector<vector<int>* > post_removeSubset_UorderedHashTable_cpmpointer_MultiThd(vector<vector<int>* >& cpm);

	vector<vector<int>* > post_removeSubset_UorderedHashTable_cpmpointer(vector<vector<int>* >& cpm);



	//---------------------------
	//   growing(temporal)
	//---------------------------
	void read_DetNetwork_EdgesList_add2CurNet(string fileName, bool isUseLargestComp, bool isSymmetrize, int weight_version);

	// a set of node being updated due to
    // the update on edges
	set<NODE *> changedNODESLIST;
	void printchangedNODESLIST();
	void getSecondNBhood_add2changedNODESLIST();
};

#endif /* NET_H_ */
