#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include <math.h>
#include "readgml.h"


//#define MAX_T 20
//#define CONST_P 30
//
//typedef struct{
//	int nvertices;
//	int **memory;
//}MEMORY_SLPA;
//
//
//typedef struct{
//	int length;
//	int *queue;
//}PROCESSING_QUEUE;
//
//
//typedef struct{
//	int degree;
//	int *neighbor;
//}NEIGHBOR_VERTICES;
//
//
//typedef struct{
//	int numlabel;
//	int *index;
//	float *probability;
//}LABEL_PROBABILITIES;
//
//
//typedef struct{
//	int num;
//	int *list;
//}SEND_LIST;
//
//
//typedef struct{
//	int nvertices;
//	int *numbelong;
//	int **communities;
//}RESULT_VERTEX;
//
//
//typedef struct{
//	int ncommunities;
//	int *nvertices;
//	int **vertices;
//}RESULT_COMMUNITY;
//

int calculate_coefficient(NETWORK *net,COEFFICIENT1 *co1, COEFFICIENT2 *co2)
{
	int i,j,k,l,m,count2;

	int *list,*list2,*flag,*count1;

	//flag=(int *)calloc(net->nvertices,sizeof(int));
	count1=(int *)calloc(net->nvertices,sizeof(int));

	co1->nvertices=net->nvertices;
	co1->coefficients=(float *)malloc(co1->nvertices*sizeof(float));

	co2->nvertices=net->nvertices;
	co2->degrees=(int *)malloc(co2->nvertices*sizeof(int));
	co2->coefficients=(float **)malloc(co2->nvertices*sizeof(float*));

	for(i=0;i<net->nvertices;i++){
		co2->degrees[i]=net->vertex[i].degree;
		list=(int *)malloc(net->vertex[i].degree*sizeof(int));
		//co1->coefficients[i]=(float *)malloc(co1->degrees[i]*sizeof(float));
		co2->coefficients[i]=(float *)malloc(co2->degrees[i]*sizeof(float));
		for(j=0;j<net->vertex[i].degree;j++){
			list[j]=net->vertex[i].edge[j].target;
		}
		//for(j=0;j<net->vertex[i].degree;j++){
		//	if(flag[list[j]])continue;
		//	//count=0;
		//	list2=(int *)malloc(net->vertex[list[j]].degree*sizeof(int));
		//	for(k=0;k<net->vertex[list[j]].degree;k++){
		//		list2[k]=net->vertex[list[j]].edge[k].target;
		//	}
		//	count1=0;
		//	count2=0;
		//	for(k=0;k<net->vertex[list[j]].degree;k++){
		//		for(m=k+1;m<net->vertex[list[j]].degree;m++){
		//			for(l=0;l<net->vertex[list2[m]].degree;l++){
		//				if(net->vertex[list2[m]].edge[l].target==list2[k]){
		//					count1++;
		//					if (list2[k]==i||list2[m]==i)
		//					{
		//						count2++;
		//					}
		//				}
		//			}
		//		}
		//	}
		//	co1->coefficients[list[j]]=count1?(float)count2/count1:0;
		//	co2->coefficients[list[j]]=net->vertex[list[j]].degree?(float)(count1*2)/(net->vertex[list[j]].degree*(net->vertex[list[j]].degree-1)):0;
		//	flag[list[j]]=1;
		//	free(list2);
		//}
		
		for(j=0;j<net->vertex[i].degree;j++){
			//if(flag[list[j]])continue;

			list2=(int *)malloc(net->vertex[list[j]].degree*sizeof(int));

			for(k=0;k<net->vertex[list[j]].degree;k++){
				list2[k]=net->vertex[list[j]].edge[k].target;
			}

			k=0;
			l=0;
			count2=0;

			while(k<net->vertex[i].degree&&l<net->vertex[list[j]].degree){
				//if(list2[l]<list[j]){
				//	l++;
				//	continue;
				//}
				if(list[k]<list2[l]){
					k++;
					continue;
				}
				if(list[k]>list2[l]){
					l++;
					continue;
				}
				if(list[k]==list2[l]){
					if (list2[l]>list[j])
					{
						count1[i]++;
					}
					count2++;
					k++;
					l++;
					continue;
				}
			}

			co2->coefficients[i][j]=count2;
			free(list2);
		}
		co1->coefficients[i]=(net->vertex[i].degree!=1)?(float)(count1[i]*2)/(net->vertex[i].degree*(net->vertex[i].degree-1)):0;
		free(list);

	}

	for(i=0;i<net->nvertices;i++){
		for(j=0;j<net->vertex[i].degree;j++){
			//co2->coefficients[i][j]=count1[net->vertex[i].edge[j].target]?co2->coefficients[i][j]/count1[net->vertex[i].edge[j].target]:0;
			co2->coefficients[i][j]=count1[net->vertex[i].edge[j].target]?(co2->coefficients[i][j]*2)/(net->vertex[net->vertex[i].edge[j].target].degree*(net->vertex[net->vertex[i].edge[j].target].degree-1)):0;
		}
	}

	free(count1);

	return 1;
}

