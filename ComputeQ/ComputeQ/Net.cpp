//============================================================================
// Name        : Net.cpp
// Author      : Jierui Xie (xiej2@rpi.edu)
// Date        : Oct. 2011
// Version     :
// Copyright   : All rights reserved.
// Description : SLPA algorithm for community detection.
// Web Site    : https://sites.google.com/site/communitydetectionslpa/
// Publication:
//             J. Xie, B. K. Szymanski and X. Liu, "SLPA: Uncovering Overlapping Communities in Social Networks via A Speaker-listener Interaction Dynamic Process", IEEE ICDM workshop on DMCCI 2011, Vancouver, CA.
//============================================================================

#include "Net.h"
#include "fileOpts.h"
#include "CommonFuns.h"
#include "CommonFuns_TMP.h"
#include <cstdlib>
#include <exception>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>

//#include "LabelRank.h"
//#include <tr1/unordered_set>
typedef std::tr1::unordered_map<int, int> UOrderedH_INT_INT;
typedef std::tr1::unordered_set<int> UOrderedSet_INT;
typedef std::tr1::unordered_set<NODE *> UOrderedSet_NODEptr;

struct sortIDs {
	bool operator()(int i, int j) {
		return (i < j);
	} //increasing
} sortIDs_inc;

Net::Net() {
}

Net::Net(string path, string name, string fname) {
	networkPath = path;
	netName = name;

	fileName = fname;
}

Net::~Net() {
	for (map<int, NODE *>::iterator it = NODESTABLE.begin();
			it != NODESTABLE.end(); ++it) {
		delete it->second;
	}

	this->NODESTABLE.clear();
}

int Net::getNumberofEdges() {
	int m = 0;
	NODE *v;
	for (map<int, NODE *>::iterator it = NODESTABLE.begin();
			it != NODESTABLE.end(); ++it) {
		v = it->second;
		m += v->numNbs;
	}

	return m;
}

void Net::readNet(string fileName, bool isSymmetrize, int weight_version) {
	//read the pairs file: 3 or 2 col, the w is IGNORED
	// Lines not starting with a number are ignored
	// *1.remove self loop
	// *2.auto symmetrize (no matter if it is already is)

	//changedNODESLIST: a set of nodes being updated(changed)
	//                  due to the change on edges
	changedNODESLIST.clear();

	string oneLine, whiteSpace = " \t\n";
	fstream fp;
	fp.open(fileName.c_str(), fstream::in); //|fstream::out|fstream::app

	if (fp.is_open()) { //if it does not exist, then failed

		// repeat until all lines is read
		while (fp.good()) {
			getline(fp, oneLine);

			//--------------------------
			//skip empty line
			if (oneLine.find_first_not_of(whiteSpace) == string::npos) {
				continue;
			}

			//skip any line NOT starting with digit number
			if (!isdigit(oneLine.at(oneLine.find_first_not_of(whiteSpace)))) {
				continue;
			}

			//cout<<"Line:"<<oneLine<<endl;
			//====================================
			//	process one line(edge)
			//====================================
			int fromID, toID;
			double weight = 1.0;    //default value

			stringstream linestream(oneLine);
			bool flag = false;
			if (weight_version > 0) {
				flag = (linestream >> fromID) && (linestream >> toID)
						&& (linestream >> weight);
			} else {
				flag = (linestream >> fromID) && (linestream >> toID);
			}

			if (flag) {
				if (fromID == toID) {
					continue; 				//**remove selfloop
				}

				pre_ReadInOneEdge(fromID, toID, weight);
				if (isSymmetrize) {
					pre_ReadInOneEdge(toID, fromID, weight);     //**symmetrize
				}

				//------------------------------------
				//     tracking the node
				//------------------------------------
				//***what if toID(v) does not exist??
				if (NODESTABLE.count(fromID) > 0) {
					changedNODESLIST.insert(NODESTABLE[fromID]);
				} else {
					cout << "ATTENTION::fromID=" << fromID
							<< " is not added as a change node!!" << endl;
				}

				if (NODESTABLE.count(toID) > 0) {
					changedNODESLIST.insert(NODESTABLE[toID]);
				} else {
					cout << "ATTENTION::toID=" << toID
							<< " is not added as a change node!!" << endl;
				}

			}
		} //while

		fp.close();
	} else {
		cout << "open failed" << endl;
		exit(1);
	}
}

