//============================================================================
// Name        : CommonFuns.cpp
// Author      : Jierui Xie (xiej2@rpi.edu)
// Date        : Oct. 2011
// Version     :
// Copyright   : All rights reserved.
// Description : SLPA algorithm for community detection.
// Web Site    : https://sites.google.com/site/communitydetectionslpa/
// Publication:
//             J. Xie, B. K. Szymanski and X. Liu, "SLPA: Uncovering Overlapping Communities in Social Networks via A Speaker-listener Interaction Dynamic Process", IEEE ICDM workshop on DMCCI 2011, Vancouver, CA.
//============================================================================

#include "CommonFuns.h"

using namespace std;



//-------------------------------------



void sortVectorInt_Int(vector<pair<int,int> > & wordsvec)
{
	sort (wordsvec.begin(), wordsvec.begin()+wordsvec.size(), sort_pair_INT_INT());
}

void sortVectorInt_Double(vector<pair<int, double> > & wordsvec)
{
	sort(wordsvec.begin(), wordsvec.begin() + wordsvec.size(), sort_pair_INT_DOUBLE());
}

void sortVectorInt_Double_first(vector<pair<int, double> > & wordsvec)
{
	sort(wordsvec.begin(), wordsvec.begin() + wordsvec.size(), sort_pair_INT_DOUBLE_first());
}

bool isSubSet(vector < pair<int, double> > set1, vector < pair<int, double> > set2)
{
	if (set1.size() > set2.size()){
		return false;
	}
	
	int diff = set2.size() - set1.size();

	sortVectorInt_Double_first(set1);
	sortVectorInt_Double_first(set2);
	
	int i = 0, j = 0;
	while (i < set1.size() && j < set2.size()){
		if (set1[i].first > set2[j].first){
			++j;
			--diff;
		}
		else if (set1[i].first < set2[j].first){
			return false;
		}
		else{
			++i;
			++j;
		}

		if (diff < 0){
			return false;
		}
	}

	return true;
}




void sortMapInt_Int(map<int,int> & words, vector<pair<int,int> > & wordsvec){
	//Use the map to create a sorted pair vector
	//-------------------------------------
	//  map->vector->sort pair
	//-------------------------------------
	for ( map<int,int>::iterator it=words.begin() ; it != words.end(); it++ ){
		wordsvec.push_back(*it);//**
	}
	//-------------------
	sort (wordsvec.begin(), wordsvec.begin()+wordsvec.size(), sort_pair_INT_INT());
}


//----------------------------------------
double myround( double value ){
	return floor( value + 0.5 );
}

//void createHistogram(map<int,int>& hist, const vector<int>& wordsList){
//	//create histogram hist from a wordsList
//	hist.clear();
//	map<int,int>::iterator mit;
//
//	for(int i=0;i<wordsList.size();i++){
//		int key=wordsList[i];
//
//		if(hist.count(key)>0){
//			//increasing the count
//			mit=hist.find(key);
//			int count=mit->second+1; //count
//
//			//reinsert into the hist
//			hist.erase (mit);		//**insert() do not change the value
//			hist.insert(pair<int,int>(key,count));
//		}
//		else{
//			//set the count to 1
//			hist.insert(pair<int,int>(key,1));
//		}
//	}
//
//}

void createHistogram(map<int,int>& hist, const vector<int>& wordsList){
	//create histogram hist from a wordsList
	hist.clear();
	map<int,int>::iterator mit;

	for(int i=0;i<wordsList.size();i++){
		int key=wordsList[i];
		mit=hist.find(key);
		if(mit!=hist.end()){
			mit->second++;
		}
		else{
			hist.insert(pair<int,int>(key,1));
		}
	}

}


string int2str(int i){
	string s;
	stringstream out;
	out << i;
	s = out.str();

	return s;
}

string dbl2str(double f){
	string s;
	stringstream out;
	out << f;
	s = out.str();

	return s;
}



