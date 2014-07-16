// Functions to read a network stored in a GML file into a NETWORK struct
//
// Mark Newman  11 AUG 06
//
// To use this software, #include "readgml.h" at the head of your program
// and then call the following.
//
// Function calls:
//   int read_network(NETWORK *network, FILE *stream)
//     -- Reads a network from the FILE pointed to by "stream" into the
//        structure "network".  For the format of NETWORK structs see file
//        "network.h".  Returns 0 if read was successful.
//   void free_network(NETWORK *network)
//     -- Destroys a NETWORK struct again, freeing up the memory




// Inclusions


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "network.h"
#include "datastruct.h"


// Constants


#define LINELENGTH 1000


// Types


typedef struct line {
  char *str;
  struct line *ptr;
} LINE;


// Globals


LINE *first;
LINE *current;

int *source, *target;

int nlines=0;


// Function to read one line from a specified stream.  Return value is
// 1 if an EOF was encountered.  Otherwise 0.


int read_line(FILE *stream, char line[LINELENGTH])// line在这里为一个字符数组
{
  if (fgets(line,LINELENGTH,stream)==NULL) return 1;  //每次都读一行
  line[strlen(line)-1] = '\0';   // Erase the terminating NEWLINE
  return 0;
}




// Function to read in the whole file into a linked-list buffer, so that we
// can do several passes on it, as required to read the GML format
// efficiently


int fill_buffer(FILE *stream)
{
  int length;
  char line1[LINELENGTH];
  LINE *previous;  //之前的指针


  if (read_line(stream,line1)!=0) {
    first = NULL;                // Indicates empty buffer
    return 1;
  }
  length = strlen(line1) + 1;
  first = (LINE *)malloc(sizeof(LINE));//first 为指向第一个节点的指针
  first->str = (char *)malloc(length*sizeof(char));
  strcpy(first->str,line1);   //用于完成将读到到line字符串赋值到 LINE的链表中。
  nlines=1;

  previous = first;
  while (read_line(stream,line1)==0) { //将GML文件的每一行存在以FIRST存放的链表中，而LINE结构体为链表的节点。
    length = strlen(line1) + 1;           //而LINE结构体中的，str字符指针，用来连接读到到字符串，这里的加1是为了存放\0空字符。
    previous->ptr =(line *) malloc(sizeof(LINE));  
    previous = previous->ptr;
    previous->str = (char *)malloc(length*sizeof(char));
    strcpy(previous->str,line1);
	nlines++;
  }
  previous->ptr = NULL;          // Indicates last line  //将最后一个节点的ptr的指向为空，说明链表到了尾点


  return 0;
}




// Function to free up the buffer again


void free_buffer()
{
  LINE *thisptr;
  LINE *nextptr;


  thisptr = first;
  while (thisptr!=NULL) {
    nextptr = thisptr->ptr;
    free(thisptr->str);  //释放thisptr所指向的字符串
    free(thisptr);      //释放LINE节点
    thisptr = nextptr;  //将thisptr释放后，thisptr向前移
  }
}




// Function to reset to the start of the buffer again // 让目前指针，指向开始的位置


void reset_buffer()
{
  current = first;
}




// Function to get the next line in the buffer.  Returns 0 if there was
// a line or 1 if we've reached the end of the buffer.


int next_line(char line[LINELENGTH])
{
  if (current==NULL) return 1;  //current指针，指向当前的LINE节点，然后LINE节点的str指针所指向的字符串赋值到line的字符数组中
  strcpy(line,current->str);
  current = current->ptr;
  return 0;
}






// Function to establish whether the network read from a given stream is //判断这个网络是有向图还是无向图
// directed or not.  Returns 1 for a directed network, and 0 otherwise.  If
// the GML file contains no "directed" line then the graph is assumed to be
// undirected, which is the GML default behavior.    //如果GML图中，不包含有向的直线，那么我们认为其为无向图。


int is_directed()                                    //无向图同时也是GML默认的行为。
{
  int result=0;
  char *ptr;
  char line[LINELENGTH];


  reset_buffer();  //通常和net_line函数一起使用，因为它重置了current指针，可以再net_line中使用。


  while (next_line(line)==0) {
    ptr = strstr(line,"directed"); //strstr函数的作用，就是在line中查找derected这个字符串第一次出现的位置。
    if (ptr==NULL) continue;
    sscanf(ptr,"directed %i",&result);//以ptr所指向的字符串，作为sscanf语句的输入
    break;                           //并将其得到的数字按照十进制存储在result变量中
  }


  return result;              //只要是有向图，其directed后面一定会有数据的，这样result值不会为0，用以判断。
}