int comp_degree1(VERTEX *v1p, VERTEX *v2p)
{

	return v1p->degree-v2p->degree;
}

int comp_degree2(VERTEX *v1p, VERTEX *v2p)
{

	return v2p->degree-v1p->degree;
}


int sort_network(NETWORK *net, int i)
{
	if(i>0){

		qsort(net->vertex,net->nvertices,sizeof(VERTEX),(int (__cdecl *)(const void *, const void *))comp_degree1);

	}else{

		qsort(net->vertex,net->nvertices,sizeof(VERTEX),(int (__cdecl *)(const void *, const void *))comp_degree2);

	}

	return 1;
}

int show_network(NETWORK *net, FILE *stream)
{
	int i,j;
	for(i=0;i<net->nvertices;i++){
		fprintf(stream,"%d %s ",net->vertex[i].id,net->vertex[i].label);
		for(j=0;j<net->vertex[i].degree;j++)
			fprintf(stream,"%d ",net->vertex[i].edge[j].target);
		fprintf(stream,"\n");
	}
	return 0;
}



//int initial_memory(MEMORY_SLPA *mem,int nvertices)
//{
//	int i;
//	mem->nvertices=nvertices;
//	mem->memory=(int **)malloc(nvertices*sizeof(int*));
//	for(i=0;i<nvertices;i++){
//		mem->memory[i]=(int *)malloc((MAX_T+1)*sizeof(int));
//		mem->memory[i][0]=i+1;
//		memset(mem->memory[i]+1,0,MAX_T*sizeof(int));
//	}
//	return 0;
//}

int initial_memory1(MEMORY_VERTEX *mem,int nvertices)
{
	int i;
	//MEMORY_LABEL *p;
	mem->nvertices=nvertices;
	mem->nlabels=(int *)malloc(nvertices*sizeof(int));
	mem->head=(MEMORY_LABEL *)malloc(nvertices*sizeof(MEMORY_LABEL));
	for(i=0;i<nvertices;i++){
		mem->nlabels[i]=1;
		mem->head[i].id=0;
		mem->head[i].num=1;
		mem->head[i].next=(MEMORY_LABEL *)malloc(sizeof(MEMORY_LABEL));
		mem->head[i].next->id=i+1;
		mem->head[i].next->num=1;
		mem->head[i].next->next=NULL;
	}
	return 0;
}

//int show_memory(MEMORY_SLPA *mem, FILE *stream)
//{
//	int i,j;
//	for(i=0;i<mem->nvertices;i++){
//		fprintf(stream,"vertex %d: ",i);
//		for(j=0;j<(MAX_T+1);j++)
//			fprintf(stream,"%d ",mem->memory[i][j]);
//		fprintf(stream,"\n");
//	}
//	return 0;
//}

int show_memory1(MEMORY_VERTEX *mem, FILE *stream)
{
	int i,j;
	MEMORY_LABEL *p;
	for(i=0;i<mem->nvertices;i++){
		fprintf(stream,"vertex %d: ",i);
		p=mem->head+i;
		for(j=0;j<mem->nlabels[i];j++){
			p=p->next;
			fprintf(stream,"label%d %d ",p->id,p->num);
		}
		fprintf(stream,"\n");
	}
	return 0;
}

int destroy_memory(MEMORY_VERTEX *mem)
{
	int i;
	MEMORY_LABEL *p,*q;
	for(i=0;i<mem->nvertices;i++){
		p=mem->head+i;
		p=p->next;
		while(p!=NULL){
			q=p->next;
			free(p);
			p=q;
		}
	}
	free(mem->head);
	free(mem->nlabels);
	//free(mem);

	return 1;
}


int initial_queue(PROCESSING_QUEUE *que, int length)
{
	int i;
	que->length=length;
	que->queue=(int *)malloc(length*sizeof(int));
	for(i=0;i<length;i++)
		que->queue[i]=i;
	return 0;
}


int random_sort(PROCESSING_QUEUE *que)
{
	int i,j,tmp;
	//time_t t;
	srand((unsigned)time(NULL));
	for(i=0;i<que->length-1;i++){
		j=rand()%(que->length-i-1);
		tmp=que->queue[j+i+1];
		que->queue[j+i+1]=que->queue[i];
		que->queue[i]=tmp;
	}
	return 0;
}


