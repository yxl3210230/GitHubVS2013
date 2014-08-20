//============================================================================
// Name        : SLPA.cpp
// Author      : Jierui Xie (xiej2@rpi.edu)
// Date        : Nov. 5th, 2011
// Version     : v1.2
// Copyright   : All rights reserved.
// Description : SLPA algorithm for community detection.
// Web Site    : https://sites.google.com/site/communitydetectionslpa/
// Publication:
//             J. Xie, B. K. Szymanski and X. Liu, "SLPA: Uncovering Overlapping Communities in Social Networks via A Speaker-listener Interaction Dynamic Process", IEEE ICDM workshop on DMCCI 2011, Vancouver, CA.
//============================================================================

#include "SLPA.h"
#include "CommonFuns.h"
#include "CommonFuns_TMP.h"
#include "rndNumbers.h"
#include "fileOpts.h"

#include <pthread.h>


typedef std::tr1::unordered_map<int, int> UOrderedH_INT_INT;

bool SLPA::isDEBUG=false;

SLPA::SLPA(string inputFileName,vector<double> THRS,int maxRun,int maxT,string outputDir,bool isUseLargestComp,int numThreads) {
	//inputFileName: the full path
	//netName: short filename(non-suf)

	//---------------------------
	//Extract the fileName
	//---------------------------
	string a,b;
	fileName_net=inputFileName;
	extractFileName_FullPath(inputFileName,netName,a,b);

	networkPath="";
	net=new Net(networkPath,netName,fileName_net);

	//---------------------------
	//		GLPA parameters
	//---------------------------
	for(int i=0;i<THRS.size();i++)
		this->THRS.push_back(THRS[i]);   //why can not use [i]=.../???

	this->maxRun=maxRun;
	this->maxT=maxT;

	isSyn=false;
	this->isUseLargestComp=isUseLargestComp;
	//---------------------------
	//		more
	//---------------------------
	this->outputDir=outputDir;

	this->numThreads=numThreads;



	start();
}

SLPA::~SLPA() {
	delete net;
}

void SLPA::pre_initial_THRCS(){
	THRCS.clear();
	for(int i=0;i<THRS.size();i++){
		THRCS.push_back((int)myround(THRS[i]*maxT));
	}
}


void SLPA::start(){
	//---------------------------
	//  load network
	//---------------------------
	bool isSymmetrize=true; //symmetrize the edges

	net->readNetwork_EdgesList(fileName_net,isUseLargestComp,isSymmetrize);
	cout<<"Network info: N="<<net->N<< " M="<<net->M<<"(symmetric)"<<endl;
	cout<<"load "<<fileName_net<< " done.."<<endl;

	//net.showVertices();
	//net->showVertices_Table();

	//---------------------------
	//  convert thr to count_thr
	//---------------------------
	pre_initial_THRCS();

	//---------------------------
	//  	game
	//---------------------------
	for(int run=1;run<=maxRun;run++){
		//if(isDEBUG)
		cout<<" run="<<run<<"......"<<endl;

		//1.initial WQ and clear network
		//initWQueue_more();
		//initLQueue();
		initPQueue();

		//2.GLPA
		if(true){
			GLPA_syn();
		}
		else{
			GLPA_asyn_pointer();
		}

		//3.threshould and post-processing
		//a. create WQhistogram
		//st1=time(NULL);
		//post_createWQHistogram_MapEntryList();
		//cout<<"post_createWQHistogram_MapEntryList: "<<difftime(time(NULL),st1)<<endl;

		//b. thresholding and output cpm
		for(int i=0;i<THRCS.size();i++){
			int thrc=THRCS[i];
			double thrp=THRS[i];

			time_t st=time(NULL);
			cout<<"Progress: Thresholding thr="<<thrp<<"......."<<endl;
			string fileName=outputDir+"SLPA_"+netName+"_run"+int2str(run)+"_r"+dbl2str(thrp)+ ".icpm";

			if(isDEBUG) cout<<"cpm="<<fileName<<endl;
			post_threshold_createCPM_pointer(thrc,fileName);

			//cout<<" Take :" <<difftime(time(NULL),st)<< " seconds."<<endl;
		}
	}
}

void SLPA::initWQueue_more(){
	time_t st=time(NULL);
	cout<<"Progress: Initializing memory......."<<endl;

	//label is node id
	NODE *v;
	for(int i=0;i<net->N;i++){

		v=net->NODES[i];

		//label
		//v->WQueue.clear();
		v->WQueue.push_back(v->ID); //INITIALLY, there is **one**

		//other
		//v->WQHistMapEntryList.clear();


		//v->WQHistgram.clear();


	}

	cout<<" Take :" <<difftime(time(NULL),st)<< " seconds."<<endl;
}


void SLPA::initLQueue()
{
	time_t st=time(NULL);
	cout<<"Progress: Initializing memory......."<<endl;

	//label is node id
	NODE *v;
	for(int i=0;i<net->N;i++){

		v=net->NODES[i];
		v->LQueue.clear();
		v->LQueue.push_back(pair<int,int>(v->ID,1));
		v->nlabels=1;

	}

	cout<<" Take :" <<difftime(time(NULL),st)<< " seconds."<<endl;
}


void SLPA::initPQueue()
{
	time_t st = time(NULL);
	cout << "Progress: Initializing memory......." << endl;

	//label is node id
	NODE *v;
	for (int i = 0; i<net->N; i++){

		v = net->NODES[i];
		v->PQueue.clear();
		v->PQueue.push_back(pair<int, double>(v->ID, 1.0));
		v->isToUpdate = 1;
		v->isChanged = 0;

	}

	cout << " Take :" << difftime(time(NULL), st) << " seconds." << endl;
}


int SLPA::chooseLabel(NODE *v)
{
	int i;
	//int n=mtrand2.randInt(v->nlabels-1);
	if(v->LQueue.size()==1){
		return v->LQueue[0].first;
	}
	srand(time(NULL));
	int n=rand()%v->nlabels;
	for (i=0;i<v->LQueue.size();i++){
		n-=v->LQueue[i].second;
		if(n<0){
			return v->LQueue[i].first;
		}
	}
	cout<<"error!"<<endl;
	system("pause");
	return -1;
	


}


