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


int read_line(FILE *stream, char line[LINELENGTH])// line������Ϊһ���ַ�����
{
  if (fgets(line,LINELENGTH,stream)==NULL) return 1;  //ÿ�ζ���һ��
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
  LINE *previous;  //֮ǰ��ָ��


  if (read_line(stream,line1)!=0) {
    first = NULL;                // Indicates empty buffer
    return 1;
  }
  length = strlen(line1) + 1;
  first = (LINE *)malloc(sizeof(LINE));//first Ϊָ���һ���ڵ��ָ��
  first->str = (char *)malloc(length*sizeof(char));
  strcpy(first->str,line1);   //������ɽ�������line�ַ�����ֵ�� LINE�������С�
  nlines=1;

  previous = first;
  while (read_line(stream,line1)==0) { //��GML�ļ���ÿһ�д�����FIRST��ŵ������У���LINE�ṹ��Ϊ����Ľڵ㡣
    length = strlen(line1) + 1;           //��LINE�ṹ���еģ�str�ַ�ָ�룬�������Ӷ������ַ���������ļ�1��Ϊ�˴��\0���ַ���
    previous->ptr =(line *) malloc(sizeof(LINE));  
    previous = previous->ptr;
    previous->str = (char *)malloc(length*sizeof(char));
    strcpy(previous->str,line1);
	nlines++;
  }
  previous->ptr = NULL;          // Indicates last line  //�����һ���ڵ��ptr��ָ��Ϊ�գ�˵��������β��


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
    free(thisptr->str);  //�ͷ�thisptr��ָ����ַ���
    free(thisptr);      //�ͷ�LINE�ڵ�
    thisptr = nextptr;  //��thisptr�ͷź�thisptr��ǰ��
  }
}




// Function to reset to the start of the buffer again // ��Ŀǰָ�룬ָ��ʼ��λ��


void reset_buffer()
{
  current = first;
}




// Function to get the next line in the buffer.  Returns 0 if there was
// a line or 1 if we've reached the end of the buffer.


int next_line(char line[LINELENGTH])
{
  if (current==NULL) return 1;  //currentָ�룬ָ��ǰ��LINE�ڵ㣬Ȼ��LINE�ڵ��strָ����ָ����ַ�����ֵ��line���ַ�������
  strcpy(line,current->str);
  current = current->ptr;
  return 0;
}






// Function to establish whether the network read from a given stream is //�ж��������������ͼ��������ͼ
// directed or not.  Returns 1 for a directed network, and 0 otherwise.  If
// the GML file contains no "directed" line then the graph is assumed to be
// undirected, which is the GML default behavior.    //���GMLͼ�У������������ֱ�ߣ���ô������Ϊ��Ϊ����ͼ��


int is_directed()                                    //����ͼͬʱҲ��GMLĬ�ϵ���Ϊ��
{
  int result=0;
  char *ptr;
  char line[LINELENGTH];


  reset_buffer();  //ͨ����net_line����һ��ʹ�ã���Ϊ��������currentָ�룬������net_line��ʹ�á�


  while (next_line(line)==0) {
    ptr = strstr(line,"directed"); //strstr���������ã�������line�в���derected����ַ�����һ�γ��ֵ�λ�á�
    if (ptr==NULL) continue;
    sscanf(ptr,"directed %i",&result);//��ptr��ָ����ַ�������Ϊsscanf��������
    break;                           //������õ������ְ���ʮ���ƴ洢��result������
  }


  return result;              //ֻҪ������ͼ����directed����һ���������ݵģ�����resultֵ����Ϊ0�������жϡ�
}




// Function to count the vertices in a GML file.  Returns number of vertices.
//��ô������node�ڵ��أ������ҵ�"node ["��־��Ȼ����ͬһ������û��"label"��־����node�ڵ����Ŀ��1
int count_vertices()
{
  int result=0;
  char *ptr;
  char line[LINELENGTH];


  reset_buffer();  //����Ҳ������currentָ�룬��������ڵ����Ŀ


  while (next_line(line)==0) { // next_line�ɹ���ʱ�򷵻ص���0ֵ����LINE���ͣ���Ϊcurrentָ����ָ��ڵ��str������ָ�����һ������
    //ptr = strstr(line,"node ["); //����strstr�����Ǻ����õģ��൱������ƥ�䡣
    //if (ptr!=NULL) {
      //ptr = strstr(line,"label");//�����̫��⣬��ʵ��������gml��ʽ���ļ�����������txt��ʽ�򿪵����ӡ����ǴӸ�ʽ�Ͽ����񲻻�����Ǹ�����
      //if (ptr==NULL) result++;  //����Ҳ�����Ӧ���ַ�������ָ���ÿգ�����Ҳ���ܼ���ڵ����Ŀ 
    //}
	  ptr = strstr(line,"node");
	  if (ptr!=NULL)result++;
  }


  return result;
}




// Function to compare the IDs of two vertices


