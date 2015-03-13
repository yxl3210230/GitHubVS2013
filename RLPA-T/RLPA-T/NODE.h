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
	int numNbs;		//�ڽڵ���
	//vector<int> nbList;
	vector<NODE *> nbList_P;   //the pointer version//�ڽڵ㼯��NODE��
	//set<int> nbSet;
	UOrderedSet_INT nbSet;	//�ڽڵ㼯��int��

	//----------------
	//SLPA
	vector<int> WQueue;		//����
	//map<int,int> WQHistgram;
	vector<pair<int,int> > WQHistMapEntryList;	//����

	vector<pair<int,int>> LQueue;	//����
	int nlabels;	//�ýڵ�ı�ǩ����	//������

	vector<pair<int, double>> PQueue;	//��ǩ����(��ǩ������)
	int isToUpdate;	//��һ�ֵ����Ƿ���Ҫ���±�ǩ
	int isChanged;	//���ֵ����ýڵ��ǩ�����Ƿ����ı�	//������

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

	NODE& operator=(const NODE& node);		//����

	virtual ~NODE();
	//----------------


};

#endif /* NODE_H_ */