void SLPA::addLabeltoLQueue(NODE *v, int label)
{
	addLabeltoVectorINT_INT(v->LQueue,label);
	v->nlabels++;
}

void SLPA::addLabeltoVectorINT_INT(vector<pair<int,int>>& pairList, int label)
{
	int i,j,flag1,flag2;
	pair<int,int> tmp;
	flag1=0;
	for(i=0;i<pairList.size();i++){
		if (pairList[i].first==label){
			pairList[i].second++;
			if(i!=0){
				flag2=0;
				for(j=i-1;j>=0;j--){
					if(pairList[j].second>=pairList[i].second){
						if ((j+1)!=i){
							tmp=pairList[j+1];						
							pairList[j+1]=pairList[i];
							pairList[i]=tmp;
						}
						flag2=1;
						break;
					}
				}
				if(flag2==0){
					tmp=pairList[0];						
					pairList[0]=pairList[i];
					pairList[i]=tmp;
				}
			}
			flag1=1;
			break;
		}
	}
	if(flag1==0){
		pairList.push_back(pair<int,int>(label,1));
	}
}


int SLPA::selectMostFrequentLabel(vector<pair<int,int>>& pairList)
{
	vector<int> v;
	int maxv=pairList[0].second;
	v.reserve(100);
	v.push_back(pairList[0].first);
	for(int i=1;i<pairList.size();i++){
		if(pairList[i].second==maxv){
			v.push_back(pairList[i].first);
		}else{
			break;
		}
	}
	return v[rand()%v.size()];
}


void SLPA::deleteLabel(NODE *v, vector<pair<int,int>>& pairList)
{
	int flag1,flag2;
	vector<pair<int,int>>::iterator it;
	pair<int,int> tmp;

	int n=v->LQueue.size();

	for(int i=0;i<v->LQueue.size();i++){
		flag1=0;
		for(int j=0;j<pairList.size();j++){
			if(v->LQueue[i].first==pairList[j].first){
				flag1=1;
				break;
			}
		}
		if(flag1==0){
			v->LQueue[i].second--;
			if(v->LQueue[i].second==0){
				it=v->LQueue.begin()+i;
				v->LQueue.erase(it);
				i--;
			}else{
				if(i!=(v->LQueue.size()-1)){
					flag2=0;
					for(int j=i+1;j<v->LQueue.size();j++){
						if(v->LQueue[i].second<=v->LQueue[j].second){
							if(j!=i+1){
								tmp=v->LQueue[i];
								v->LQueue[i]=v->LQueue[j-1];
								v->LQueue[j-1]=tmp;
								i--;
							}
							flag2=1;
							break;
						}
					}
					if(flag2==0){
						tmp=v->LQueue[i];
						v->LQueue[i]=v->LQueue[v->LQueue.size()-1];
						v->LQueue[v->LQueue.size()-1]=tmp;
						i--;
					}
				}
			}
			v->nlabels--;
		}
	}
}


void SLPA::addLabeltoVectorINT_INT1(vector<pair<int,int>>& pairList, int label)
{
	int flag=0;
	for(int i=0;i<pairList.size();i++){
		if(label==pairList[i].first){
			pairList[i].second++;
			flag=1;
			break;
		}
	}
	if(flag==0){
		pairList.push_back(pair<int,int>(label,1));
	}
}


void SLPA::addLabeltoLQueue1(NODE *v, int label)
{
	addLabeltoVectorINT_INT1(v->LQueue,label);
	v->nlabels++;
}

void SLPA::deleteLabel1(NODE *v, vector<pair<int,int>>& pairList)
{
	int flag;
	for(int i=0;i<v->LQueue.size();i++){
		flag=0;
		for(int j=0;j<pairList.size();j++){
			if(v->LQueue[i].first==pairList[j].first){
				flag=1;
				break;
			}
		}
		if(flag==0){
			v->LQueue[i].second--;
			if(v->LQueue[i].second==0){
				v->LQueue.erase(v->LQueue.begin()+i);
				i--;
			}
			v->nlabels--;
		}
	}
}

void SLPA::addLabeltoVector(vector<pair<int, double>>& pairList, NODE *v)
{
	int i, j, flag;
	for (i = 0; i < v->PQueue.size(); i++){
		flag = 0;
		for (j = 0; j < pairList.size(); j++){
			if (v->PQueue[i].first == pairList[j].first){
				pairList[j].second += v->PQueue[i].second;
				flag = 1;
				break;
			}
		}
		if (flag == 0){
			pairList.push_back(v->PQueue[i]);
		}
	}
}

void SLPA::addLabeltoNode(vector<pair<int, double>>& pairList, NODE *v)
{
	int i, j, flag;
	for (i = 0; i < pairList.size(); i++){
		flag = 0;
		for (j = 0; j < v->PQueue.size(); j++){
			if (pairList[i].first == v->PQueue[j].first){
				v->PQueue[j].second += pairList[i].second;
				flag = 1;
				break;
			}
		}
		if (flag == 0){
			v->PQueue.push_back(pairList[i]);
		}
	}
	norm_probability(v->PQueue);
}


void SLPA::norm_probability(vector<pair<int, double>>& pairList)
{
	int i;
	double sum=0;
	for (i = 0; i < pairList.size(); i++){
		sum += pairList[i].second;
	}
	if (sum == 1.0){
		return;
	}
	for (i = 0; i < pairList.size(); i++){
		pairList[i].second /= sum;
	}
}


//void SLPA::thresholdLabelInNode(NODE *v)
//{
//	int i, j, n, m;
//	double threshold;
//	pair<int, double> tmp;
//	threshold = (double)1 / (3 * v->numNbs);
//	n = v->PQueue.size();
//	m = 0;
//	for (i = 0; i < n; i++){
//		if (v->PQueue[i].second < threshold){
//			if (i != n - 1){
//				tmp = v->PQueue[i];
//				v->PQueue[i] = v->PQueue[n - 1];
//				v->PQueue[n - 1] = tmp;
//				--n;
//				--i;
//			}
//			++m;
//		}
//	}
//	while (m--){
//		v->PQueue.pop_back();
//	}
//}