void Net::readNetwork_EdgesList(string fileName, bool isUseLargestComp,
		bool isSymmetrize, int weight_version) {
	//string(isFileExist(fileName.c_str()))
	//---------------------------------------
	if (!isFileExist(fileName.c_str())) {
		cout << fileName << " not found!" << endl;
		exit(1);
	}

	NODESTABLE.clear();

	//---------------------------------------
	// a. read in 2 or 3 col network file
	//    initialize NODESTABLE,v->nbSet_In
	//---------------------------------------
	time_t st = time(NULL);
	readNet(fileName, isSymmetrize, weight_version);

	//------------------------------------
	//  b. initialize nbList using nbSet_In (remove duplicated nbs)
	//------------------------------------
	pre_convert_nbSet_In2_nbList();
	N = NODESTABLE.size();
	M = getNumberofEdges();

	cout << "Reading in the network takes :" << difftime(time(NULL), st)
			<< " seconds." << endl;
	//------------------------------------
	//   b. Using only the largest connected component
	//      do not use this for very large network
	//------------------------------------
	if (isUseLargestComp) {
		cout << "Using largest connected component only.......\n";
		//post_UseLargestComponent_UnorderSet_cpmpointer();
		pre_findGiantComponents();
	}

	//------------------------------------
	//------------------------------------
	if (isSymmetrize)
		cout << "Network info: N=" << N << " M=" << M << "(symmetric)" << endl;
	else
		cout << "Network info: N=" << N << " M=" << M << endl;
	cout << "load " << fileName << " done.." << endl;

	//net.showVertices();net->showVertices_Table();
}

void Net::pre_ReadInOneEdge(int fromID, int toID, double weight) {
	//***WHY add only fromID?? (march 07,2012)
	//***What about toID does not have outgoing edges??
	map<int, NODE *>::iterator mit;
	NODE *vp;

	if (NODESTABLE.count(toID) > 0) {	//add nb
		vp = NODESTABLE[toID];
		vp->nbSet_In.insert(fromID);
	} else {	//add new node
		vp = new NODE(toID);  //create
		vp->nbSet_In.insert(fromID);
		NODESTABLE.insert(pair<int, NODE *>(toID, vp));
	}

	if (NODESTABLE.count(fromID) == 0) {
		vp = new NODE(fromID);
		NODESTABLE.insert(pair<int, NODE *>(fromID, vp));
	}

	//Read in the weght and update if necessary
	string edgeKey = int2str(toID) + "_" + int2str(fromID);
	if (this->edgesTable.count(edgeKey) > 0) {
		this->edgesTable[edgeKey] = weight;
	} else {
		this->edgesTable.insert(pair<string, double>(edgeKey, weight));
	}
}

void Net::showVertices() {
	cout << "-----------------------" << endl;
	cout << "N=" << NODESTABLE.size() << endl;
	for (map<int, NODE *>::iterator it = NODESTABLE.begin();
			it != NODESTABLE.end(); ++it) {
		NODE * vp = it->second;
		cout << vp->ID << endl;
	}
}

void Net::showVertices_Table() {
	cout << "-----------------------" << endl;
	//cout<<"N="<<NODESTABLE.size()<<endl;
	cout << "N=" << N << "(" << NODESTABLE.size() << ")" << " M=" << M << endl;

	map<int, NODE *>::iterator it;
	for (it = NODESTABLE.begin(); it != NODESTABLE.end(); it++) {
		NODE * v = it->second;
		cout << v->ID << endl;

		for (int j = 0; j < v->numNbs; j++)
			cout << "->" << v->nbList_In[j]->ID << endl;
	}
}

void Net::pre_convert_nbSet_In2_nbList() {
	//now, we can use both nbList_In and nbSet_In
	//*use nbSet_In rather that nbList_In is after symmetrized (not check),remove duplicated nb
	//  nbList is NOT available
	//
	UOrderedSet_INT::iterator sit;
	NODE * v;
	NODE * nbv;
	double maxWeight = DBL_MIN;

	//create nbList-pointer version: ASSUMING the TABLES is ready
	for (map<int, NODE *>::iterator it = NODESTABLE.begin();
			it != NODESTABLE.end(); ++it) {
		v = it->second;
		int toID = v->ID;
		v->nbList_In.clear();
		v->nbweightList_In.clear();
		for (sit = v->nbSet_In.begin(); sit != v->nbSet_In.end(); sit++) {
			nbv = NODESTABLE.find(*sit)->second;
			int fromID = nbv->ID;
			v->nbList_In.push_back(nbv);  //pointer
			string edgeKey = int2str(toID) + "_" + int2str(fromID);
			double weight = this->edgesTable[edgeKey];
			v->nbweightList_In.push_back(weight);
		}

		v->numNbs = v->nbList_In.size();
	}
}