int cmpid(VERTEX *v1p, VERTEX *v2p)
{
  if (v1p->id>v2p->id) return 1;    //�Ƚ������ڵ��id�����ڵĻ�����1��С�ڵĻ�����-1.
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


  network->vertex = (VERTEX *)calloc(network->nvertices,sizeof(VERTEX));// ����calloc����һ�·������ڵ�Ŀռ䣬�����еĽڵ㴩������
                                                              //�����׵�ַ ��ֵ��network��vertexָ�롣�൱�ڹ�����һ������
  // Go through the file reading the details of each vertex one by one


  reset_buffer(); //����������õĺü�ʱ������������currentָ�롣ʹ����first��ͬ����ָͬ���һ��LINE�ڵ�
  for (i=0; i<network->nvertices; i++) {


    // Skip to next "node" entry


    do {
      next_line(line);
    } while (strstr(line,"node")==NULL); //�ȶ���Ȼ���жϡ���Ѱ��"node"ʱ��˵����һ��������Ϊnode�ڵ㡣


    // Read in the details of this vertex


    do {                                //��������Կ�����gml��ʽ�Ļ���


      // Look for ID //��ѯID


      ptr = strstr(line,"id");
      if (ptr!=NULL) sscanf(ptr,"id %d",&network->vertex[i].id);


      // Look for label //��ѯlabel


      ptr = (strstr(line,"label"));   //���ѭ��д�ĺð�����������˵�node��id��label�����ڲ�ͬ���е��������δ���
      if (ptr!=NULL) { //��Ǵ����������������������Ӧ��
      start = strchr(line,'"'); //��line�У�strchr(s,c).���ַ���s�У��״γ����ַ�c��λ�á�
    if (start==NULL) {       //""����˼�ǳ���"�ĵط�����Ϊ����ȷ��label�Ƿ����ַ��������ڵĻ�����ָ��c��ָ�룬�����ڵĻ�����NULL
     sscanf(ptr,"label %s",&label);
} 
else {
   stop = strchr(++start,'"');//�����Ļ������start=strstr(line,"label")��˵��ֻ����ʼλ�÷����˱仯��ԭ��Ϊline�ַ��������ʼλ��
   if (stop==NULL) length = strlen(line) - (start-line);         //����Ϊlabel��ǩ���µ��ַ����ĵ�һ���ַ���
   else length = stop - start;  //����������һ���жϣ���һ�����ǩ�Ƿ�Ϊһ�ַ�����ͬʱ����" ".
   strncpy(label,start,length);
   label[length] = '\0';
   network->vertex[i].label = (char *)malloc((length+1)*sizeof(char));//��label���Ը�length��Ԫ�ظ�ֵ��ʱ����Ԫ�صĸ���Ϊlength+1
   strcpy(network->vertex[i].label,label);//���֣�һ��label�������Զ���ģ�����һ��������vertex
}
      }//�ͱ�Ǵ���if���Ӧ��


      // If we see a closing square bracket we are done


      if (strstr(line,"]")!=NULL) break;  //�����Ļ��Ѿ��ҵ������ı�־�������ͺ��ˣ�ͬʱҲ���Կ���]Ӧ�ú�node[ ������һ���С�


    } while (next_line(line)==0);//���˵ptr=NULL�Ļ�����ô��ֱ���ٶ���һ��


  }


  // Sort the vertices in increasing order of their IDs so we can find them
  // quickly later  //�Խڵ��������Ϊ����Ľڵ������׼����


  qsort(network->vertex,network->nvertices,sizeof(VERTEX),(int (__cdecl *)(const void *, const void *))cmpid);
}




// Function to find a vertex with a specified ID using binary search.//ʹ�ö��ֲ��ң������Ҹ����ض�ID�Ľڵ㡣
// Returns the element in the vertex[] array holding the vertex in question,
// or -1 if no vertex was found.  //�ҵ��Ļ���������ڵ㣬�Ҳ����Ļ��ͷ���-1�����ֲ��ҵĳ����Ժ�ܿ��ܻ�������


int find_vertex(int id, NETWORK *network)
{
  int top,bottom,split;
  int idsplit;


  top = network->nvertices;
  if (top<1) return -1;
  bottom = 0;
  split = top/2; //�ڵ�ķָ�㣬��Ϊ���ʱ�������Ѿ����սڵ��ID�Ĵ�С�Ѿ��ź����ˣ����Ұ��յ��Ǵ�С�����˳��


  do {
    idsplit = network->vertex[split].id;//split��������Ӧ��IDΪ����
    if (id>idsplit) {
      bottom = split + 1;
      split = (top+bottom)/2;//�����ʱ��top��ֵΪ�����仯��ֻ������Ѱ��split��λ�á�
    } else if (id<idsplit) {
      top = split;
      split = (top+bottom)/2;//��ֵҪô��ҪôС��Ҫô��ֻ�е��ڡ�
    } else return split;
  } while (top>bottom);


  return -1;
}
    


// Function to determine the degrees of all the vertices by going through
// the edge data


