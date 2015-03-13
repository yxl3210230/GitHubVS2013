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

using namespace std;

typedef std::tr1::unordered_set<int> UOrderedSet_INT;


class NODE {
public:
	int ID;
	int numNbs;		//邻节点数
	//vector<int> nbList;
	vector<NODE *> nbList_P;   //the pointer version//邻节点集，NODE集
	//set<int> nbSet;
	UOrderedSet_INT nbSet;	//邻节点集，int集

	//----------------
	//SLPA
	vector<int> WQueue;		//无用
	//map<int,int> WQHistgram;
	vector<pair<int,int> > WQHistMapEntryList;	//无用

	vector<pair<int,int>> LQueue;	//无用
	int nlabels;	//该节点的标签数量	//已无用

	vector<pair<int, double>> PQueue;	//标签集，(标签，评分)
	int isToUpdate;	//下一轮迭代是否需要更新标签
	int isChanged;	//本轮迭代该节点标签种类是否发生改变	//已无用

	//----------------
	int status;
	int t; 		               //for asynchronous update

	//----------------
	NODE();

	NODE(int id){
		ID=id;

		status=0;
		t=0;
	};

	NODE(const NODE& node){
		ID = node.ID;
		numNbs = node.numNbs;
		nbList_P = node.nbList_P;
		nbSet = node.nbSet;
		PQueue = node.PQueue;
		isToUpdate = node.isToUpdate;

		status = 0;
		t = 0;
	};

	NODE& operator=(const NODE& node);		//新增

	virtual ~NODE();
	//----------------


};

#endif /* NODE_H_ */