// Function to count the vertices in a GML file.  Returns number of vertices.
//怎么来查找node节点呢？首先找到"node ["标志，然后在同一行里面没有"label"标志，则将node节点的数目加1
int count_vertices()
{
  int result=0;
  char *ptr;
  char line[LINELENGTH];


  reset_buffer();  //这样也是重置current指针，用来计算节点的数目


  while (next_line(line)==0) { // next_line成功的时候返回的是0值。将LINE类型，名为current指针所指向节点的str返回所指向的那一行数据
    //ptr = strstr(line,"node ["); //其中strstr函数是很有用的，相当于用来匹配。
    //if (ptr!=NULL) {
      //ptr = strstr(line,"label");//这个不太理解，其实所看到的gml格式的文件，不像是用txt格式打开的样子。但是从格式上看好像不会出现那个样子
      //if (ptr==NULL) result++;  //如果找不到相应的字符串，则将指针置空，但是也不能计算节点的数目 
    //}
	  ptr = strstr(line,"node");
	  if (ptr!=NULL)result++;
  }


  return result;
}




// Function to compare the IDs of two vertices


int cmpid(VERTEX *v1p, VERTEX *v2p)
{
  if (v1p->id>v2p->id) return 1;    //比较两个节点的id，大于的话返回1，小于的话返回-1.
  if (v1p->id<v2p->id) return -1;
  return 0;
}




// Function to allocate space for a network structure stored in a GML file
// and determine the parameters (id, label) of each of the vertices.


void create_network(NETWORK *network)
{
  int i;
  int length;
  char *ptr;
  char *start,*stop;
  char line[LINELENGTH];
  char label[LINELENGTH];


  // Determine whether the network is directed


  network->directed = is_directed();


  // Count the vertices


  network->nvertices = count_vertices();


  // Make space for the vertices


  network->vertex = (VERTEX *)calloc(network->nvertices,sizeof(VERTEX));// 运用calloc函数一下分配多个节点的空间，将所有的节点穿成链表。
                                                              //并将首地址 赋值给network的vertex指针。相当于构建了一个数组
  // Go through the file reading the details of each vertex one by one


  reset_buffer(); //这个函数调用的好及时啊，重新设置current指针。使其与first相同，共同指向第一个LINE节点
  for (i=0; i<network->nvertices; i++) {


    // Skip to next "node" entry


    do {
      next_line(line);
    } while (strstr(line,"node")==NULL); //先读，然后判断。当寻到"node"时，说明这一行描述的为node节点。


    // Read in the details of this vertex


    do {                                //从上面可以看出，gml格式的话，


      // Look for ID //查询ID


      ptr = strstr(line,"id");
      if (ptr!=NULL) sscanf(ptr,"id %d",&network->vertex[i].id);


      // Look for label //查询label


      ptr = (strstr(line,"label"));   //这个循环写的好啊！充分体现了当node、id、label三者在不同的行的情况下如何处理。
      if (ptr!=NULL) { //标记处！！！和最外层的括号相对应。
      start = strchr(line,'"'); //在line中，strchr(s,c).在字符串s中，首次出现字符c的位置。
    if (start==NULL) {       //""的意思是出现"的地方，因为不能确定label是否是字符串，存在的话返回指向c的指针，不存在的话返回NULL
     sscanf(ptr,"label %s",&label);
} 
else {
   stop = strchr(++start,'"');//这样的话相对于start=strstr(line,"label")来说，只是起始位置发生了变化，原先为line字符数组的起始位置
   if (stop==NULL) length = strlen(line) - (start-line);         //现在为label标签项下的字符串的第一个字符。
   else length = stop - start;  //在这里做了一个判断，看一看其标签是否为一字符串，同时具有" ".
   strncpy(label,start,length);
   label[length] = '\0';
   network->vertex[i].label = (char *)malloc((length+1)*sizeof(char));//当label可以给length个元素赋值的时候，其元素的个数为length+1
   strcpy(network->vertex[i].label,label);//区分，一个label是属于自定义的，而另一个则属于vertex
}
      }//和标记处的if相对应。


      // If we see a closing square bracket we are done


      if (strstr(line,"]")!=NULL) break;  //这样的话已经找到结束的标志，结束就好了，同时也可以看到]应该和node[ 并不在一行中。


    } while (next_line(line)==0);//如果说ptr=NULL的话，那么就直接再读下一行


  }


  // Sort the vertices in increasing order of their IDs so we can find them
  // quickly later  //对节点进行排序，为后面的节点查找做准备。


  qsort(network->vertex,network->nvertices,sizeof(VERTEX),(int (__cdecl *)(const void *, const void *))cmpid);
}




// Function to find a vertex with a specified ID using binary search.//使用二分查找，来查找给予特定ID的节点。
// Returns the element in the vertex[] array holding the vertex in question,
// or -1 if no vertex was found.  //找到的话返回这个节点，找不到的话就返回-1，二分查找的程序以后很可能会遇到。


