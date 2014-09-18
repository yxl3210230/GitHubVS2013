#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include<Windows.h>
#include<algorithm>
#include"NMI.h"

using namespace std;

vector<vector<int> > corvertex, comvertex;


int readCommunities(string fileName, RESULT_COMMUNITY *comp){
	

	string oneLine,stmp, whiteSpace=" \t\n";
	fstream fp;
	stringstream linestream;
	int i,tmp;

	vector<int> v;
	vector<vector<int> > nv;

	fp.open(fileName.c_str(),fstream::in); //|fstream::out|fstream::app

	if(fp.is_open()){//if it does not exist, then failed

		// repeat until all lines is read
		while ( fp.good()){
			getline (fp,oneLine);

			//--------------------------
			//skip empty line
			if(oneLine.find_first_not_of(whiteSpace)==string::npos) continue;

			//skip any line NOT starting with digit number
			if(!isdigit(oneLine.at(oneLine.find_first_not_of(whiteSpace)))) continue;

			//stringstream linestream(oneLine);
			linestream.clear();
			//linestream.str("");
			linestream<<oneLine;
			linestream >> stmp;
			v.clear();
			if (stmp.at(stmp.length() - 1) != ':'){
				v.push_back(atoi(stmp.c_str()));
			}
			while(linestream>>tmp){
				v.push_back(tmp);
			}
			sort(v.begin(), v.end());
			nv.push_back(v);
		} //while

		fp.close();
	}
	else{
		cout<<"open failed"<<endl;
		return 0;
	}

	comvertex.assign(nv.begin(), nv.end());

	comp->ncommunities=nv.size();
	comp->nvertices=new int[nv.size()];
	comp->vertices=new int*[nv.size()];

	for(i=0;i<nv.size();i++){
		comp->nvertices[i]=nv[i].size();
		comp->vertices[i]=new int[nv[i].size()];
		for(tmp=0;tmp<nv[i].size();tmp++){
			comp->vertices[i][tmp]=nv[i][tmp]-1;
		}
	}

	return 1;
}

int community_statistics(RESULT_VERTEX *result1, RESULT_COMMUNITY * result2)
{
	int i,j,num;
	int *count,*flag;

	count=(int *)calloc(result1->nvertices,sizeof(int));

	for(i=0;i<result1->nvertices;i++){
		for(j=0;j<result1->numbelong[i];j++){
			count[result1->communities[i][j]-1]++;
		}
	}

	for(i=0,num=0;i<result1->nvertices;i++){
		if(count[i]!=0)
			num++;
	}

	result2->ncommunities=num;
	result2->nvertices=(int *)malloc(num*sizeof(int));
	result2->vertices=(int **)malloc(num*sizeof(int*));

	for(i=0,j=0;i<result1->nvertices;i++){
		if(count[i]){
			result2->nvertices[j]=count[i];
			count[i]=j++;
		}
	}

	for(i=0;i<num;i++){
		result2->vertices[i]=(int *)malloc(result2->nvertices[i]*sizeof(int));
	}

	flag=(int *)calloc(num,sizeof(int));

	for(i=0;i<result1->nvertices;i++){
		for(j=0;j<result1->numbelong[i];j++){
			result2->vertices[count[result1->communities[i][j]-1]][flag[count[result1->communities[i][j]-1]]++]=i;
		}
	}

	free(count);
	free(flag);
	return 1;
}