//void SLPA::thresholdLabelInNode(NODE *v)
//{
//	int i, j, n, m;
//	double tmp;
//	if (v->PQueue.size() <= v->numNbs){
//		return;
//	}
//	sortVectorInt_Double(v->PQueue);
//	tmp = v->PQueue[v->numNbs - 1].second;
//	i = v->numNbs;
//	while (i < v->PQueue.size()){
//		if (v->PQueue[i].second < tmp){
//			break;
//		}
//		++i;
//	}
//	i = v->PQueue.size()-i;
//	while (i--){
//		v->PQueue.pop_back();
//	}
//
//}

//void SLPA::thresholdLabelInNode(NODE *v)
//{
//	int i, j, n, m;
//	double tmp;
//	if (v->PQueue.size() <= v->numNbs){
//		return;
//	}
//
//	sortVectorInt_Double(v->PQueue);
//
//	tmp = v->PQueue[v->numNbs - 1].second;
//	for (i = v->numNbs; i < v->PQueue.size(); i++){
//		if (v->PQueue[i].second < tmp){
//			break;
//		}
//	}
//	if (i != v->PQueue.size()){
//		v->PQueue.erase(v->PQueue.begin() + i, v->PQueue.end());
//	}
//	
//	norm_probability(v);
//
//	for (i = 1; i < v->PQueue.size(); i++){
//		tmp = v->PQueue[i].second / v->PQueue[i - 1].second;
//		if (tmp < 0.5){
//			v->PQueue.erase(v->PQueue.begin() + i, v->PQueue.end());
//			norm_probability(v);
//			break;
//		}
//	}
//}

//void SLPA::thresholdLabelInNode(NODE *v)
//{
//	int i, j, n, m;
//	double tmp,sum;
//
//	sortVectorInt_Double(v->PQueue);
//
//	sum = v->PQueue[0].second;
//	for (i = 1; i < v->PQueue.size(); i++){
//		tmp = v->PQueue[i].second / v->PQueue[i - 1].second;
//		if (tmp < 0.2){
//			v->PQueue.erase(v->PQueue.begin() + i, v->PQueue.end());
//			break;
//		}
//		sum += v->PQueue[i].second;
//		if (sum > 0.61&&i != v->PQueue.size()-1){
//			v->PQueue.erase(v->PQueue.begin() + i + 1, v->PQueue.end());
//			break;
//		}
//	}
//	norm_probability(v->PQueue);
//}

void SLPA::thresholdLabelInNode(NODE *v)
{
	int i, j, n, m;
	double tmp, maxl;

	sortVectorInt_Double(v->PQueue);

	maxl = v->PQueue[0].first;
	for (i = 1; i < v->PQueue.size(); i++){
		if (v->PQueue[i].second == v->PQueue[0].second){
			continue;
		}
		if (v->PQueue[i].second < 0.1){
			v->PQueue.erase(v->PQueue.begin() + i, v->PQueue.end());
			break;
		}
	}
	norm_probability(v->PQueue);
}

void SLPA::thresholdLabelInVector(vector<pair<int, double>>& pairList, int n)
{
	int vol = n;
	if (pairList.size() > vol){
		sortVectorInt_Double(pairList);
		double tmp = pairList[vol - 1].second;
		while (vol != pairList.size()){ 
			if (tmp == pairList[vol].second){
				++vol;
			}
			else{
				break;
			}
		}
		pairList.erase(pairList.begin() + vol, pairList.end());
	}
}

void SLPA::labelinflation(NODE *v)
{
	for (int i = 0; i < v->PQueue.size(); i++){
		v->PQueue[i].second = pow(v->PQueue[i].second, 2);
	}
	norm_probability(v->PQueue);
}

//void SLPA::stateDetection(NODE *v)
//{
//	int i, j, k, l;
//	if (v->PQueue.size() == 1){
//		for (i = 0, k = 0; i < v->numNbs; i++){
//			if ((v->nbList_P[i]->PQueue.size() == 1) && (v->nbList_P[i]->PQueue[0].first == v->PQueue[0].first)){
//				++k;
//			}
//		}
//		if (k >(v->numNbs / 2)){
//			v->isToUpdate = 0;
//		}
//		else{
//			v->isToUpdate = 1;
//		}
//	}
//	else{
//		for (i = 0, k = 0; i < v->numNbs; i++){
//			if (v->nbList_P[i]->PQueue.size() >= v->PQueue.size()){
//				++k;
//			}
//		}
//		if ((k / v->numNbs) > 0.6){
//			for (i = 0, j = 0; i < v->numNbs; i++){
//				if (isSubSet(v->PQueue, v->nbList_P[i]->PQueue)){
//					++j;
//				}
//			}
//			if ((j / v->numNbs) > 0.6){
//				v->isToUpdate = 0;
//			}
//			else{
//				v->isToUpdate = 1;
//			}
//		}
//		else{
//			v->isToUpdate = 1;
//		}
//	}
//}

void SLPA::stateDetection(NODE *v)
{
	int i, j, k, l;

	for (i = 0, j = 0; i < v->numNbs; i++){
		if (isSubSet(v->PQueue, v->nbList_P[i]->PQueue)){
			++j;
		}
	}
	if (((double)j / v->numNbs) >= 0.6){
		v->isToUpdate = 0;
	}
	else{
		v->isToUpdate = 1;
	}
		
}