void get_degrees(NETWORK *network)//��������Ķ�����û�м���Ȩֵ
{
  int s,t;  // �����st����ʾ��source�ڵ㣬��targetĿ��ڵ㡣
  int vs,vt;
  char *ptr;
  char line[LINELENGTH];


  reset_buffer(); //����currentָ�롣


  while (next_line(line)==0) {


    // Find the next edge entry


    ptr = strstr(line,"edge");
    if (ptr==NULL) continue;  //ֱ������"edge"���ַ���Ϊֹ��


    // Read the source and target of the edge


    s = t = -1; //��ʱ������˵���Ѿ��ҵ�edge���ڵ��С�


    do {


      ptr = strstr(line,"source");
      if (ptr!=NULL) sscanf(ptr,"source %i",&s);
      ptr = strstr(line,"target");
      if (ptr!=NULL) sscanf(ptr,"target %i",&t);


      // If we see a closing square bracket we are done


      if (strstr(line,"]")!=NULL) break;


    } while (next_line(line)==0); //����ı�ʾ��ʽ����֮ǰ���ڵ��ID�ͽڵ��label�����Ͼ�����ͬ�ģ�����ֿ����˵�һ�β�δ����source�����


    // Increment the degrees of the appropriate vertex or vertices


    if ((s>=0)&&(t>=0)) { //�������У��ҵ���Ӧ��ID����Ӧ��VERTEX�ڵ㡣���ı�vertex�ڵ�Ķ�����
      vs = find_vertex(s,network);
      network->vertex[vs].degree++;
      if (network->directed==0) { //����������Ϊ����ͼ��������Ŀ��ڵ�Ķ�����
    vt = find_vertex(t,network);
network->vertex[vt].degree++;
      }
    }//��Ӧǰ�������if���


  }//��Ӧ��������whileѭ��


  return;
}




// Function to read in the edges


void read_edges(NETWORK *network)
{
  int i;
  int s,t; //��Ӧgml�ļ��е�source�ڵ㣬��target�ڵ�
  int vs,vt; //��Ӧ��NETWORK�����е�Դ�ڵ���Ŀ��ڵ㡣
  int *count;
  double w;// ��Ӧ��Ȩֵ��������������õ�Ҳ����
  char *ptr;
  char line[LINELENGTH];


  // Malloc space for the edges and temporary space for the edge counts �߼���
  // at each vertex


  for (i=0; i<network->nvertices; i++) {
    network->vertex[i].edge = (EDGE *)malloc(network->vertex[i].degree*sizeof(EDGE));//�������ڴ洢�ڵ�i����Ӧ�ı�EDGE�ṹ����Ŀռ�ļ��ϡ�
  }
  count = (int *)calloc(network->nvertices,sizeof(int));//����n��int�洢�ռ䡣��������ĵ�ַ���Ƹ�countָ�����
                                                 //��Ӧÿһ���ڵ������ӵıߵ���Ŀ������˵count[12]=3;��ʾ���ǽڵ�12��������������
  // Read in the data                            //���ֵӦ�úͽڵ�Ķ���һֱ��
  memset(count,0,network->nvertices*sizeof(int));

  reset_buffer();//��ǰ�������һ��


  while (next_line(line)==0) {


    // Find the next edge entry


    ptr = strstr(line,"edge");
    if (ptr==NULL) continue;


    // Read the source and target of the edge and the edge weight


    s = t = -1;
    w = 1.0;


    do {


      ptr = strstr(line,"source"); //����ܼ򵥵Ľ�source��target��value��ֵ�ֱ����s,t,w�����������С�
      if (ptr!=NULL) sscanf(ptr,"source %i",&s);
      ptr = strstr(line,"target");
      if (ptr!=NULL) sscanf(ptr,"target %i",&t);
      ptr = strstr(line,"value");
      if (ptr!=NULL) sscanf(ptr,"value %lf",&w);


      // If we see a closing square bracket we are done


      if (strstr(line,"]")!=NULL) break;


    } while (next_line(line)==0);


    // Add these edges to the appropriate vertices


    if ((s>=0)&&(t>=0)) {   //��gml�ļ��У��ҵ�source�ڵ���target�ڵ㣬���ҵ������ڵ�֮����Ӧ��Ȩֵ��
      vs = find_vertex(s,network);
      vt = find_vertex(t,network);
      network->vertex[vs].edge[count[vs]].target = vt;//�տ�ʼʽ���е㳤���������ܸ��ӡ���ȷ��vetex֮�󣬶�target���и��ơ�
      network->vertex[vs].edge[count[vs]].weight = w;//count[vs]��δ��ֵ����count[vs]Ϊһ����������ʼֵΪ0.edge[count[vs]]Ҳֻ��һ��EDGE�Ľṹ��
      count[vs]++;                                  //count[vs]��ֵ�ڷ����仯������¼����ĳһ���ڵ���Ӧ�ıߵ���Ŀ��
      if (network->directed==0) { //������ЩȨֵ����NETWORK�е�
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


  for (i=0; i<network->nvertices; i++) { //���ͷŵ�ʱ�����ͷűߣ����ͷű�ǩ������ͷŽڵ㡣
    free(network->vertex[i].edge);
    free(network->vertex[i].label);
  }
  free(network->vertex);
}