vector<vector<int>*> Net::post_removeSubset_UorderedHashTable_cpmpointer(
		vector<vector<int>*>& cpm) {
	//no need to be in the net. it does not use structure
	//???????????????????????????????????
	//*****(after have newcpm) at the end,NEED to delete the old cpm pointers*****

	cout << "start post_removeSubset_UorderedHashTable_cpmpointer()...."
			<< endl;

	time_t st;
	//bool isDEBUG=true;
	//if(isDEBUG) cout<<"removeSubset (Unordered HASH)............."<<endl;

	vector<vector<int>*> newcpm;

	//1. ***sort cpm by the community size(***decreasing***)
	st = time(NULL);
	sort_cpm_pointer(cpm);  //***CMPP

	//cout<<"sort_cpm takes :" <<difftime(time(NULL),st)<< " seconds."<<endl;

	//2.check the subset relationship
	//cout<<"***before cpm.sie="<<cpm.size()<<endl;
	st = time(NULL);

	//2.1 vector of map corresponding to the sorted cpm(decreasing)
	vector<UOrderedH_INT_INT*> vectHTable;

	for (int i = 0; i < cpm.size(); i++) {
		UOrderedH_INT_INT* H = new UOrderedH_INT_INT;  //**
		for (int j = 0; j < cpm[i]->size(); j++)            //***CMPP
			//H->insert(pair<int,int>(cpm[i][j],cpm[i][j])); //id as both key and value
			H->insert(pair<int, int>((*cpm[i])[j], (*cpm[i])[j])); //id as both key and value

		vectHTable.push_back(H);
	}

	//0.the indicator
	//vector<int> indicators;
	//for(int i=0;i<cpm.size();i++)
	//	indicators.push_back(1);  //1-default ok
	//int indicators[cpm.size()];
	int *indicators = new int[cpm.size()];
	for (int i = 0; i < cpm.size(); i++)
		indicators[i] = 1;  		  //1-default ok

	//2.2 find the subset (compare smaller cpmi to largest H(j) first)
	bool issubset;
	for (int i = cpm.size() - 1; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			//visit all coms(in HASH) that are LARGER than it
			//check if cpm(i) is a subset of H(j)
			issubset = true;
			for (int k = 0; k < cpm[i]->size(); k++)
				if (vectHTable[j]->count((*cpm[i])[k]) == 0) {		//not found
					issubset = false;
					break;
				}

			//issubset=issubset_cpm_hash(cpm[i],vectHTable[j]);

			if (issubset) {  //remove this cpm
				indicators[i] = 0;   //**change i**
				break;
			}
		}
	}

	//3.newcpm
	for (int i = 0; i < cpm.size(); i++) {
		if (indicators[i] == 0)
			continue;

		newcpm.push_back(cpm[i]);
	}

	//release memory
	for (int i = 0; i < vectHTable.size(); i++)
		delete vectHTable[i];

	return newcpm;
}

