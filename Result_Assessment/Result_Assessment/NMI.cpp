#include<iostream>
#include<math.h>
#include"NMI.h"

int element_compariosn(int total, int num1, int *set1, int num2, int *set2, float p[2][2])
{
	int i=0,j=0,count=0;

	while(i<num1&&j<num2){

		if(set1[i]<set2[j])
			i++;
		else if(set1[i]>set2[j])
			j++;
		else{
			i++;
			j++;
			count++;
		}

	}

	p[1][1]=(float)count/total;
	p[1][0]=(float)(num1-count)/total;
	p[0][1]=(float)(num2-count)/total;
	p[0][0]=1-p[1][1]-p[1][0]-p[0][1];

	return 1;
}

float h(float p)
{
	return p==0?0:-p*log10(p);
}

float result_comparison(RESULT_COMMUNITY *comp, RESULT_COMMUNITY *result, int total)
{
	int i,j,k;
	float p[2][2];
	float *list;
	float hxky,hxy,hx,t1,t2,sum=0;

	list=(float *)malloc(result->ncommunities*sizeof(float));

	for(i=0;i<result->ncommunities;i++){
		list[i]=-1;
	}

	for(i=0;i<comp->ncommunities;i++){
		k=0;
		hx=h((float)comp->nvertices[i]/total)+h(1-(float)comp->nvertices[i]/total);
		for(j=0;j<result->ncommunities;j++){
			//计算p
			element_compariosn(total,comp->nvertices[i],comp->vertices[i],result->nvertices[j],result->vertices[j],p);
			//计算H(Xk|Yl)
			t1=h(p[1][1])+h(p[0][0]);
			t2=h(p[0][1])+h(p[1][0]);
			if(t1>t2){
				list[j]=h(p[1][1])+h(p[0][0])+h(p[0][1])+h(p[1][0])-h(p[0][1]+p[1][1])-h(p[0][0]+p[1][0]);
				k++;
			}

		}
		//计算H(Xk|Y)
		if(k==0){
			hxky=hx;
		}else{
			hxky=1073741824;
			for(k=0;k<result->ncommunities;k++){
				if(list[k]==-1)continue;
				if(hxky>list[k])hxky=list[k];
			}
		}

		sum+=hxky/hx;

	}
	//计算H(X|Y)
	sum/=comp->ncommunities;
	
	free(list);

	return sum;
}

float calculate_nmi(RESULT_COMMUNITY *comp, RESULT_COMMUNITY *result, int total)
{
	return 1-(result_comparison(comp,result,total)+result_comparison(result,comp,total))/2;
}