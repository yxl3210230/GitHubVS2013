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

}


double calculate_nmi(vector<pair<int, double>> &set1, vector<pair<int, double>> &set2)
{

}