vector<vector<int>*> Net::pre_findAllConnectedComponents_InOneCluster_CPM_cpmpointer(
		vector<vector<int>*>& cpm) {
	//*****(after have newcpm) at the end,NEED to delete the old cpm pointers*****

	//***The cluster could be a sub cluster or the whole network***
	//INPUT: CPM(line contains one cluster with node ids). if a com consists of separate components, they become
	//       separate comms.
	// Trick: using set instead of list

	//OUTPUT: newCPM sorted in **decreasing** cluster size order

	map<int, NODE *>::iterator mit;
	NODE *v;

	//*****(use pointer)
	//SLPA::sort_cpm(cpm);
	//**sort BOTH inside(node id) and among comms(comm size)!**
	sort_cpm_pointer(cpm);

	//*****
	vector<vector<int>*> newcpm;

	for (int i = 0; i < cpm.size(); i++) {
		//for each community
		//*****(use pointer)
		set<int> Com(cpm[i]->begin(), cpm[i]->end());  //copy one com

		while (Com.size() > 0) {
			//for each component
			set<int> exploredSet;
			set<int> unexploredSet;

			//first node
			int vid = getFirstElemnet_Set_PRIMITIVE<int>(Com);

			//set<int> nbSet_In=NODESTABLE.get(vid).nbSet_In;
			mit = NODESTABLE.find(vid);
			v = mit->second;

			//*********
			//set<int> nbSet_In=v->nbSet_In;
			set<int> nbSet_In(v->nbSet_In.begin(), v->nbSet_In.end());

			//Key**: confined to one cluster
			//set<int> newnbSet_In=mySet_Intersect_PRIMITIVE<int>(nbSet_In,Com);  //CollectionFuns.interSet(nbSet_In,Com);
			set<int> newnbSet_In = mySet_Intersect_PRIMITIVE<int>(nbSet_In,
					Com);  //CollectionFuns.interSet(nbSet_In,Com);

			unexploredSet.insert(newnbSet_In.begin(), newnbSet_In.end()); //unexploredSet.addAll(newnbSet_In);
			Com = mySet_Diff_PRIMITIVE<int>(Com, newnbSet_In); //Com.removeAll(newnbSet_In);

			exploredSet.insert(vid);   //exploredSet.add(vid);
			Com.erase(vid);            //Com.remove(vid);

			while (unexploredSet.size() > 0) {
				//first node
				vid = getFirstElemnet_Set_PRIMITIVE<int>(unexploredSet); //vid=getFirstElemnetInSet(unexploredSet);
				mit = NODESTABLE.find(vid);	//nbSet_In=NODESTABLE.get(vid).nbSet_In;
				v = mit->second;
				nbSet_In.clear();

				//**************
				//nbSet_In=v->nbSet_In;
				set<int> xx(v->nbSet_In.begin(), v->nbSet_In.end());
				nbSet_In = xx;

				//***Key: confined to one cluster
				newnbSet_In.clear();
				newnbSet_In = mySet_Intersect_PRIMITIVE<int>(nbSet_In, Com); //CollectionFuns.interSet(nbSet_In,Com);
				unexploredSet.insert(newnbSet_In.begin(), newnbSet_In.end()); //unexploredSet.addAll(newnbSet_In);
				Com = mySet_Diff_PRIMITIVE<int>(Com, newnbSet_In); //Com.removeAll(newnbSet_In);

				unexploredSet.erase(vid); //unexploredSet.remove(vid);
				exploredSet.insert(vid);  //exploredSet.add(vid);
			}

			//*****(use pointer)
			//get a connected component
			//vector<int>* oneComponent_ptr=new vector<int>(exploredSet.begin(), exploredSet.end());
			vector<int>* oneComponent_ptr = new vector<int>();
			for (set<int>::iterator it = exploredSet.begin();
					it != exploredSet.end(); ++it) {
				oneComponent_ptr->push_back(*it);
			}
			newcpm.push_back(oneComponent_ptr);
		}
	}

	//------------------------
	//*****(use pointer)
	//**sort BOTH inside(node id) and among comms(comm size)!**
	sort_cpm_pointer(newcpm);

	//------------------------
	if (newcpm.size() != cpm.size()) {
		cout << "before K=" << cpm.size()
				<< " after post_sameLabelDisconnectedComponents() K="
				<< newcpm.size() << endl;
		//cout<<"------------before----------"<<endl;
		//printVectVect_INT_pointer(cpm);
		//cout<<"------------after----------"<<endl;
		//printVectVect_INT_pointer(newcpm);
	}

	//---------------------------
	//release old cpm memory
	//---------------------------
	for (int i = 0; i < cpm.size(); i++)
		delete cpm[i];

	return newcpm;
}