bool SLPA::checkLabelChange(NODE *v, vector<pair<int, double>> pairList)
{
	if (v->PQueue.size() != pairList.size()){
		v->isChanged = 1;
		return true;
	}
	if (v->PQueue.size() == 1 && pairList.size() == 1){
		if (v->PQueue[0].first != pairList[0].first){
			v->isChanged = 1;
			return true;
		}
		else{
			v->isChanged = 0;
			return false;
		}
	}
	sortVectorInt_Double_first(v->PQueue);
	sortVectorInt_Double_first(pairList);
	for (int i = 0; i < pairList.size(); i++){
		if (v->PQueue[i].first != pairList[i].first){
			v->isChanged = 1;
			return true;
		}
	}
	v->isChanged = 0;
	return false;
}

void SLPA::labelProportionate(vector<pair<int, double>>& pairList, double mix)
{
	for (int i = 0; i < pairList.size(); ++i){
		pairList[i].second *= mix;
	}
}
double SLPA::computeSimilarity(vector<pair<int, double>> set1, vector<pair<int, double>>& set2){
	norm_probability(set1);
	sortVectorInt_Double_first(set1);
	sortVectorInt_Double_first(set2);
	int i = 0, j = 0;
	double sum = 0, intersection = 0;
	while (i < set1.size() && j < set2.size()){
		if (set1[i].first < set2[j].first){
			sum += set1[i].second;
			++i;
		}
		else if (set1[i].first > set2[j].first){
			sum += set2[j].second;
			++j;
		}
		else {
			sum += (set1[i].second > set2[j].second) ? set1[i].second : set2[j].second;
			intersection += (set1[i].second < set2[j].second) ? set1[i].second : set2[j].second;
			++i;
			++j;
		}
	}
	if (i == set1.size() && j != set2.size()){
		for (; j < set2.size(); ++j){
			sum += set2[j].second;
		}
	}
	if (i != set1.size() && j == set2.size()){
		for (; i < set1.size(); ++i){
			sum += set1[i].second;
		}
	}
	return intersection / sum;

}

void SLPA::mixLabeltoNode(vector<pair<int, double>>& pairList, NODE *v)
{
	int i;
	double maxp = 0;
	for (i = 0; i < pairList.size(); ++i){
		if (maxp < pairList[i].second){
			maxp = pairList[i].second;
		}
	}
	for (i = 0; i < pairList.size(); ++i){
		pairList[i].second /= maxp;
	}

	maxp = 0;
	for (i = 0; i < v->PQueue.size(); ++i){
		if (maxp < v->PQueue[i].second){
			maxp = v->PQueue[i].second;
		}
	}
	for (i = 0; i < v->PQueue.size(); ++i){
		v->PQueue[i].second /= maxp;
	}

	addLabeltoNode(pairList, v);

}

void SLPA::computeCoefficients(vector<vector<double>>& co)
{
	int i, j, k, l, count;
	NODE *v, *nbv;
	vector<double> coe;
	double tmp;
	vector<int> nbl1, nbl2;

	co.clear();
	for (i = 0; i < net->N; ++i){
		v = net->NODES[i];
		tmp = ((double)v->numNbs*(v->numNbs - 1)) / 2;
		nbl1.clear();
		for (j = 0; j < v->numNbs; ++j){
			nbl1.push_back(v->nbList_P[j]->ID);
		}
		sort(nbl1.begin(), nbl1.end());
		coe.clear();
		for (j = 0; j < v->numNbs; ++j){
			nbv = v->nbList_P[j];
			nbl2.clear();
			for (k = 0; k < nbv->numNbs; ++k){
				nbl2.push_back(nbv->nbList_P[k]->ID);
			}
			sort(nbl2.begin(),nbl2.end());
			k = 0;
			l = 0;
			count = 0;
			while (k < nbl1.size() && l < nbl2.size()){
				if (nbl1[k] < nbl2[l]){
					++k;
				}
				else if (nbl1[k] > nbl2[l]){
					++l;
				}
				else{
					++k;
					++l;
					++count;
				}
			}
			coe.push_back((double)count / tmp);
		}
		co.push_back(coe);
	}
}

void SLPA::GLPA_syn()
{
	int i, j, k, t;
	time_t st = time(NULL);
	NODE *v, *nbv;
	vector<pair<int, double>> nbp, snapelement;
	vector<vector<pair<int, double>>> synlist, snapshot;
	vector<string> output;
	string line;
	vector<double> psim(net->N, 0);
	double sim;
	vector<int> scount(net->N, 0);
	bool endflag = false;
	vector<vector<double>> co;

	cout << "Start iteration:";

	for (i = 0; i < net->N; ++i){
		line += int2str(net->NODES[i]->ID);
		line += "\t";
	}
	output.push_back(line);

	computeCoefficients(co);

	for (t = 1; t <= maxT; t++){
		cout << "*";
		srand(time(NULL)); 
		//random_shuffle(net->NODES.begin(), net->NODES.end());
		synlist.clear();
		synlist.reserve(net->N);
		snapshot.clear();
		for (i = 0; i<net->N; i++){
			v = net->NODES[i];
			v->isChanged = 0;
			if (v->isToUpdate){
				nbp.clear();
				nbp.reserve(100);
				
				snapelement.clear();
				for (j = 0; j < v->PQueue.size(); j++){
					snapelement.push_back(v->PQueue[j]);
				}
				snapshot.push_back(snapelement);
				
				for (j = 0; j < v->numNbs; j++){
					nbv = v->nbList_P[j];
					addLabeltoVector(nbp, nbv);
				}
				//thresholdLabelInVector(nbp, v->numNbs);
				synlist.push_back(nbp);
			}
			else{
				synlist.push_back(vector<pair<int,double>>());
			}
		}
		line.clear();
		for (i = 0, j = 0, k = 0; i < net->N; ++i){
			v = net->NODES[i];
			//if (v->ID == 10)system("pause");
			if (v->isToUpdate){
				sim = computeSimilarity(synlist[i], v->PQueue);
				line += dbl2str(sim);
				line += "\t";
				if (sim == psim[i]){
					++scount[i];
				}
				else{
					psim[i] = sim;
					scount[i] = 0;
				}
				if (scount[i] != 5){
					//addLabeltoNode(synlist[i], v);
					mixLabeltoNode(synlist[i], v);
					labelinflation(v);
					thresholdLabelInNode(v);
					if (checkLabelChange(v, snapshot[j++]) == true){
						++k;
					}
				}
				else{
					v->isToUpdate = 0;
				}
			}
			else{
				line += "*\t";
			}
			//stateDetection(v);
			//line += int2str(v->isToUpdate);
			//line += " ";
		}
		output.push_back(line);

		if (k == 0 || endflag){
			//cout << "1";
			endflag = true;
			cout << "Quit?" << endl;
			if (getchar() == 'q'){
				break;
			}
		}

	}
	writeToTxt("output.txt", false, output);

	cout << endl;

	for(int i=0;i<net->N;i++){
		v = net->NODES[i];
		norm_probability(v->PQueue);
		sortVectorInt_Double(v->PQueue);
		//if (v->PQueue.size() > 1){
		//	cout << v->ID << " : ";
		//}
		for (j = 0; j < v->PQueue.size(); j++){
			//if (v->PQueue.size() > 1){
			//	cout << v->PQueue[j].first << "(" << v->PQueue[j].second << ") ";
			//}
			v->LQueue.push_back(pair<int, int>(v->PQueue[j].first, (int)((v->PQueue[j].second * 100) + 0.5)));
		}
		//if (v->PQueue.size() > 1){
		//	cout << endl;
		//}
	}

	cout << "Iteration(" << t << ") is over (takes " << difftime(time(NULL), st) << " seconds)" << endl;
}