int show_queue(PROCESSING_QUEUE *que)
{
	int i;
	for(i=0;i<que->length;i++)
		printf("%d ",que->queue[i]);
	printf("\n");
	return 0;
}


int find_neighbor(NETWORK *net, NEIGHBOR_VERTICES *nei,int index)
{
	int i;
	nei->degree=net->vertex[index].degree;
	nei->neighbor=(int *)malloc(nei->degree*sizeof(int));
	for(i=0;i<nei->degree;i++)
		nei->neighbor[i]=net->vertex[index].edge[i].target;

	return 0;
}


int show_neighbor(NEIGHBOR_VERTICES *nei)
{
	int i;
	printf("Neighbor: ");
	for(i=0;i<nei->degree;i++)
		printf("%d ",nei->neighbor[i]);
	printf("\n");
	return 0;
}

int comp_label(int *label1, int *label2)
{
	return *label1-*label2;
}

int memory_sort(MEMORY_SLPA *mem, int *sortedlabel, int vertexid, int totallabel)
{
	int i;
	for(i=0;i<totallabel;i++)
		sortedlabel[i]=mem->memory[vertexid][i];
	//printf("before sort: ");
	//for(i=0;i<totallabel;i++)
	//	printf("%d ",sortedlabel[i]);
	//printf("\n");
	qsort(sortedlabel,totallabel,sizeof(int),(int (__cdecl *)(const void *, const void *))comp_label);
	return 0;
}


int count_label(int *sortedlabel, int *countlabel, int totallabel)
{
	int i,tmp,tmpindex,numlabel;
	tmp=sortedlabel[0];
	tmpindex=0;
	numlabel=1;
	for(i=1;i<totallabel;i++){
		if(sortedlabel[i]==tmp)countlabel[tmpindex]++;
		else{
			tmp=sortedlabel[i];
			tmpindex=i;
			numlabel++;
		}
	}
	return numlabel;
}

int count_probabilities(LABEL_PROBABILITIES *pro, MEMORY_SLPA *mem, int vertexid, int totallabel)
{
	int numlabel, i, j;
	int *sortedlabel, *countlabel;

	sortedlabel=(int *)malloc(totallabel*sizeof(int));
	countlabel=(int *)malloc(totallabel*sizeof(int));
	for(i=0;i<totallabel;i++)
		countlabel[i]=1;

	memory_sort(mem,sortedlabel,vertexid,totallabel);

	//printf("sort resualt: ");
	//for(i=0;i<totallabel;i++)
	//	printf("%d ",sortedlabel[i]);
	//printf("\n");

	numlabel=count_label(sortedlabel,countlabel,totallabel);

	//printf("number of labels: %d\n", numlabel);

	//printf("count resualt: ");
	//for(i=0;i<totallabel;i++)
	//	printf("%d ",countlabel[i]);
	//printf("\n");

	//if(pro->index!=NULL)
	//	free(pro->index);
	//if(pro->probability!=NULL)
	//	free(pro->probability);

	pro->numlabel=numlabel;
	pro->index=(int *)malloc(numlabel*sizeof(int));
	pro->probability=(float *)malloc(numlabel*sizeof(float));


	i=0;
	j=0;

	while(i<totallabel){
		pro->index[j]=sortedlabel[i];
		pro->probability[j]=(float)countlabel[i]/totallabel;
		i+=countlabel[i];
		j++;
	}

	free(sortedlabel);
	free(countlabel);

	return 0;
}

int count_probabilities1(LABEL_PROBABILITIES *pro, MEMORY_VERTEX *mem, int vertexid)
{
	int i;
	MEMORY_LABEL *p;

	pro->numlabel=mem->nlabels[vertexid];
	pro->index=(int *)malloc(pro->numlabel*sizeof(int));
	pro->probability=(float *)malloc(pro->numlabel*sizeof(float));

	p=mem->head+vertexid;

	for(i=0;i<pro->numlabel;i++){
		p=p->next;
		pro->index[i]=p->id;
		pro->probability[i]=(float)p->num/mem->head[vertexid].num;
	}

	return 0;
}

int send_label(LABEL_PROBABILITIES *pro, int *index)
{
	int i;
	float randnum, tmp;

	randnum=(float)rand()/(RAND_MAX+1);
	tmp=pro->probability[0];
	i=0;

	while(randnum>tmp)
		tmp+=pro->probability[++i];

	*index=pro->index[i];

	return i;
}

int show_sendlist(SEND_LIST *sendlist,FILE *stream)
{
	int i;
	fprintf(stream,"Send List: ");
	for(i=0;i<sendlist->num;i++)
		fprintf(stream,"%d ",sendlist->list[i]);
	fprintf(stream,"\n");
	return 0;
}

