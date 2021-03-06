/*
 * CommonFuns.h
 *
 *  Created on: Oct 14, 2011
 *      Author: Jerry
 */

#ifndef COMMONFUNS_H_
#define COMMONFUNS_H_

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <utility>
#include <string>

#include <ctime> // time()
#include <cmath>
#include <unordered_set>

using namespace std;



struct sort_pair_INT_INT {
	bool operator()(const std::pair<int,int> &i, const std::pair<int,int> &j) {
		return i.second > j.second;
	}
};//sort_pair_intint_dec;

struct sort_INT_DEC {
	bool operator()(int i, int j) {
		return i > j;
	}
};

struct sort_INT_INC {
	bool operator()(int i, int j) {
		return i < j;
	}
};//sort_int_inc;




//-----------------
void sortVectorInt_Int(vector<pair<int,int> > & wordsvec);
void sortMapInt_Int( map<int,int> & words, vector< pair<int,int> >& wordsvec);
void createHistogram(map<int,int>& hist, const vector<int>& wordsList);

double myround(double value);
string int2str(int i);
string dbl2str(double f);

#endif /* COMMONFUNS_H_ */
