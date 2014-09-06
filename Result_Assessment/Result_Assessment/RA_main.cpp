#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<vector>
#include<Windows.h>
#include"NMI.h"

using namespace std;

int readCommunities(string fileName, RESULT_COMMUNITY *comp){
	

	string oneLine, whiteSpace=" \t\n";
	fstream fp;
	stringstream linestream;
	int i,tmp;

	vector<int> v;
	vector<vector<int>> nv;

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
			while(linestream>>tmp){
				v.push_back(tmp);
			}
			nv.push_back(v);
		} //while

		fp.close();
	}
	else{
		cout<<"open failed"<<endl;
		return 0;
	}

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

	sPath += "\\*.icpm";

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
	double sum = 0;
	for (int i = 0; i < vl.size(); ++i){
		sum += vl[i];
	}
	return sum / vl.size();
}

double computesd(vector<double>& vl)
{
	double sum = 0;
	double avg = computeavg(vl);
	for (int i = 0; i < vl.size(); ++i){
		sum += pow(avg - vl[i], 2);
	}
	return pow(sum / vl.size(), 0.5);
}

int main(int argc, char *argv[])
{
	int i,tmp;
	double mi;

	RESULT_COMMUNITY *result,*comp;
	result=new RESULT_COMMUNITY;
	comp=new RESULT_COMMUNITY;

	vector<string> inputfiles;
	vector<double> mil, maxs;

	if (argc != 1 && argc != 2 && argc != 3){
		cout << "arguments error!" << endl;
		system("pause");
		return 0;
	}
	string filename_comp = "community.dat";
	string indir = "input";
	if (argc == 2){
		filename_comp = argv[1];
	}
	if (argc == 3){
		filename_comp = argv[1];
		indir = argv[2];
	}

	FindFiles(inputfiles,".\\"+indir);

	cout<<"Find "<<inputfiles.size()<<" files."<<endl;

	tmp=readCommunitiesComp(filename_comp,result);
	//show_result_community(result,stdout);

	for(i=0;i<inputfiles.size();i++){
		if(readCommunities(".\\"+indir+"\\"+inputfiles[i],comp)){
			//show_result_community(comp,stdout);
			mi = calculate_nmi(comp, result, tmp);
			mil.push_back(mi);
			cout<<inputfiles[i]<<"\t"<<mi<<endl;
		}
		if (mil.size() == 10){
			maxs.push_back(findmax(mil));
			mil.clear();
		}
	}

	if (!maxs.empty()){
		cout << "max =" << findmax(maxs) << endl;
		cout << "min =" << findmin(maxs) << endl;
		cout << "avg =" << computeavg(maxs) << endl;
		cout << "D =" << computesd(maxs) << endl;
	}

	system("pause");

	return 1;
}