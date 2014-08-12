//============================================================================
// Name        : SLPA.h
// Author      : Jierui Xie (xiej2@rpi.edu)
// Date        : Oct. 2011
// Version     :
// Copyright   : All rights reserved.
// Description : SLPA algorithm for community detection.
// Web Site    : https://sites.google.com/site/communitydetectionslpa/
// Publication:
//             J. Xie, B. K. Szymanski and X. Liu, "SLPA: Uncovering Overlapping Communities in Social Networks via A Speaker-listener Interaction Dynamic Process", IEEE ICDM workshop on DMCCI 2011, Vancouver, CA.
//============================================================================
#ifndef SLPA_H_
#define SLPA_H_

#include "Net.h"
#include "NODE.h"
#include <map>
#include <vector>
#include <utility>
#include <unordered_map>

#include "MersenneTwister.h"


//---------------------------
//		Multi-threading
//---------------------------
typedef std::tr1::unordered_map<int, int> UOrderedH_INT_INT;

struct thread_data{
	int  startind;
	int  endind;

	int *pIndicator;

	//expect to do sharro copy of the pointers
	vector<vector<int>* > cpm;
	vector<UOrderedH_INT_INT* > vectHTable;
};


class SLPA {
public:
	//---------------------------
	//		network parameters
	//---------------------------
	Net* net;
	string netName;
	string fileName_net;
	string networkPath;

	bool isUseLargestComp; //***
	//---------------------------
	//		SLPA parameters
	//---------------------------
	vector<double> THRS;      //thr
	vector<int> THRCS; 		  //thr count
	bool isSyn;  			  //is synchronous version?
	int maxT;
	int maxRun;

	//---------------------------
	//		more
	//---------------------------
	string outputDir;

	MTRand mtrand1;
	MTRand mtrand2;

	time_t st1,st2;
	double dt1,dt2,dt3,dt4;

	SLPA(string, vector<double>,int ,int ,string ,bool,int );
	virtual ~SLPA();

	void start();
	void pre_initial_THRCS();
	void initWQueue_more();

	void initLQueue();
	void initPQueue();
	int chooseLabel(NODE *v);
	void addLabeltoLQueue(NODE *v, int label);
	void addLabeltoLQueue1(NODE *v, int label);
	void addLabeltoVectorINT_INT(vector<pair<int,int>>& pairList, int label);
	void addLabeltoVectorINT_INT1(vector<pair<int,int>>& pairList, int label);
	void addLabeltoVector(vector<pair<int, double>>& pairList, NODE *v);
	void addLabeltoNode(vector<pair<int, double>>& pairList, NODE *v);
	int selectMostFrequentLabel(vector<pair<int,int>>& pairList);
	void deleteLabel(NODE *v, vector<pair<int,int>>& pairList);
	void deleteLabel1(NODE *v, vector<pair<int,int>>& pairList);
	void norm_probability(vector<pair<int, double>>& pairList);
	void thresholdLabelInNode(NODE *v);
	void thresholdLabelInVector(vector<pair<int, double>>& pairList, int n);
	void stateDetection(NODE *v);
	bool checkLabelChange(NODE *v, vector<pair<int, double>> pairList);
	void labelinflation(NODE *v);
	void labelProportionate(vector<pair<int, double>>& pairList, double mix);
	double computeSimilarity(vector<pair<int, double>> set1, vector<pair<int, double>>& set2);
	void mixLabeltoNode(vector<pair<int, double>>& pairList, NODE *v);

	void GLPA_syn();
	void GLPA_asyn_pointer();

	int ceateHistogram_selRandMax(const vector<int>& wordsList);
	void post_createWQHistogram_MapEntryList();
	void post_thresholding(vector<pair<int,int> >& pairList, int thrc, vector<int>& WS);

	//need to change
	//vector<vector<int> > post_sameLabelDisconnectedComponents(vector<vector<int> >& cpm);
	//static void show_cpm(vector<vector<int> >& cpm);
	static void sort_cpm(vector<vector<int> >& cpm);

	//cpm pointer function
	vector<vector<int>* > post_removeSubset_UorderedHashTable_cpmpointer(vector<vector<int>* >& cpm);
	static void sort_cpm_pointer(vector<vector<int>* >& cpm);

	void write2txt_CPM_pointer(string fileName,vector<vector<int>* >& cpm);
	void post_threshold_createCPM_pointer(int thrc,string fileName);

	void dothreshold_createCPM(int thrc,vector<vector<int> >& cpm);
	void dothreshold_createCPM_pointer(int thrc,vector<vector<int>* >& cpm);

	static bool isDEBUG;


	//---------------------------
	//		Multi-threading
	//---------------------------
	int numThreads;

	void decomposeTasks(int numTasks,int numThd,int stInds[],int enInds[]);
	static void *removesubset_onethread(void *threadarg);
	vector<vector<int>* > post_removeSubset_UorderedHashTable_cpmpointer_MultiThd(vector<vector<int>* >& cpm);
};

#endif /* SLPA_H_ */