int readCommunitiesComp(string fileName, RESULT_COMMUNITY *comp){
	

	string oneLine, whiteSpace=" \t\n";
	fstream fp;
	stringstream linestream;
	int i,j,tmp=0;

	vector<int> v;
	vector<vector<int>> nv;

	RESULT_VERTEX *comp1;
	comp1=new RESULT_VERTEX;

	fp.open(fileName.c_str(),fstream::in); //|fstream::out|fstream::app

	if(fp.is_open()){//if it does not exist, then failed

		// repeat until all lines is read
		while ( fp.good()){
			getline (fp,oneLine);

			//--------------------------
			//skip empty line
			if(oneLine.find_first_not_of(whiteSpace)==string::npos) continue;

			//skip any line NOT starting with digit number
			if(!isdigit(oneLine.at(oneLine.find_first_not_of(whiteSpace)))) continue;

			//stringstream linestream(oneLine);
			linestream.clear();
			//linestream.str("");
			linestream<<oneLine;
			v.clear();
			if((linestream>>i)&&(linestream>>j)){
				if (i!=tmp+1)
				{
					cout<<"file format error!"<<endl;
					system("pause");
					exit(1);
				}
				v.push_back(j);
				while (linestream>>j)
				{
					v.push_back(j);
				}
				nv.push_back(v);
				tmp=i;
			}
			
		} //while

		fp.close();
	}
	else{
		cout<<"open failed"<<endl;
		system("pause");
		exit(1);
	}

	corvertex.assign(nv.begin(), nv.end());

	comp1->nvertices=nv.size();
	comp1->numbelong=new int[nv.size()];
	comp1->communities=new int*[nv.size()];

	for(i=0;i<nv.size();i++){
		comp1->numbelong[i]=nv[i].size();
		comp1->communities[i]=new int[nv[i].size()];
		for(tmp=0;tmp<nv[i].size();tmp++){
			comp1->communities[i][tmp]=nv[i][tmp];
		}
	}

	community_statistics(comp1,comp);

	for (i=0;i<comp1->nvertices;i++){
		delete[] comp1->communities[i];
	}
	delete[] comp1->numbelong;

	i=comp1->nvertices;

	delete comp1;


	return i;

}

int show_result_community(RESULT_COMMUNITY * result, FILE *stream)
{
	int i,j;

	fprintf(stream,"total communities: %d \n",result->ncommunities);

	for(i=0;i<result->ncommunities;i++){
		fprintf(stream,"community %d: ",i+1);
		for(j=0;j<result->nvertices[i];j++){
			fprintf(stream,"%d ",result->vertices[i][j]);
		}
		fprintf(stream,"\n");
	}

	return 1;
}


void FindFiles(vector<string>& sFileNames,string sPath)
{
	WIN32_FIND_DATAA wfd;

	sPath += "\\*.*";

	HANDLE hFile = FindFirstFile(sPath.c_str(),&wfd);

	if(INVALID_HANDLE_VALUE == hFile){
		cout<<"error1"<<endl;
		system("pause");
		return;
	}

	do{

		sFileNames.push_back(wfd.cFileName);

	}while(FindNextFile(hFile,&wfd));

 

}

double findmax(vector<double>& vl)
{
	if (vl.size() == 0){
		return 0;
	}
	double max = vl[0];
	for (int i = 1; i < vl.size(); ++i){
		if (vl[i] > max){
			max = vl[i];
		}
	}
	return max;
}

double findmin(vector<double>& vl)
{
	if (vl.size() == 0){
		return 0;
	}
	double min = vl[0];
	for (int i = 1; i < vl.size(); ++i){
		if (vl[i] < min){
			min = vl[i];
		}
	}
	return min;
}

double computeavg(vector<double>& vl)
{
	if (vl.size() == 0){
		return 0;
	}
	double sum = 0;
	for (int i = 0; i < vl.size(); ++i){
		sum += vl[i];
	}
	return sum / vl.size();
}

double computesd(vector<double>& vl)
{
	if (vl.size() == 0){
		return 0;
	}
	double sum = 0;
	double avg = computeavg(vl);
	for (int i = 0; i < vl.size(); ++i){
		sum += pow(avg - vl[i], 2);
	}
	return pow(sum / vl.size(), 0.5);
}

double comparevertex()
{
	int c1, c2, c3;
	map<int, vector<int> > tran;
	map<int, vector<int> >::iterator mit;
	vector<int> tmp;

	for (int i = 0; i < corvertex.size(); ++i){
		tmp.clear();
		tmp.push_back(0);
		tran.insert(pair<int, vector<int> >(i + 1, tmp));
	}

	for (int i = 0; i < comvertex.size(); ++i){
		for (int j = 0; j < comvertex[i].size(); ++j){
			mit = tran.find(comvertex[i][j]);
			if (mit == tran.end()){
				cout << "error comparevertex!" << endl;
				system("pause");
				exit(0);
			}
			if (mit->second[0] == 0){
				mit->second[0] = i + 1;
			}
			else{
				mit->second.push_back(i + 1);
			}
		}
	}

	c1 = c2 = c3 = 0;
	
	for (int i = 0; i < corvertex.size(); ++i){
		if (tran[i + 1].size() > 1){
			++c1;
		}
		if (corvertex[i].size() > 1){
			++c2;
		}
		if (corvertex[i].size() > 1 && tran[i + 1].size() > 1){
			++c3;
		}
	}
	double tmp1 = c1 ? (double)c3 / c1 : 0;
	double tmp2 = c2 ? (double)c3 / c2 : 0;

	return (tmp1 + tmp2) ? 2 * tmp1*tmp2 / (tmp1 + tmp2) : 0;

}


