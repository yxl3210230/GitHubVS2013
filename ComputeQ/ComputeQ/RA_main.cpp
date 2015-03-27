#include<iostream>
//#include<sstream>
//#include<fstream>
//#include<string>
//#include<vector>
//#include<map>
#include<Windows.h>
//#include<algorithm>
#include"DisjointM.h"

using namespace std;


void FindFiles(vector<string>& sFileNames,string sPath)
{
	WIN32_FIND_DATAA wfd;
	string tmp = sPath;
	sPath += "\\*.*";

	HANDLE hFile = FindFirstFile(sPath.c_str(),&wfd);

	if(INVALID_HANDLE_VALUE == hFile){
		cout<<"error1"<<endl;
		system("pause");
		return;
	}

	do{

		sFileNames.push_back(tmp+"\\"+wfd.cFileName);

	}while(FindNextFile(hFile,&wfd));

 

}



int main(int argc, char *argv[])
{
	int i, j, tmp;
	double Q;
	string cpmdir, netdir;

	vector<string> cpmfiles, netfiles;


	if (argc == 3){
		netdir = argv[1];
		cpmdir = argv[2];
	}
	else{
		cout << "arguments error!" << endl;
		system("pause");
		return 0;
	}

	FindFiles(cpmfiles, ".\\" + cpmdir);
	FindFiles(netfiles, ".\\" + netdir);
	cpmfiles.erase(cpmfiles.begin(), cpmfiles.begin() + 2);
	netfiles.erase(netfiles.begin(), netfiles.begin() + 2);

	for (i = 0; i < cpmfiles.size() && i < netfiles.size(); ++i){
		DisjointM mq;
		Q = mq.calQ_unw(cpmfiles[i], netfiles[i], NULL, false, true, 0);
		cout << cpmfiles[i] << "\t" << Q << endl;
		if (i % 20 == 0 && i != 0){
			system("pause");
		}
	}
	
	system("pause");

	return 1;
}