int comp_label1(SL_ELEMENT *label1, SL_ELEMENT *label2)
{
	return label1->index-label2->index;
}

int receive_label_old(SEND_LIST *sendlist)
{
	int i,j,maxi,tmp,num;
	int *sortedlabel, *countlabel, *maxlabel;
	sortedlabel=(int *)malloc(sendlist->num*sizeof(int));
	countlabel=(int *)malloc(sendlist->num*sizeof(int));
	for(i=0;i<sendlist->num;i++)
		countlabel[i]=1;

	for(i=0;i<sendlist->num;i++)
		sortedlabel[i]=sendlist->list[i].index;

	qsort(sortedlabel,sendlist->num,sizeof(int),(int (__cdecl *)(const void *, const void *))comp_label);
	
	//计算标签出现次数
	count_label(sortedlabel,countlabel,sendlist->num);

	//查找出现次数最多的标签
	tmp=countlabel[0];
	maxi=0;
	for(i=1;i<sendlist->num;i++){
		if(tmp<countlabel[i]){
			tmp=countlabel[i];
			maxi=i;
		}
	}

	//查找其他出现次数最多的标签
	num=0;
	for(i=0;i<sendlist->num;i++){
		if(countlabel[i]==tmp){
			num++;
		}
	}

	if(num>1){
		j=0;
		maxlabel=(int *)malloc(num*sizeof(int));

		for(i=0;i<sendlist->num;i++){
			if(countlabel[i]==tmp){
				maxlabel[j++]=sortedlabel[i];
			}
		}

		//从这些标签中随机选择
		maxi=rand()%num;
		maxi=maxlabel[maxi];

		free(maxlabel);
	}else{
		maxi=sortedlabel[maxi];
	}
	free(countlabel);
	free(sortedlabel);

	return maxi;
}

int receive_label(SEND_LIST *sendlist)
{
	int i,j,maxi,tmp,num;
	int *sortedlabel, *countlabel, *maxlabel;
	float tmpf,maxf;
	SL_ELEMENT *list,*list2;

	sortedlabel=(int *)malloc(sendlist->num*sizeof(int));
	countlabel=(int *)malloc(sendlist->num*sizeof(int));
	for(i=0;i<sendlist->num;i++)
		countlabel[i]=1;

	qsort(sendlist->list,sendlist->num,sizeof(SL_ELEMENT),(int (__cdecl *)(const void *, const void *))comp_label1);

	for(i=0;i<sendlist->num;i++)
		sortedlabel[i]=sendlist->list[i].index;

	
	//计算标签出现次数
	num=count_label(sortedlabel,countlabel,sendlist->num);
	list=(SL_ELEMENT *)calloc(num,sizeof(SL_ELEMENT));

	tmp=countlabel[0];
	maxi=tmp;
	list[0].index=sendlist->list[0].index;
	maxf=0;

	for(i=0,j=0;i<sendlist->num;i++){
		list[j].weight+=sendlist->list[i].weight;
		list[j].weight2+=sendlist->list[i].weight2;
		tmp--;
		if(tmp==0){
			tmp=countlabel[i+1];
			if(maxf<list[j].weight){
				maxf=list[j].weight;
			}
			list[j].weight2/=maxi;
			maxi=tmp;
			j++;
			if(j==num)break;
			list[j].index=sendlist->list[i+1].index;
		}
	}
	
	for(i=0,tmp=0;i<num;i++){
		if(maxf==list[i].weight){
			tmp++;
			maxi=list[i].index;
		}
	}

	if(tmp==1){
		free(countlabel);
		free(sortedlabel);
		free(list);
		return maxi;
	}

	//list2=(SL_ELEMENT *)malloc(tmp*sizeof(SL_ELEMENT));

	//tmpf=1;
	//for(i=0,j=0;i<num;i++){
	//	if(maxf==list[i].weight){
	//		list2[j].index=list[i].index;
	//		list2[j].weight=list[i].weight;
	//		list2[j].weight2=list[i].weight2;
	//		if(tmpf>list[i].weight2){
	//			tmpf=list[i].weight2;
	//		}
	//		j++;
	//	}
	//}

	//for(i=0,num=0;i<tmp;i++){
	//	if(tmpf==list2[i].weight2){
	//		maxi=list2[i].index;
	//		num++;
	//	}
	//}

	//if(num==1){
	//	free(countlabel);
	//	free(sortedlabel);
	//	free(list);
	//	free(list2);
	//	return maxi;
	//}

	//maxlabel=(int *)malloc(num*sizeof(int));

	//for(i=0,j=0;i<tmp;i++){
	//	if(tmpf==list2[i].weight2){
	//		maxlabel[j++]=list2[i].index;
	//	}
	//}
	maxlabel=(int *)malloc(tmp*sizeof(int));
	for(i=0,j=0;i<num;i++){
		if(maxf==list[i].weight){
			maxlabel[j++]=list[i].index;
		}
	}
	maxi=rand()%tmp;
	//maxi=rand()%num;
	maxi=maxlabel[maxi];

	free(list);
//	free(list2);
	free(maxlabel);

	////查找出现次数最多的标签
	//tmp=countlabel[0];
	//maxi=0;
	//for(i=1;i<sendlist->num;i++){
	//	if(tmp<countlabel[i]){
	//		tmp=countlabel[i];
	//		maxi=i;
	//	}
	//}

	////查找其他出现次数最多的标签
	//num=0;
	//for(i=0;i<sendlist->num;i++){
	//	if(countlabel[i]==tmp){
	//		num++;
	//	}
	//}

	//if(num>1){
	//	j=0;
	//	maxlabel=(int *)malloc(num*sizeof(int));

	//	for(i=0;i<sendlist->num;i++){
	//		if(countlabel[i]==tmp){
	//			maxlabel[j++]=sortedlabel[i];
	//		}
	//	}

	//	//从这些标签中随机选择
	//	maxi=rand()%num;
	//	maxi=maxlabel[maxi];

	//	free(maxlabel);
	//}else{
	//	maxi=sortedlabel[maxi];
	//}
	free(countlabel);
	free(sortedlabel);

	return maxi;
}