double findmaxnmi(vector<pair<string, double> > &fnmi, string &fstr)
{
	if (fnmi.size() == 0){
		return 0;
	}
	double maxv = fnmi[0].second;
	fstr = fnmi[0].first;
	for (int i = 1; i < fnmi.size(); ++i){
		if (maxv < fnmi[i].second){
			maxv = fnmi[i].second;
			fstr = fnmi[i].first;
		}
	}
	return maxv;
}


int main(int argc, char *argv[])
{
	int i, j, tmp;
	double mi;
	string fstr;

	RESULT_COMMUNITY *result,*comp;
	result=new RESULT_COMMUNITY;
	comp=new RESULT_COMMUNITY;

	vector<string> inputfiles, arg1, arg2;
	vector<double> maxs, gmax, gmin, gavg, gsd, gfs, fsl;
	vector<pair<string, double> > fnmi;
	

	if (argc % 2 == 1 && argc >= 3){
		for (i = 2; i < argc; i += 2){
			arg1.push_back(argv[i - 1]);
			arg2.push_back(argv[i]);
		}
	}
	else if (argc == 2){
		arg1.push_back(argv[1]);
		arg2.push_back("input");
	}
	else if (argc == 1){
		arg1.push_back("community.dat");
		arg2.push_back("input");
	}
	else{
		cout << "arguments error!" << endl;
		system("pause");
		return 0;
	}

	for (i = 0; i < arg1.size(); ++i){

		inputfiles.clear();
		fnmi.clear();
		fsl.clear();

		FindFiles(inputfiles, ".\\" + arg2[i]);
		inputfiles.erase(inputfiles.begin(), inputfiles.begin() + 2);

		cout << "Find " << inputfiles.size() << " files in floder " + arg2[i] + " ." << endl;
		if (inputfiles.size() == 0){
			continue;
		}
		tmp = readCommunitiesComp(arg1[i], result);
		//show_result_community(result,stdout);

		for (j = 0; j < inputfiles.size(); j++){
			//cout << "*";
			fnmi.push_back(pair<string, double>(".\\" + arg2[i] + "\\" + inputfiles[j], 0));
			if (readCommunities(fnmi.back().first, comp)){
				//show_result_community(comp,stdout);
				mi = calculate_nmi(comp, result, tmp);
				cout << "( "<<mi<<" , "<<comparevertex()<<" )    ";
				fnmi.back().second = mi;
				//cout << inputfiles[i] << "\t" << mi << endl;
			}
			if (fnmi.size() == 10){
				maxs.push_back(findmaxnmi(fnmi,fstr));
				readCommunities(fstr, comp);
				cout << fstr << endl;
				fsl.push_back(comparevertex());
				fnmi.clear();

			}
		}
		cout << endl;
		if (maxs.empty()){
			maxs.push_back(findmaxnmi(fnmi, fstr));
			readCommunities(fstr, comp);
			cout << fstr << endl;
			fsl.push_back(comparevertex());
		}

		gmax.push_back(findmax(maxs));
		gmin.push_back(findmin(maxs));
		gavg.push_back(computeavg(maxs));
		gsd.push_back(computesd(maxs));
		gfs.push_back(computeavg(fsl));
		cout << "max = " << gmax.back() << endl;
		cout << "min = " << gmin.back() << endl;
		cout << "avg = " << gavg.back() << endl;
		cout << "S.D = " << gsd.back() << endl;
		cout << "F-score = " << gfs.back() << endl;

		maxs.clear();
	}
	cout << "---------------------------------!" << endl;
	cout << "Global max = " << computeavg(gmax) << endl;
	cout << "Global min = " << computeavg(gmin) << endl;
	cout << "Global avg = " << computeavg(gavg) << endl;
	cout << "Global S.D = " << computeavg(gsd) << endl;
	cout << "Global F-score = " << computeavg(gfs) << endl;

	system("pause");

	return 1;
}