int find_vertex(int id, NETWORK *network)
{
  int top,bottom,split;
  int idsplit;


  top = network->nvertices;
  if (top<1) return -1;
  bottom = 0;
  split = top/2; //节点的分割点，因为这个时候，链表已经按照节点的ID的大小已经排好序了，并且按照的是从小到大的顺序。


  do {
    idsplit = network->vertex[split].id;//split链表处所对应的ID为多少
    if (id>idsplit) {
      bottom = split + 1;
      split = (top+bottom)/2;//在这个时候top的值为发生变化，只是重新寻找split的位置。
    } else if (id<idsplit) {
      top = split;
      split = (top+bottom)/2;//其值要么大，要么小，要么就只有等于。
    } else return split;
  } while (top>bottom);


  return -1;
}
    


// Function to determine the degrees of all the vertices by going through
// the edge data


void get_degrees(NETWORK *network)//这里计算点的度数并没有计算权值
{
  int s,t;  // 这里的st，表示的source节点，与target目标节点。
  int vs,vt;
  char *ptr;
  char line[LINELENGTH];


  reset_buffer(); //重置current指针。


  while (next_line(line)==0) {


    // Find the next edge entry


    ptr = strstr(line,"edge");
    if (ptr==NULL) continue;  //直到遇到"edge"的字符串为止。


    // Read the source and target of the edge


    s = t = -1; //此时的请求，说明已经找到edge所在的行。


    do {


      ptr = strstr(line,"source");
      if (ptr!=NULL) sscanf(ptr,"source %i",&s);
      ptr = strstr(line,"target");
      if (ptr!=NULL) sscanf(ptr,"target %i",&t);


      // If we see a closing square bracket we are done


      if (strstr(line,"]")!=NULL) break;


    } while (next_line(line)==0); //这个的表示方式，和之前将节点的ID和节点的label基本上就是相同的，并充分考虑了第一次并未遇到source的情况


    // Increment the degrees of the appropriate vertex or vertices


    if ((s>=0)&&(t>=0)) { //在网络中，找到相应的ID所对应的VERTEX节点。并改变vertex节点的度数。
      vs = find_vertex(s,network);
      network->vertex[vs].degree++;
      if (network->directed==0) { //如果这个网络为无向图，则增加目标节点的度数。
    vt = find_vertex(t,network);
network->vertex[vt].degree++;
      }
    }//对应前面的两个if语句


  }//对应的是外层的while循环


  return;
}




// Function to read in the edges


void read_edges(NETWORK *network)
{
  int i;
  int s,t; //对应gml文件中的source节点，和target节点
  int vs,vt; //对应于NETWORK网络中的源节点与目标节点。
  int *count;
  double w;// 对应于权值，这里的类型设置的也不错
  char *ptr;
  char line[LINELENGTH];


  // Malloc space for the edges and temporary space for the edge counts 边计数
  // at each vertex


  for (i=0; i<network->nvertices; i++) {
    network->vertex[i].edge = (EDGE *)malloc(network->vertex[i].degree*sizeof(EDGE));//分配用于存储节点i所对应的边EDGE结构的题的空间的集合。
  }
  count = (int *)calloc(network->nvertices,sizeof(int));//分配n个int存储空间。并将分配的地址复制给count指针变量
                                                 //对应每一个节点所连接的边的数目，比如说count[12]=3;表示的是节点12有三条边相连。
  // Read in the data                            //这个值应该和节点的度数一直啊
  memset(count,0,network->nvertices*sizeof(int));

  reset_buffer();//和前面的作用一样


  while (next_line(line)==0) {


    // Find the next edge entry


    ptr = strstr(line,"edge");
    if (ptr==NULL) continue;


    // Read the source and target of the edge and the edge weight


    s = t = -1;
    w = 1.0;


    do {


      ptr = strstr(line,"source"); //这里很简单的将source、target、value的值分别存于s,t,w这三个变量中。
      if (ptr!=NULL) sscanf(ptr,"source %i",&s);
      ptr = strstr(line,"target");
      if (ptr!=NULL) sscanf(ptr,"target %i",&t);
      ptr = strstr(line,"value");
      if (ptr!=NULL) sscanf(ptr,"value %lf",&w);


      // If we see a closing square bracket we are done


      if (strstr(line,"]")!=NULL) break;


    } while (next_line(line)==0);


    // Add these edges to the appropriate vertices


    if ((s>=0)&&(t>=0)) {   //在gml文件中，找到source节点与target节点，并找到两个节点之间多对应的权值。
      vs = find_vertex(s,network);
      vt = find_vertex(t,network);
      network->vertex[vs].edge[count[vs]].target = vt;//刚开始式子有点长，看起来很复杂。在确定vetex之后，对target进行复制。
      network->vertex[vs].edge[count[vs]].weight = w;//count[vs]并未有值啊？count[vs]为一个变量，起始值为0.edge[count[vs]]也只是一个EDGE的结构体
      count[vs]++;                                  //count[vs]的值在发生变化，最后记录的是某一个节点多对应的边的数目。
      if (network->directed==0) { //并将这些权值赋予NETWORK中的
network->vertex[vt].edge[count[vt]].target = vs;
network->vertex[vt].edge[count[vt]].weight = w;
count[vt]++;
      }
    }


  }


  free(count);
  return;
}