void Net::post_addSelfloop(int weight_version) {
	//***duplication-safe
	cout << "add selfloop (duplication-safe) ...." << endl;
	NODE *v;

	for (map<int, NODE *>::iterator it = NODESTABLE.begin();
			it != NODESTABLE.end(); ++it) {
		v = it->second;

		//Only add when not yet exist
		if (v->nbSet_In.count(v->ID) == 0) {
			v->numNbs++;
			//itself as nb
			v->nbSet_In.insert(v->ID);
			v->nbList_In.push_back(v);
			M++;
		}
	}

	UOrderedSet_INT::iterator sit;

	//norm and selfloop
	for (map<int, NODE *>::iterator it = NODESTABLE.begin();
			it != NODESTABLE.end(); ++it) {
		v = it->second;
		string edgeKey = int2str(v->ID) + "_" + int2str(v->ID);
		double defaultWeight = 1.0;
		double totalWeight = 0;
		int number = v->nbweightList_In.size();
		double minWeight = 1.0;
		double maxWeight = 1.0;
		double avgWeight = 1.0;
		if (number > 0) {
			minWeight = DBL_MAX;
			maxWeight = DBL_MIN;
		}

		for (int j = 0; j < number; j++) {
			double weight = v->nbweightList_In[j];
			if (weight < minWeight) {
				minWeight = weight;
			}

			if (weight > maxWeight) {
				maxWeight = weight;
			}
			totalWeight += weight;
		}

		if (number > 0) {
			avgWeight = totalWeight / number;
		}

		//add max of selfloop after norm
		if (weight_version == 1) {
			v->nbweightList_In.push_back(maxWeight);
			if (this->edgesTable.count(edgeKey) > 0) {
				this->edgesTable[edgeKey] = maxWeight;
			} else {
				this->edgesTable.insert(
						pair<string, double>(edgeKey, maxWeight));
			}
		} else if (weight_version == 2) {
			//add average weight of selfloop before norm
			v->nbweightList_In.push_back(avgWeight);
			if (this->edgesTable.count(edgeKey) > 0) {
				this->edgesTable[edgeKey] = avgWeight;
			} else {
				this->edgesTable.insert(
						pair<string, double>(edgeKey, avgWeight));
			}
		} else if (weight_version == 3) {
			//add minimum weight of selfloop before norm
			v->nbweightList_In.push_back(minWeight);
			if (this->edgesTable.count(edgeKey) > 0) {
				this->edgesTable[edgeKey] = minWeight;
			} else {
				this->edgesTable.insert(
						pair<string, double>(edgeKey, minWeight));
			}
		} else if (weight_version == 0) {
			//no weight (just 1)
			v->nbweightList_In.push_back(defaultWeight);
			if (this->edgesTable.count(edgeKey) > 0) {
				this->edgesTable[edgeKey] = defaultWeight;
			} else {
				this->edgesTable.insert(
						pair<string, double>(edgeKey, defaultWeight));
			}
		}
	}
}

void Net::post_removeSelfloop() {
	NODE * v;

	for (map<int, NODE *>::iterator it = NODESTABLE.begin();
			it != NODESTABLE.end(); ++it) {
		v = it->second;

		//Only add when not yet exist
		if (v->nbSet_In.count(v->ID) > 0) {
			v->numNbs--;
			M--;
			v->nbSet_In.erase(v->ID);
		}
	}
}

void Net::pre_findGiantComponents() {
	time_t st = time(NULL);
	//------------------------------------------
	//		find gaint component
	//------------------------------------------
	vector<vector<NODE *> > coms; //all components

	UOrderedSet_NODEptr UnExpSet;  // unexported set
	for (map<int, NODE *>::iterator it = NODESTABLE.begin();
			it != NODESTABLE.end(); ++it) {
		UnExpSet.insert(it->second);
	}

	UOrderedSet_NODEptr WorkingSet; 						// unexported set

	NODE *v;
	NODE *nbv;

	while (!UnExpSet.empty()) {
		//new com
		vector<NODE *> com;

		v = *UnExpSet.begin(); // take the first node

		//mark this node
		WorkingSet.insert(v);
		UnExpSet.erase(v);
		v->status = 1;
		com.push_back(v);

		//find one com
		while (!WorkingSet.empty()) {
			v = *WorkingSet.begin(); // take the first nb node

			//explore the nbs
			for (int i = 0; i < v->nbList_In.size(); i++) {
				nbv = v->nbList_In[i];

				if (nbv->status == 0) {
					//mark this node
					WorkingSet.insert(nbv);
					UnExpSet.erase(nbv);
					nbv->status = 1;
					com.push_back(nbv);
				}
			}

			//remove this v
			WorkingSet.erase(v);
		}

		// add to the list
		coms.push_back(com);
	}

	//----------------------------
	//	et the giant one and  update the network
	//----------------------------
	//***Key:sorting and get largest (decreasing size)
	sortVecVec_bySize<NODE*>(coms);

	cout << "check the sorting of sizes:" << endl;
	int sum = 0;
	for (int i = 0; i < coms.size(); i++) {
		cout << coms[i].size() << endl;
		sum += coms[i].size();
	}
	cout << " sum size=" << sum << " N=" << N << endl;

	//------------------------------------------
	//3.reset the network as only the largest largestCom
	//change NODESTABLE and N
	NODESTABLE.clear();
	for (int i = 0; i < coms[0].size(); i++)
		NODESTABLE.insert(pair<int, NODE *>(coms[0][i]->ID, coms[0][i]));

	N = NODESTABLE.size();
	M = getNumberofEdges();

	//start *1*
	for (int i = 1; i < coms.size(); i++) {
		for (int j = 0; j < coms[i].size(); j++)
			delete coms[i][j];  //one node
	}

	//4.**assuming that we do not need to handle the nbList and nbSet_In
	cout << "Finding largest component in the network takes :"
			<< difftime(time(NULL), st) << " seconds." << endl;
}

