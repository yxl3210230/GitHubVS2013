//============================================================================
// Name        : NODE.cpp
// Author      : Jierui Xie (xiej2@rpi.edu)
// Date        : Oct. 2011
// Version     : v1.0
// Copyright   : All rights reserved.
// Description : SLPA algorithm for community detection.
// Web Site    : https://sites.google.com/site/communitydetectionslpa/
// Publication:
//             J. Xie, B. K. Szymanski and X. Liu, "SLPA: Uncovering Overlapping Communities in Social Networks via A Speaker-listener Interaction Dynamic Process", IEEE ICDM workshop on DMCCI 2011, Vancouver, CA.
//============================================================================


#include "NODE.h"

NODE::NODE() {
	// TODO Auto-generated constructor stub

}

NODE::~NODE() {
	// TODO Auto-generated destructor stub
}

NODE& NODE::operator=(const NODE& node){
	if (this == &node)//如果自己给自己赋值则直接返回
	{
		return *this;
	}
	ID = node.ID;
	numNbs = node.numNbs;
	nbList_P = node.nbList_P;
	nbSet = node.nbSet;
	PQueue = node.PQueue;
	isToUpdate = node.isToUpdate;

	status = 0;
	t = 0;
	return *this;
}