int add_label(MEMORY_VERTEX *mem, int vertexid, int labelid)
{
	int i;

	MEMORY_LABEL *p,*node;

	p=mem->head+vertexid;

	while(p->next!=NULL){
		if(p->next->id<labelid){
			p=p->next;
			continue;
		}
		if(p->next->id==labelid){
			p->next->num++;
			break;
		}
		if(p->next->id>labelid){
			node=(MEMORY_LABEL *)malloc(sizeof(MEMORY_LABEL));
			node->id=labelid;
			node->num=1;
			node->next=p->next;
			p->next=node;
			mem->nlabels[vertexid]++;
			break;
		}
	}

	if(p->next==NULL){
		node=(MEMORY_LABEL *)malloc(sizeof(MEMORY_LABEL));
		node->id=labelid;
		node->num=1;
		node->next=NULL;
		p->next=node;
		mem->nlabels[vertexid]++;
	}
	mem->head[vertexid].num++;

	return 1;
}

int add_label_syn(MEMORY_VERTEX *mem, int *synlist)
{
	int i;
	MEMORY_LABEL *p,*node;

	for(i=0;i<mem->nvertices;i++){
		add_label(mem,i,synlist[i]);
	}

	return 1;
}


int post_processing(LABEL_PROBABILITIES *pro, RESULT_VERTEX *result, int index, float threshold)
{
	int i,j,num;
	float tmp;

	for(i=0,num=0;i<pro->numlabel;i++){
		if(pro->probability[i]>threshold)
			num++;
	}

	if(num==0){

		tmp=pro->probability[0];
		for(i=1;i<pro->numlabel;i++){
			if(pro->probability[i]>tmp)
				tmp=pro->probability[i];
		}

		for(i=0;i<pro->numlabel;i++){
			if(pro->probability[i]==tmp)
				num++;
		}

		result->numbelong[index]=num;
		result->communities[index]=(int *)malloc(num*sizeof(int));

		for(i=0,j=0;i<pro->numlabel;i++){
			if(pro->probability[i]==tmp)
				result->communities[index][j++]=pro->index[i];
		}

		return 1;

	}

	result->numbelong[index]=num;
	result->communities[index]=(int *)malloc(num*sizeof(int));

	for(i=0,j=0;i<pro->numlabel;i++){
		if(pro->probability[i]>threshold)
			result->communities[index][j++]=pro->index[i];
	}

	return 1;
}