void SLPA::GLPA_asyn_pointer(){
	//pointer version:
	//	 store the pointer of nb in *nbList_P*
	//   save time for retrieving hashTable
	time_t st=time(NULL);
	dt1=dt2=dt3=dt4=0;

	NODE *v,*nbv;
	int label;
	vector<int> nbWs;
	vector<pair<int, int>> nbWs1;
	vector<pair<int, double>> nbp;

	//t=1 because we initialize the WQ(t=0)
	cout<<"Start iteration:";

	for(int t=1;t<maxT;t++){
		//1.shuffle
		//cout<<"-------------t="<<t<<"---------------------"<<endl;
		//cout<<"*"<<flush;
		cout<<"*";
		srand (time(NULL)); // ***YOU need to use this, such that you can get a new one each time!!!!! seed the random number with the system clock
		random_shuffle (net->NODES.begin(), net->NODES.end());
		//net->showVertices();


		//2. do one iteration-asyn
		for(int i=0;i<net->N;i++){

			v=net->NODES[i];


			//a.collect labels from nbs
			//nbWs.clear();
			//nbWs1.clear();
			//nbWs1.reserve(100);
			nbp.clear();
			nbp.reserve(100);
			for(int j=0;j<v->numNbs;j++){
				nbv=v->nbList_P[j];
				//nbWs.push_back(nbv->WQueue[mtrand2.randInt(nbv->WQueue.size()-1)]);
				//addLabeltoVectorINT_INT(nbWs1,chooseLabel(nbv));

				//st1=time(NULL);
				//int randnum=mtrand2.randInt(nbv->WQueue.size()-1);
				//srand (time(NULL));
				//int randnum=rand()%nbv->WQueue.size();
				//int randnum=chooseLabel(nbv);
				//dt1+=difftime(time(NULL),st1);

				//st1=time(NULL);
				//addLabeltoVectorINT_INT(nbWs1,nbv->WQueue[randnum]);
				//addLabeltoVectorINT_INT(nbWs1,randnum);
				//dt2+=difftime(time(NULL),st1);

				addLabeltoVector(nbp, nbv);

			}
			//when call addLabeltoVectorINT_INT1
			//sortVectorInt_Int(nbWs1);

			//b.select one of the most frequent label
			//st1=time(NULL);
			//label=ceateHistogram_selRandMax(nbWs);
			//label=selectMostFrequentLabel(nbWs1);
			//dt3+=difftime(time(NULL),st1);


			//c. update the WQ **IMMEDIATELY**
			//st1=time(NULL);
			//v->WQueue.push_back(label);
			//addLabeltoLQueue(v,label);
			//deleteLabel(v,nbWs1);
			//dt4+=difftime(time(NULL),st1);

			addLabeltoNode(nbp, v);
			norm_probability(v->PQueue);
			thresholdLabelInNode(v);
		}

		//cout<<" Take :" <<difftime(time(NULL),st)<< " seconds."<<endl;
	}

	//for(int i=0;i<net->N;i++){
	//	sortVectorInt_Int(net->NODES[i]->LQueue);
	//}

	cout<<endl;
	cout<<"Iteration is over (takes "<<difftime(time(NULL),st)<< " seconds)"<<endl;
	cout<<"random_shuffle: "<<dt1<<endl;
	cout<<"chooseLabel: "<<dt2<<endl;
	cout<<"selectMostFrequentLabel: "<<dt3<<endl;
	cout<<"addLabeltoLQueue: "<<dt4<<endl;
}

int SLPA::ceateHistogram_selRandMax(const vector<int>& wordsList){
	int label;
	map<int,int> hist;
	map<int,int>::iterator mit;
	//------------------------------------------
	//	    1. create the histogram
	//------------------------------------------
	//count the number of Integer in the wordslist
	createHistogram(hist, wordsList);

	//------------------------------------------
	//2. randomly select label(key) that corresponding to the max *values*.
	//	    sort the key-value pairs, find the multiple max
	//		randomly select one and return the label(key)
	//------------------------------------------
	//***list is int he decreasing order of value.
	vector<pair<int,int> > pairlist;
	sortMapInt_Int(hist, pairlist);

	//for(Map.Entry en : list) {
	//	System.out.printf("  %-8s%d%n", en.getKey(), en.getValue());
	//}
	//cout<<"-------------------"<<endl;
	//for(int i=0;i<pairlist.size();i++){
	//	cout<<"w="<<pairlist[i].first<<" count="<<pairlist[i].second<<endl;
	//}
	int maxv=pairlist[0].second;
	int cn=1;

	//cout<<"maxv="<<maxv<<endl;
	for(int i=1;i<pairlist.size();i++){    //start from the **second**
		if(pairlist[i].second==maxv)       //multiple max
			cn++;
		else
			break; //stop when first v<maxv
	}


	if(cn==1)
		label=pairlist[0].first;         //key-label
	else{
		//generator.nextInt(n); 0~n-1
		//int wind=rndDblBtw0Nminus1(cn);
		int wind=mtrand1.randInt(cn-1); //**[0~n]
		//cout<<"*****wind="<<wind<<endl;


		label=pairlist[wind].first;
	}
	//cout<<"cn="<<cn<<endl;
	//cout<<"label="<<label<<endl;

	return label;
}


