#include"nmi.h"
using namespace std;

double h(double p)
{
	return p == 0 ? 0 : -p*log2(p);
}


double shannon_entropy(vector<pair<int, double>> &set1)
{
	double sum=0;

	for (int i = 0; i < set1.size(); i++){
		sum += h(set1[i].second);
	}

	return sum;
}


double joint_entropy(vector<pair<int, double>> &set1, vector<pair<int, double>> &set2)
{
	int i, j;
	vector<pair<int, double>> exset1, exset2;
	i = j = 0;
	while (i != set1.size() && j != set2.size()){
		if (set1[i].first < set2[j].first){
			exset1.push_back(set1[i]);
			exset2.push_back(pair<int, double>(set1[i].first, 0));
			++i;
		}
		else if (set1[i].first > set2[j].first){
			exset2.push_back(set2[j]);
			exset1.push_back(pair<int, double>(set2[j].first, 0));
			++j;
		}
		else{
			exset1.push_back(set1[i]);
			exset2.push_back(set2[j]);
			++i;
			++j;
		}
	}

	if (i == set1.size()){
		while (j != set2.size()){
			exset2.push_back(set2[j]);
			exset1.push_back(pair<int, double>(set2[j].first, 0));
			++j;
		}
	}
	else{
		while (i != set1.size()){
			exset1.push_back(set1[i]);
			exset2.push_back(pair<int, double>(set1[i].first, 0));
			++i;
		}
	}

	double sum = 0;

	for (i = 0; i < exset1.size(); ++i){
		for (j = 0; j < exset2.size(); ++j){
			sum += h(exset1[i].second*exset2[j].second);
		}
	}

	return sum;
}


double calculate_nmi(vector<pair<int, double>> &set1, vector<pair<int, double>> &set2)
{
	double hx, hy, hxy, ixy;

	sortVectorInt_Double_first(set1);
	sortVectorInt_Double_first(set2);

	hx = shannon_entropy(set1);
	hy = shannon_entropy(set2);
	hxy = joint_entropy(set1, set2);
	ixy = hx + hy - hxy;

	return (2 * ixy) / (hx + hy);
}