int read_datas()
{
	int s,t,i=0;
	char line[LINELENGTH];

	if(nlines == 0)return 0;

	source=(int *)malloc(nlines*sizeof(int));
	target=(int *)malloc(nlines*sizeof(int));

	reset_buffer();

	while(!next_line(line)){
		sscanf(line,"%d\t%d",&s,&t);
		source[i]=s-1;
		target[i]=t-1;
		i++;
	}

	return 1;
}

int count_vertex(int *sortedlabel, int *countvetex, int totallabel)
{
	int i,tmp,tmpindex,numlabel;
	tmp=sortedlabel[0];
	tmpindex=0;
	numlabel=1;
	for(i=1;i<totallabel;i++){
		if(sortedlabel[i]==tmp)countvetex[tmpindex]++;
		else{
			tmp=sortedlabel[i];
			tmpindex=i;
			numlabel++;
		}
	}
	return numlabel;
}


void free_datas()
{
	free(source);
	free(target);
}



int create_network_bn(NETWORK *net)
{
	int i,j,k,l;

	int *countvertex;

	countvertex=(int *)malloc(nlines*sizeof(int));
	for(i=0;i<nlines;i++)
		countvertex[i]=1;

	read_datas();

	count_vertex(source,countvertex,nlines);

	net->directed = 0;
	net->nvertices = source[nlines-1]+1;

	net->vertex=(VERTEX *)malloc(net->nvertices*sizeof(VERTEX));

	for(i=0,j=0,k=0;i<net->nvertices;i++){

		if((i)!=source[j])continue;

		net->vertex[i].degree=countvertex[j];
		net->vertex[i].edge=(EDGE *)malloc(countvertex[j]*sizeof(EDGE));
		net->vertex[i].id=i;
		net->vertex[i].label=NULL;

		for(l=0;j<k+net->vertex[i].degree;j++,l++){

			net->vertex[i].edge[l].target=target[j];
			net->vertex[i].edge[l].weight=1;
			
		}

		k+=net->vertex[i].degree;
	}

	free_datas();

	return 1;
}


int create_communities(RESULT_VERTEX *comp)
{
	int i,j,k,flag;
	char line[LINELENGTH];
	int tmp[LINELENGTH];

	if(nlines == 0)return 0;

	comp->nvertices=nlines;
	comp->numbelong=(int *)calloc(nlines, sizeof(int));
	comp->communities=(int **)malloc(nlines*sizeof(int*));

	reset_buffer();

	j=0;

	while(!next_line(line)){
		i=0;
		k=0;
		flag=0;
		while(line[i]!=0&&line[i]!=EOF){

			if(48<=line[i]&&line[i]<=57&&!flag){
				tmp[k++]=atoi(line+i);
				flag=1;
			}
			else{
				if(line[i]<48||line[i]>57){
					flag=0;
				}
			}
			i++;
		}

		comp->numbelong[j]=k-1;
		comp->communities[j]=(int *)malloc((k-1)*sizeof(int));

		for(k=0;k<comp->numbelong[j];k++){
			comp->communities[j][k]=tmp[k+1];
		}

		j++;
	}

	return 1;
}

int read_network_bn(NETWORK *network, FILE *stream)
{
	fill_buffer(stream);
	
	create_network_bn(network);

	free_buffer();


  return 0;
}


int read_communities(RESULT_VERTEX *comp, FILE *stream)
{
	fill_buffer(stream);
	create_communities(comp);
	free_buffer();

	return 0;
}



// Function to read a complete network


int read_network(NETWORK *network, FILE *stream)
{
  fill_buffer(stream);
  create_network(network);
  get_degrees(network);
  read_edges(network);
  free_buffer();


  return 0;
}




// Function to free the memory used by a network again


void free_network(NETWORK *network)
{
  int i;


  for (i=0; i<network->nvertices; i++) { //在释放的时候，先释放边，在释放标签，最后释放节点。
    free(network->vertex[i].edge);
    free(network->vertex[i].label);
  }
  free(network->vertex);
}