//void SLPA::post_createWQHistogram_MapEntryList(){
//	NODE *v;
//	map<int,int> WQHistgram;
//
//	for(int i=0;i<net->N;i++){
//		v=net->NODES[i];
//
//		//use WQueue to create histogram
//		WQHistgram.clear();
//		createHistogram(WQHistgram, v->WQueue);
//
//		//list is in the ***decreasing*** order of value(count).
//		//use histogram to create a pair list
//		v->WQHistMapEntryList.clear();
//		sortMapInt_Int(WQHistgram, v->WQHistMapEntryList);
//
//		//MEMORY::remove v->WQueue to
//		v->WQueue.clear();
//	}
//
//	//cout<<"Progress: Created mapEntryList ......."<<endl;
//}

void SLPA::post_createWQHistogram_MapEntryList(){
	NODE *v;

	for(int i=0;i<net->N;i++){
		v=net->NODES[i];
		v->WQHistMapEntryList.clear();
		for (int j=0;j<v->WQueue.size();j++){
			addLabeltoVectorINT_INT(v->WQHistMapEntryList,v->WQueue[j]);
		}
		v->WQueue.clear();
	}

	//cout<<"Progress: Created mapEntryList ......."<<endl;
}

void SLPA::dothreshold_createCPM_pointer(int thrc,vector<vector<int>* >& cpm){
	time_t st=time(NULL);

	//the map of distinct label and community id
	map<int,int> w_comidTable;
	map<int,int>::iterator mit;

	int comid=-1; //**-1, such that we can access via vector[comid]

	NODE *v;
	int ov_cn=0;
	for(int i=0;i<net->N;i++){
		v=net->NODES[i];

		//1.get the world list after threshoulding
		vector<int> WS;  //w list that beyond the thrc

		//vector<pair<int,int> > pairList;
		//sortMapInt_Int(v->MQueue,pairList);
		//post_thresholding(pairList,thrc,WS);

		post_thresholding(v->LQueue,thrc,WS);

		//post_thresholding(v->WQHistMapEntryList,thrc,WS); //***TO IMP

		if(WS.size()<1) cout<<"ERROR:empty WS"<<endl;
		if(WS.size()>1) ov_cn++;

		//2. create CPM:put each membership to a community
		for(int j=0;j<WS.size();j++){
			int label=WS[j];

			//------------
			if(w_comidTable.count(label)==0){//not in yet
				comid++;
				w_comidTable.insert(pair<int,int>(label, comid)); //**

				//cpm.push_back(vector<int>());  //copy to the (an empty vector)

				//***CPMPP
				vector<int>* avector=new vector<int>();  //TO REMOVE
				cpm.push_back(avector);  //copy to the (an empty vector)
			}

			//------------
			mit=w_comidTable.find(label);
			int v_comid=mit->second;

			//cpm[v_comid].push_back(v->ID);  //add one id
			cpm[v_comid]->push_back(v->ID);  //add one id
		}
	}

	//cout<<"Creating CPM takes :" <<difftime(time(NULL),st)<< " seconds."<<endl;
}



void SLPA::post_threshold_createCPM_pointer(int thrc,string fileName){
	bool isDEBUG=false;
	time_t st;

	//CPM: the index is the **commID(0~k-1)**, and vales is node **ID**
	vector<vector<int>* > cpm; //***CPMP, TO REMOVE

	//=========================================
	//1.threshold + createCPM
	//=========================================
		dothreshold_createCPM_pointer(thrc,cpm);


	//=========================================
	//2.***post process*** the communities CPM
	//=========================================
	//a. reassign sameLabel disconnected subcomponent
	//(handle the same label disconnected components problems in general)

	/*if(false) {
		if(isDEBUG) cout<<"---before reassign---"<<endl;
		if(isDEBUG) printVectVect_PRIMITIVE<int>(cpm);
		cpm=post_sameLabelDisconnectedComponents(cpm); //**TO IMPROVE
		if(isDEBUG) cout<<"---After reassign---"<<endl;
		if(isDEBUG) printVectVect_PRIMITIVE<int>(cpm);
	}*/

	//---------------------------
	//b. remove subset
	//---------------------------
	//if(isDEBUG) cout<<"---before---"<<endl;
	//if(isDEBUG) printVectVect_PRIMITIVE<int>(cpm);

	st=time(NULL);
	//cpm=post_removeSubset(cpm);                    //**TO IMPROVE
	//cpm=post_removeSubset_HashTable(cpm);
	//cpm=post_removeSubset_UorderedHashTable(cpm);

	//**working single thread version
	if(numThreads==0){
		cpm=post_removeSubset_UorderedHashTable_cpmpointer(cpm);
	}
	else{	// multi threads
		cpm=post_removeSubset_UorderedHashTable_cpmpointer_MultiThd(cpm);
	}

	cout<<"removeSubset takes :" <<difftime(time(NULL),st)<< " seconds."<<endl;

	//if(isDEBUG) cout<<"---After---"<<endl;
	//if(isDEBUG) printVectVect_PRIMITIVE<int>(cpm);

	//---------------------------
	//4. save cpm
	//---------------------------
	st=time(NULL);
	sort_cpm_pointer(cpm);  //sort each com by increasing ID for; and by decrasing community size
	//cout<<"sorting takes :" <<difftime(time(NULL),st)<< " seconds."<<endl;

	st=time(NULL);
	write2txt_CPM_pointer(fileName,cpm);
	//cout<<"write2tx takes :" <<difftime(time(NULL),st)<< " seconds."<<endl;

	//if(isDEBUG)  show_cpm(cpm);


	//---------------------------
	//release memory
	//---------------------------
	for(int i=0;i<cpm.size();i++)
		delete cpm[i];

}