void Net::write2txt_Net2EdgesList(string fileName) {
	//read the nodes and their nbs, output the EdgesList
	// (i,j,w), w is set to 1 by default
	NODE * v;
	int fromID, toID, cn = 0;

	vector<string> data;
	for (map<int, NODE *>::iterator it = NODESTABLE.begin();
			it != NODESTABLE.end(); ++it) {
		v = it->second;

		fromID = v->ID;
		for (int j = 0; j < v->nbList_In.size(); j++) {
			toID = v->nbList_In[j]->ID;

			//constructe on line (edge)
			string line = int2str(fromID) + " " + int2str(toID) + " 1";

			data.push_back(line);

			cn++;
		}
	}

	//fileOpts.writeToTxt(fileName, false, data);// **false:the first one
	writeToTxt(fileName, false, data);

	cout << "write " << fileName << " (N=" << NODESTABLE.size() << " M=" << cn
			<< ")!" << endl;
}

//------------------------------------------------------------
//				Growing networks (temporal)
//------------------------------------------------------------
void Net::read_DetNetwork_EdgesList_add2CurNet(string fileName,
		bool isUseLargestComp, bool isSymmetrize, int weight_version) {
	//read in the change in the edges list, i.e., Det G
	// add to the current net and update corresponding variable,N,M etc.

	// ***Seem that even not PURE change, it is fine because
	//    use nbSet_In.
	//
	//---------------------------------------
	if (!isFileExist(fileName.c_str())) {
		cout << fileName << " not found!" << endl;
		exit(1);
	}

	//****************************************
	//           ONLY Different
	//****************************************
	//NODESTABLE.clear();

	//---------------------------------------
	// a. read in 2 or 3 col network file
	//    initialize NODESTABLE,v->nbSet_In
	//---------------------------------------
	readNet(fileName, isSymmetrize, weight_version);

	//------------------------------------
	//  b. initialize nbList using nbSet_In (remove duplicated nbs)
	//------------------------------------
	pre_convert_nbSet_In2_nbList();
	N = NODESTABLE.size();
	M = getNumberofEdges();

	//------------------------------------
	//   b. Using only the largest connected component
	//      do not use this for very large network
	//------------------------------------
	if (isUseLargestComp) {
		cout << "Using largest connected component only.......\n";
		//post_UseLargestComponent_UnorderSet_cpmpointer();
		pre_findGiantComponents();
	}

	//------------------------------------
	//------------------------------------
	if (isSymmetrize)
		cout << "Network info: N=" << N << " M=" << M << "(symmetric)" << endl;
	else
		cout << "Network info: N=" << N << " M=" << M << endl;
	cout << "load " << fileName << " done.." << endl;

	//net.showVertices();net->showVertices_Table();
}

void Net::printchangedNODESLIST() {
	cout << "nodes that change due to the change in edges:" << endl;
	for (set<NODE *>::iterator sit = changedNODESLIST.begin();
			sit != changedNODESLIST.end(); sit++) {
		cout << (*sit)->ID << endl;
	}

	cout << "total:" << changedNODESLIST.size() << endl;
}

void Net::getSecondNBhood_add2changedNODESLIST() {
	NODE *v;
	set<NODE *> secondnbSet_In;

	cout << "total(1-nbs):" << changedNODESLIST.size() << endl;
	for (set<NODE *>::iterator sit = changedNODESLIST.begin();
			sit != changedNODESLIST.end(); sit++) {
		v = (*sit);

		for (int k = 0; k < v->nbList_In.size(); k++) {
			secondnbSet_In.insert(v->nbList_In[k]);
		}
	}

	//add secondnbSet_In to changedNODESLIST
	for (set<NODE *>::iterator sit = secondnbSet_In.begin();
			sit != secondnbSet_In.end(); sit++) {
		v = (*sit);
		changedNODESLIST.insert(v);
	}

	cout << "total(2-nbs):" << changedNODESLIST.size() << endl;
}