int show_result_vertex(RESULT_VERTEX *result, FILE *stream)
{
	int i,j;

	for(i=0;i<result->nvertices;i++){
		fprintf(stream,"vertex %d belong to: ",i);
		for(j=0;j<result->numbelong[i];j++)
			fprintf(stream,"%d ",result->communities[i][j]);
		fprintf(stream,"\n");
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


int result_processing(RESULT_VERTEX *result1, RESULT_COMMUNITY * result2)
{
	int i,j,k,l,num,count;
	int *flag,*tmp;

	flag=(int *)calloc(result2->ncommunities,sizeof(int));
	
	for(i=0;i<result2->ncommunities;i++){

		if(result1->numbelong[result2->vertices[i][0]]==1)continue;
		
		count=0;

		for(j=0;j<result1->numbelong[result2->vertices[i][0]];j++){

			for(k=0;k<result2->nvertices[i];k++){
				num=0;
				for(l=0;l<result1->numbelong[result2->vertices[i][k]];l++){
					if(result1->communities[result2->vertices[i][k]][l]==result1->communities[result2->vertices[i][0]][j]){
						num=1;
						break;
					}
				}
				if(num==0){
					break;
				}

			}
			if(num==1){
				count++;
			}
			if(count==2){
				break;
			}
		}
		if(count==2){
			flag[i]=1;
		}

	}
	num=0;
	for(i=0;i<result2->ncommunities;i++){
		if(flag[i]==1){
			num++;
		}
	}

	if(num!=0){
		result2->ncommunities-=num;
		for(i=result2->ncommunities+num-1;flag[i]!=0;i--,num--);
		for(j=i-1;num>0;j--){
			if(flag[j]!=1)continue;
			result2->nvertices[j]=result2->nvertices[i];
			tmp=result2->vertices[j];
			result2->vertices[j]=result2->vertices[i];
			result2->vertices[i]=tmp;

			i--;
			num--;
		}
		
	}


	return 1;
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


int count_delete_vertices(MEMORY_VERTEX *mem, SEND_LIST *sendlist, DELETE_LIST *dellist, int index)
{
	int i,j,flag,count;
	MEMORY_LABEL *p,*node,*head,*current;

	count=0;
	head=(MEMORY_LABEL *)malloc(sizeof(MEMORY_LABEL));
	current=head;
	for(i=0;i<mem->nlabels[index];i++){
		p=mem->head+index;
		p=p->next;
		flag=0;
		for(j=0;j<sendlist->num;j++){
			if(p->id==sendlist->list[j].index){
				flag=1;
				break;
			}
		}
		if(flag==0){
			count++;
			node=(MEMORY_LABEL *)malloc(sizeof(MEMORY_LABEL));
			node->id=p->id;
			current->next=node;
			current=current->next;
		}
	}

	if(count==0){
		dellist->nlabels[index]=0;
	}
	else{
		dellist->nlabels[index]=count;
		dellist->label[index]=(int *)malloc(count*sizeof(int));
		current=head->next;
		for(i=0;i<count;i++){
			dellist->label[index][i]=current->id;
			current=current->next;
		}
		current=head->next;
		for(i=0;i<count;i++){
			node=current;
			current=current->next;
			free(node);
		}
	}

	free(head);

	return 1;
}


int delete_vertices(MEMORY_VERTEX *mem, DELETE_LIST *dellist)
{
	int i,j,k;
	MEMORY_LABEL *p,*tmp;

	for(i=0;i<mem->nvertices;i++){
		if(dellist->nlabels[i]==0){
			continue;
		}
		p=mem->head+i;
		for(j=0,k=0;j<mem->nlabels[i]&&k<dellist->nlabels[i];j++){
			if(p->next->id==dellist->label[i][k]){
				p->next->num--;
				(mem->head+i)->num--;
				if(p->next->num==0){
					tmp=p->next;
					p->next=p->next->next;
					free(tmp);
					mem->nlabels[i]--;
				}
				k++;
			}
			p=p->next;
		}
	}

	return 1;
}


int reset_delete_list(DELETE_LIST *dellist)
{
	int i;

	for(i=0;i<dellist->nvertices;i++){
		if(dellist->nlabels[i]==0){
			continue;
		}
		free(dellist->label[i]);
		dellist->nlabels[i]=0;
	}

	return 1;
}


//float function_f(float ai, float aj)
//{
//	return 1/((1+exp(-(2*CONST_P*ai-CONST_P)))*(1+exp(-(2*CONST_P*aj-CONST_P))));
//}
//
//float function_bout(float ai, float aj)
//{
//	float i,sum;
//	return 1;
//}


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
	//交换计算
	free(list);

	return sum;
}

//int pro_compare(LABEL_PROBABILITIES *pro,LABEL_PROBABILITIES *pro1)
//{
//	int i;
//	if(pro->numlabel!=pro1->numlabel)return 0;
//	for(i=0;i<pro->numlabel;i++){
//		if(pro->index[i]!=pro1->index[i])return 0;
//	}
//	for(i=0;i<pro->numlabel;i++){
//		if(pro->probability[i]!=pro1->probability[i])return 0;
//	}
//	return 1;
//}


int main()
{
	NETWORK *net;
	//MEMORY_SLPA *mem;	
	PROCESSING_QUEUE *que;
	NEIGHBOR_VERTICES *nei;
	LABEL_PROBABILITIES *pro,*pro1;
	SEND_LIST *sendlist;
	RESULT_VERTEX *result1, *comp1;
	RESULT_COMMUNITY *result2, *comp2;
	COEFFICIENT1 *co1;
	COEFFICIENT2 *co2;
	DELETE_LIST *dellist;

	int i,j,iterator,c,k,*synlist;
	float nmi;

	//int MAX_T;

	FILE *fp,*fout;
	//if((fp=fopen("dolphins.gml","r"))==NULL)
	if((fp=fopen("network0.dat","r"))==NULL)
	{
		printf("can not open file\n");
		system("pause");
		exit(0);
	}

	if((fout=fopen("output.txt","w"))==NULL)
	{
		printf("can not open file\n");
		system("pause");
		exit(0);
	}

	net=(NETWORK *)malloc(sizeof(NETWORK));
	//mem=(MEMORY_SLPA *)malloc(sizeof(MEMORY_SLPA));
	que=(PROCESSING_QUEUE *)malloc(sizeof(PROCESSING_QUEUE));
	nei=(NEIGHBOR_VERTICES *)malloc(sizeof(NEIGHBOR_VERTICES));
	pro=(LABEL_PROBABILITIES *)malloc(sizeof(LABEL_PROBABILITIES));
	pro1=(LABEL_PROBABILITIES *)malloc(sizeof(LABEL_PROBABILITIES));
	//pro->index=NULL;
	//pro->probability=NULL;

	sendlist=(SEND_LIST *)malloc(sizeof(SEND_LIST));
	result1=(RESULT_VERTEX *)malloc(sizeof(RESULT_VERTEX));
	comp1=(RESULT_VERTEX *)malloc(sizeof(RESULT_VERTEX));
	result2=(RESULT_COMMUNITY *)malloc(sizeof(RESULT_COMMUNITY));
	comp2=(RESULT_COMMUNITY *)malloc(sizeof(RESULT_COMMUNITY));
	co1=(COEFFICIENT1 *)malloc(sizeof(COEFFICIENT1));
	co2=(COEFFICIENT2 *)malloc(sizeof(COEFFICIENT2));
	dellist=(DELETE_LIST *)malloc(sizeof(DELETE_LIST));

	MEMORY_VERTEX *mem1;
	mem1=(MEMORY_VERTEX *)malloc(sizeof(MEMORY_VERTEX));

	printf("Reading network...");
	read_network_bn(net,fp);
	//read_network(net,fp);
	//show_network(net,fout);
	printf("finish!\n");

	fclose(fp);

	if((fp=fopen("community0.dat","r"))==NULL)
	{
		printf("can not open file\n");
		system("pause");
		exit(0);
	}
	
	printf("Reading communities of BN...");
	read_communities(comp1,fp);
	community_statistics(comp1,comp2);
	//show_result_vertex(comp1,fout);
	//show_result_community(comp2,fout);
	printf("finish!\n");

	//sort_network(net,1);
	printf("Calculating coefficients...");
	calculate_coefficient(net,co1,co2);
	printf("finish!\n");

	dellist->nvertices=net->nvertices;
	dellist->nlabels=(int *)calloc(dellist->nvertices,sizeof(int));
	dellist->label=(int **)malloc(dellist->nvertices*sizeof(int *));

	//for(i=0;i<co1->nvertices;i++){
	//	
	//	fprintf(fout,"%d %f\n",i,co1->coefficients[i]);
	//}
	//fprintf(fout,"asdasdasdd\n");
	//for(i=0;i<co2->nvertices;i++){
	//	for (j=0;j<co2->degrees[i];j++)
	//	{
	//		fprintf(fout,"%f ",co2->coefficients[i][j]);
	//	}
	//	fprintf(fout,"\n");
	//}
	//
	//MAX_T=0;
	//while(MAX_T<100){
	//	MAX_T++;
	c=1;
	while(c--){

	synlist=(int *)malloc(net->nvertices*sizeof(int));
	printf("Initialing memory...");
	//initial_memory(mem, net->nvertices);
	//show_memory(mem,fout);
	initial_memory1(mem1, net->nvertices);
	//show_memory1(mem1,fout);
	printf("finish!\n");

	initial_queue(que, net->nvertices);

	iterator=MAX_T;

	printf("Calculating...");
	while(iterator--){

		//处理队列随机排序
		random_sort(que);
		//show_queue(que);

		for(i=0;i<que->length;i++){
			//查找邻节点
			find_neighbor(net,nei,que->queue[i]);
			//printf("%d ",que->queue[i]);
			//show_neighbor(nei);

			sendlist->num=nei->degree;
			sendlist->list=(SL_ELEMENT *)malloc(nei->degree*sizeof(SL_ELEMENT));

			for(j=0;j<nei->degree;j++){
				//计算概率
				//count_probabilities(pro,mem,nei->neighbor[j],MAX_T-iterator);
				count_probabilities1(pro1,mem1,nei->neighbor[j]);
				//printf("pro1: ");
				//for(k=0;k<pro->numlabel;k++){
				//	printf("%d %f ",pro->index[k],pro->probability[k]);
				//}
				//printf("\npro2: ");
				//for(k=0;k<pro1->numlabel;k++){
				//	printf("%d %f ",pro1->index[k],pro1->probability[k]);
				//}
				//printf("\n");
				//if(!pro_compare(pro,pro1)){
				//	printf("%d %d %d %d",iterator,i,j,nei->neighbor[j]);
				//	system("pause");
				//}

				//根据概率发送标签至sendlist
				k=send_label(pro1,&sendlist->list[j].index);
				sendlist->list[j].weight=(1+co2->coefficients[que->queue[i]][j])*pro1->probability[k];
				//sendlist->list[j].weight=1+co2->coefficients[que->queue[i]][j];
				sendlist->list[j].weight2=co1->coefficients[nei->neighbor[j]];

				//send_label(pro1,&sendlist->list[j].index);

				free(pro1->index);
				free(pro1->probability);

			}
			//show_sendlist(sendlist,fout);
			free(nei->neighbor);

			//记录dellist
			//count_delete_vertices(mem1,sendlist,dellist,que->queue[i]);

			//从sendlist中选择标签
			//j=receive_label(sendlist);
			j=receive_label_old(sendlist);

			//异步写入memory
			//mem->memory[que->queue[i]][MAX_T-iterator]=j;
			//add_label(mem1,que->queue[i],j);

			//记录同步列表synlist
			synlist[que->queue[i]]=j;
			free(sendlist->list);
		}
		//同步接收标签
		add_label_syn(mem1,synlist);

		//删除dellist中的标签
		//delete_vertices(mem1,dellist);

		//重置dellist
		//reset_delete_list(dellist);
	}
	free(synlist);
	free(que->queue);

	show_memory1(mem1,fout);
	printf("finish!\n");

	result1->nvertices=mem1->nvertices;
	result1->numbelong=(int *)malloc(result1->nvertices*sizeof(int));
	result1->communities=(int **)malloc(result1->nvertices*sizeof(int*));

	printf("Post processing...");
	for(i=0;i<result1->nvertices;i++){
		//count_probabilities(pro,mem,net->vertex[i].id,MAX_T+1);
		count_probabilities1(pro,mem1,net->vertex[i].id);
		post_processing(pro,result1,i,0.05);
	}
	printf("finish!\n");

	free(pro->index);
	free(pro->probability);

	//for(i=0;i<mem->nvertices;i++){
	//	free(mem->memory[i]);
	//}
	//free(mem->memory);
	destroy_memory(mem1);

	//printf("Output results...");
	//show_result_vertex(result1,fout);
	community_statistics(result1,result2);
	//show_result_community(result2,fout);

	nmi=1-(result_comparison(comp2,result2,net->nvertices)+result_comparison(result2,comp2,net->nvertices))/2;
	fprintf(fout,"%f\t",nmi);

	//删除被完全包含的社区
	result_processing(result1,result2);
	//show_result_community(result2,fout);
	//printf("finish!\n");

	
	printf("Calculating NMI...");
	//log((float)exp((float)1));
	nmi=1-(result_comparison(comp2,result2,net->nvertices)+result_comparison(result2,comp2,net->nvertices))/2;
	printf("finish!\n");

	fprintf(fout,"%f\n",nmi);

	for(i=0;i<result1->nvertices;i++){
		free(result1->communities[i]);
	}
	free(result1->communities);
	free(result1->numbelong);
		
	for(i=0;i<result2->ncommunities;i++){
		free(result2->vertices[i]);
	}
	free(result2->vertices);
	free(result2->nvertices);

	}
//}

	for(i=0;i<comp1->nvertices;i++){
		free(comp1->communities[i]);
	}
	free(comp1->communities);
	free(comp1->numbelong);

	for(i=0;i<comp2->ncommunities;i++){
		free(comp2->vertices[i]);
	}
	free(comp2->vertices);
	free(comp2->nvertices);

	free(dellist->label);
	free(dellist->nlabels);
	free(dellist);

	free(mem1);
	free(que);
	free(nei);
	free(sendlist);
	free_network(net);
	fclose(fout);
	fclose(fp);
	//printf("hello world!!");
	system("pause");
	return 0;
}