void SLPA::post_thresholding(vector<pair<int,int> >& pairList, int thrc, vector<int>& WS){
	//For label with count<=THRESHOULD **COUNT**,
	//we remove it from the hist(here is represented by WQHistMapEntryList) then
	//some nodes may become ***unlabeled***.if a node becomes unlabeled,
	//   keep the most frequent label in its list
	//   RETURN: the labels after threshoulding
	int label;

	//cout<<"------------------thrc="<<thrc<<endl;
	//for(int i=0;i<pairList.size();i++)
	//	cout<<"  "<< pairList[i].first<<" cout="<< pairList[i].second<<endl;

	//*****list MUST BE already ordered in **decreasing count order.****

	int maxv=pairList[0].second; //first one is max count

	if(maxv<=thrc){//keep one label to avoid unlabeled node randomly
		// collect the max count
		int cn=1;
		for(int i=1;i<pairList.size();i++){              //start from the **second**
			if(pairList[i].second==maxv)//multiple max
				cn++;
			else
				break; //stop when first v<maxv
		}

		// handle the multiple max counts
		if(cn==1)
			label=pairList[0].first;  //key
		else{
			//generator.nextInt(n); 0~n-1
			int wind=mtrand2.randInt(cn-1);
			//cout<<"wind="<<wind<<endl;
			label=pairList[wind].first;  //key randInt->[0~n]
		}

		//add one
		WS.push_back(label);

	}
	else{
		//go down the list until below the thrc
		for(int i=0;i<pairList.size();i++){              //start from the **first**
			if(pairList[i].second>thrc){                 //cout**Threshold**
				label=pairList[i].first;				 //key
				WS.push_back(label);
			}
			else
				break;									//stop when first v<thrc
		}
	}
}

void SLPA::sort_cpm_pointer(vector<vector<int>* >& cpm){
	//inside each com, sort ID by **increasing** order
	for(int i=0;i<cpm.size();i++){
		//sort(cpm[i].begin(),cpm[i].end(),sort_INT_INC());
		sort(cpm[i]->begin(),cpm[i]->end(),sort_INT_INC());
	}

	//each com, sort **decreasing** community size
	sortVecVec_bySize_pointer<int>(cpm);
}


vector<vector<int>* > SLPA::post_removeSubset_UorderedHashTable_cpmpointer(vector<vector<int>* >& cpm){
	time_t st;
	//bool isDEBUG=true;
	//if(isDEBUG) cout<<"removeSubset (Unordered HASH)............."<<endl;

	vector<vector<int>* > newcpm;


	//1. ***sort cpm by the community size(***decreasing***)
	st=time(NULL);
	sort_cpm_pointer(cpm);  //***CMPP

	//cout<<"sort_cpm takes :" <<difftime(time(NULL),st)<< " seconds."<<endl;

	//2.check the subset relationship
	//cout<<"***before cpm.sie="<<cpm.size()<<endl;
	st=time(NULL);

	//2.1 vector of map corresponding to the sorted cpm(decreasing)
	vector<UOrderedH_INT_INT* > vectHTable;

	for(int i=0;i<cpm.size();i++){
		UOrderedH_INT_INT* H=new UOrderedH_INT_INT;  //**
		for(int j=0;j<cpm[i]->size();j++)            //***CMPP
			//H->insert(pair<int,int>(cpm[i][j],cpm[i][j])); //id as both key and value
			H->insert(pair<int,int>((*cpm[i])[j],(*cpm[i])[j])); //id as both key and value

		vectHTable.push_back(H);
	}

	//0.the indicator
	//vector<int> indicators;
	//for(int i=0;i<cpm.size();i++)
	//	indicators.push_back(1);  //1-default ok
	int *indicators=new int[cpm.size()];
	for(int i=0;i<cpm.size();i++)
		indicators[i]=1;  		  //1-default ok

	//2.2 find the subset (compare smaller cpmi to largest H(j) first)
	bool issubset;
	for(int i=cpm.size()-1;i>0;i--){
		for(int j=0;j<i;j++){
			//visit all coms(in HASH) that are LARGER than it
			//check if cpm(i) is a subset of H(j)
			issubset=true;
			for(int k=0;k<cpm[i]->size();k++)
				if(vectHTable[j]->count((*cpm[i])[k])==0){//not found
					issubset=false;
					break;
				}

			//issubset=issubset_cpm_hash(cpm[i],vectHTable[j]);

			if(issubset){  //remove this cpm
				indicators[i]=0;   //**change i**
				break;
			}
		}
	}

	//3.newcpm
	for(int i=0;i<cpm.size();i++){
		if(indicators[i]==0) continue;

		newcpm.push_back(cpm[i]);
	}

	//release memory
	for(int i=0;i<vectHTable.size();i++)
		delete vectHTable[i];

	return newcpm;
}

void SLPA::write2txt_CPM_pointer(string fileName,vector<vector<int>* >& cpm) {
	vector<string> data;

	for(int i=0;i<cpm.size();i++){
		//vector<int>& oneComm=cpm[i]; //ref
		vector<int>& oneComm=*cpm[i]; //**CPMP ref

		string line;
		for(int j=0;j<oneComm.size();j++){
			line+=int2str(oneComm[j]);
			line+=" ";
		}
		data.push_back(line);
	}

	//fileOpts.writeToTxt(fileName, false, data);// **false:the first one
	writeToTxt(fileName, false, data);
}

//------------------------------------------------
//			org cpm functions
//------------------------------------------------
void SLPA::sort_cpm(vector<vector<int> >& cpm){
	//inside each com, sort ID by **increasing** order
	for(int i=0;i<cpm.size();i++){
		sort(cpm[i].begin(),cpm[i].end(),sort_INT_INC());
	}

	//each com, sort **decreasing** community size
	sortVecVec_bySize<int>(cpm);
}

//------------------------------------------------
//			Multi-threading
//------------------------------------------------
void SLPA::decomposeTasks(int numTasks,int numThd,int stInds[],int enInds[]){
	int rem=numTasks%numThd;
	int step=(numTasks-rem)/numThd;  //**TO IMPROVE

	for(int i=0;i<numThd;i++){
		stInds[i]=i*step;
		enInds[i]=(i+1)*step-1;
	}
	enInds[numThd-1]+=rem;

	if(false){
		cout<<"----------------decomposeTasks---------------"<<endl;
		cout<<"rem="<<rem<<" step="<<step<<endl;
		for(int i=0;i<numThd;i++){
			cout<<stInds[i]<<" "<<enInds[i]<<endl;
		}
	}

}
void *SLPA::removesubset_onethread(void *threadarg){
	// set the corresponding element in indicators
	// and return the pointer

	//We use pointers:
	//***ASSUMING the my_data->cpm do the shallow copy(**pointers**) from the original one
	//   then we can the following

	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;

	if(false) cout<<"startind="<<my_data->startind<<" endind="<<my_data->endind<<endl;

	//----------------------
	//references of the **shallow** copy in thread_data
	vector<vector<int>* >& cpm=my_data->cpm;
	vector<UOrderedH_INT_INT* >& vectHTable=my_data->vectHTable;
	int *indicators=my_data->pIndicator;    //*the array name as pointer


	//2.2 find the subset (compare smaller cpmi to largest H(j) first)
	bool issubset;
	//for(int i=cpm.size()-1;i>0;i--){
	//**ONLY in stinds~eninds**
	for(int z=my_data->endind;z>=my_data->startind;z--){
		int i=z;

		//------same as before----
		for(int j=0;j<i;j++){
			//visit all coms(in HASH) that are LARGER than it
			//check if cpm(i) is a subset of H(j)
			issubset=true;
			for(int k=0;k<cpm[i]->size();k++)
				if(vectHTable[j]->count((*cpm[i])[k])==0){//not found
					issubset=false;
					break;
				}

			//issubset=issubset_cpm_hash(cpm[i],vectHTable[j]);

			if(issubset){          //remove this cpm
				indicators[i]=0;   //**change i**
				break;
			}
		}
	}

	//----------------------
	//for(int i=my_data->startind;i<=my_data->endind;i++)
	//		my_data->pIndicator[i]=i;

	pthread_exit(NULL);
	return 0;
}



vector<vector<int>* > SLPA::post_removeSubset_UorderedHashTable_cpmpointer_MultiThd(vector<vector<int>* >& cpm){
	time_t st;
	bool isDEBUG=false;
	cout<<"removeSubset (Multiple threads)............."<<endl;

	vector<vector<int>* > newcpm;


	//1. ***sort cpm by the community size(***decreasing***)
	st=time(NULL);
	sort_cpm_pointer(cpm);  //***CMPP

	//cout<<"sort_cpm takes :" <<difftime(time(NULL),st)<< " seconds."<<endl;

	//2.check the subset relationship
	//cout<<"***before cpm.sie="<<cpm.size()<<endl;
	st=time(NULL);

	//2.1 vector of map corresponding to the sorted cpm(decreasing)
	vector<UOrderedH_INT_INT* > vectHTable;

	for(int i=0;i<cpm.size();i++){
		UOrderedH_INT_INT* H=new UOrderedH_INT_INT;  //**
		for(int j=0;j<cpm[i]->size();j++)            //***CMPP
			//H->insert(pair<int,int>(cpm[i][j],cpm[i][j])); //id as both key and value
			H->insert(pair<int,int>((*cpm[i])[j],(*cpm[i])[j])); //id as both key and value

		vectHTable.push_back(H);
	}

	//===========================================
	int *indicators=new int[cpm.size()];
	for(int i=0;i<cpm.size();i++)
		indicators[i]=1;  		  //1-default ok

	int numTasks=cpm.size();
	int numThd=numThreads;       //****

	int *stInds=new int[numThd];
	int *enInds=new int[numThd];

	decomposeTasks(numTasks, numThd, stInds, enInds);
	//------------------------------------------------
	struct thread_data *thread_data_array=new struct thread_data[numThd];

	pthread_t *threads=new pthread_t[numThd];    //**
	pthread_attr_t attr;
	void *status;

	// Initialize and set thread detached attribute
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int rc;
	long t;
	for( t=0; t<numThd; t++){
		if(isDEBUG) cout<<"creating thread "<<t<<endl;
		thread_data_array[t].startind=stInds[t];
		thread_data_array[t].endind=enInds[t];
		thread_data_array[t].pIndicator=indicators;   //**TO change in function**
		thread_data_array[t].cpm=cpm;                //**shallow copy**
		thread_data_array[t].vectHTable=vectHTable;   //**shallow copy**

		rc = pthread_create(&threads[t], NULL, removesubset_onethread, (void *) &thread_data_array[t]);
		if (rc){
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	// Free attribute and wait for the other threads
	pthread_attr_destroy(&attr);

	//**This determines the order
	for(t=numThd-1; t>=0; t--) {
		rc=pthread_join(threads[t], &status);
		if (rc) {
			cout<<"ERROR; return code from pthread_join() is "<<rc<<endl;
			exit(-1);
		}
	}

	//------------------------------------------------
	if(isDEBUG) for(int i=0;i<cpm.size();i++)
		cout<<"indicator["<<i<<"]="<<indicators[i]<<endl;

	//===========================================
	//3.newcpm
	for(int i=0;i<cpm.size();i++){
		if(indicators[i]==0) continue;

		newcpm.push_back(cpm[i]);
	}

	//release memory
	for(int i=0;i<vectHTable.size();i++)
		delete vectHTable[i];

	return newcpm;
}
