/*
    Assignment-1:   Network Flow
    Author      :   Sriyash Poddar, 18CS30040 

    Complexity Analysis :
    1. MaxFlow: { O(E) [BFS] + O(E) [AUGMENTING PATH] }* O(VE) [TOTAL AUGMENTATIONS] * O(V) [EDGE_UPDATES] = O(V^2E^2)
    2. NeedBasedFlow: O(V) [GRAPH MODIFICATION] + O(V^2E^2) [MAXFLOW] = O(V^2E^2)
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define INF INT_MAX

typedef struct edge_
{/*struct to store edge*/
    int y;
    int c;
    int c_; //additional variable to store capacity
    int f;
    struct edge_ *next;
}EDGE;

typedef struct vertex_
{/*struct to store vertex*/
    int x;
    int n;
    struct edge_ *p;
}VERTEX;

typedef struct graph_
{/*struct to store graph*/
    int V;
    int E;
    struct vertex_ *H;
}GRAPH;

typedef struct node_
{/*struct to store node for queue*/
    int v;
    struct node_ *next;
}NODE;

typedef struct queue_
{/*struct for a queue for BFS*/
    struct node_ *head;
    struct node_ *tail;
}QUEUE;

int max(int a, int b){
    /*Function to return max of two no.s*/
    return (a>b)?a:b;
}

int min(int a, int b){
    /*Function to return min of two no.s*/
    return (a<b)?a:b;
}

int abs(int a){
    /*Function to return absolute value of a no.*/
    return (a>0)?a:-a;
}

int isEmpty(QUEUE queue){
    /*Functon to check if a queue is empty*/
    return (queue.head == NULL);
}

NODE *top(QUEUE queue){
    /*Function to get front/top value of queue*/
    return queue.head;
}

QUEUE dequeue(QUEUE queue){
    /*Function to pop value from queue*/
    NODE *temp = queue.head;
    queue.head = queue.head->next;
    if(queue.head==NULL)
        /*Empty queue*/
        queue.tail=NULL;
    return queue;
}

QUEUE enqueue(QUEUE queue, int e){
    /*Function to push value into queue*/
    NODE *temp = (NODE *)malloc(sizeof(NODE)) ;
    temp->v = e;
    temp->next = NULL;
    if(queue.tail == NULL){
        /*Adding to empty queue*/
        queue.tail = temp;
        queue.head = temp;
    }
    else{
        queue.tail->next = temp;
        queue.tail = temp;
    }
    return queue;
}

QUEUE init_queue(){
    /*Create an empty queue*/
    QUEUE queue;
    queue.head = NULL;
    queue.tail = NULL;
    return queue;
}

int getAugmentingPath(GRAPH *graph, int s, int t, int *parent){
    int distance[graph->V];
    int visited[graph->V];
    int path_visited[graph->V];

    for(int i=0; i<graph->V; i++){
        /*Arrays for BFS tree and augmenting paths*/
        visited[i] = 0;
        path_visited[i] = 0;
        distance[i] = -INF;
    }
    distance[s-1] = 0;
    visited[s-1] = 1;
    QUEUE queue = init_queue();
    queue = enqueue(queue, s);
    while(!isEmpty(queue)){
        /*Breadth First Search*/
        NODE *temp = top(queue);
        queue = dequeue(queue);
        EDGE *edge = graph->H[temp->v-1].p;
        while(edge!=NULL){
            if(!visited[edge->y-1] && edge->c > 0){
                queue = enqueue(queue, edge->y);
                distance[edge->y-1] = 1 + distance[temp->v-1];
                visited[edge->y-1] = 1;
            }
            edge = edge->next;
        }
    }
    /*Calculating min path length*/
    int min_path_length = distance[t-1];
    if(min_path_length == -INF)
        return -INF;
    
    /*Initialising variables for finding required augmenting path*/
    int max_res[graph->V];
    for(int i=0; i<graph->V; i++){
        max_res[i] = 0;
        parent[i] = -1;
    }
    queue = init_queue();
    queue = enqueue(queue, s);
    max_res[s-1] = INF;
    /*BFS from u->v, check if max_res[v]==max_res[u]+1, then update max_res[v] accordingly*/
    while(!isEmpty(queue)){
        NODE *temp = top(queue);
        queue = dequeue(queue);
        EDGE *edge = graph->H[temp->v-1].p;
        while(edge!=NULL){
            if(distance[edge->y-1] == distance[temp->v-1]+1 && edge->c >0){
                max_res[edge->y-1] = max(max_res[edge->y-1], min(edge->c, max_res[temp->v-1]));
                if(max_res[edge->y-1] == min(edge->c, max_res[temp->v-1])){
                    parent[edge->y-1] = temp->v;
                }
                queue = enqueue(queue, edge->y);
            }
            edge = edge->next;
        }
    }
    return max_res[t-1];
}

void update_edge_cap(GRAPH *graph, int des, int src, int res_cap){
    /*Updating the edge in case of push from src to des*/
    EDGE *edge = graph->H[src-1].p;
    while(edge != NULL){
        if(edge->y == des){
            edge->c -= res_cap;
            edge->f += res_cap;
            break;
        }
        edge = edge->next;
    }
}

void update_reverse_edge_cap(GRAPH *graph, int src, int des, int res_cap){
    /*Updating the reverse edge in case of push from des to src*/
    EDGE *edge = graph->H[src-1].p;
    while(edge != NULL){
        if(edge->y == des){
            edge->c += res_cap;
            edge->f -= res_cap;
            break;
        }
        edge = edge->next;
    }
    if(edge==NULL){
        /*Reverse edge doesnt exist*/
        EDGE *edge = (EDGE *)malloc(sizeof(EDGE));
        edge->f = -res_cap;
        edge->c = res_cap;
        edge->c_ = -1;
        edge->y = des;
        edge->next = graph->H[src-1].p;
        graph->H[src-1].p = edge;
    }
}

void ComputeMaxFlow(GRAPH *G, int s, int t){
    /*Function to get max flow*/
    int* parent = (int *)malloc(sizeof(int)*(G->V));
    int res_cap;
    int max_flow = 0;
    while((res_cap=getAugmentingPath(G,s,t,parent)) != -INF){
        int curr_vertex = t;
        max_flow += res_cap;
        while(parent[curr_vertex-1]!=-1){
            /*Updating residual graph*/
            update_edge_cap(G,curr_vertex, parent[curr_vertex-1], res_cap);
            update_reverse_edge_cap(G,curr_vertex, parent[curr_vertex-1], res_cap);
            curr_vertex = parent[curr_vertex-1];
        }
    }
    printf("FLOW VALUE: %d\n", max_flow);
}

void printGraph(GRAPH graph){
    /*Function to print graph*/
    for(int i=0; i<graph.V; i++){
        printf("%d", graph.H[i].x);
        EDGE *edge = graph.H[i].p;
        while(edge!=NULL){
            if(edge->c_ >= 0 && edge->y<=graph.V)
                printf(" -> ( %d, %d, %d )", edge->y, edge->c_, edge->f<0?0:edge->f);
            edge = edge->next;
        }
        printf("\n");
    }
}

int checkNeed(GRAPH *graph){
    /*Check if need based flow is possible*/
    int s= graph->V-1, t=graph->V;

    for(int i=0;i<graph->V-2;i++){
        EDGE *edge = graph->H[i].p;
        while(edge != NULL){
            if(edge->y==t){
                if(edge->c>0)
                /*consumer not satisfied*/
                    return 0;
                break;
            }
            edge = edge->next;
        }
    }

    EDGE *edge = graph->H[s-1].p;
    while(edge != NULL){
        /*producer not satisfied*/
        if(edge->c>0)
            return 0;
        edge = edge->next;
    }

    return 1;
}

void NeedBasedFlow(GRAPH *graph){
    /*Function to get need based flow*/
    graph->V += 2;
    graph->H = (VERTEX *)realloc(graph->H, sizeof(VERTEX)*(graph->V));
    /*Creating dummy source and sink which takes care of need based flow*/
    int s = graph->V-1, t = graph->V;
    graph->H[s-1].x = s;
    graph->H[s-1].p = NULL;
    graph->H[t-1].x = t;
    graph->H[t-1].p = NULL;
    for(int i=0;i<graph->V-2;i++){
        if(graph->H[i].n<0){
            EDGE *edge = (EDGE *)malloc(sizeof(EDGE));
            edge->f = 0;
            edge->y = i+1;
            edge->c = abs(graph->H[i].n);
            edge->next = graph->H[s-1].p;
            graph->H[s-1].p = edge;
        }
        else if(graph->H[i].n>0){
            EDGE *edge = (EDGE *)malloc(sizeof(EDGE));
            edge->f = 0;
            edge->y = t;
            edge->c = abs(graph->H[i].n);
            edge->next = graph->H[i].p;
            graph->H[i].p = edge;
        }
    }
    /*Function call to compute max flow*/
    ComputeMaxFlow(graph, s, t);
    if(checkNeed(graph)){
        /*Reverting graph to original*/
        graph->V -= 2;
        graph->H = (VERTEX *)realloc(graph->H, sizeof(VERTEX)*(graph->V));
        printGraph(*graph);
    }
    else{
        printf("NEED BASED FLOW NOT POSSIBLE: -1\n");
    }

}

GRAPH *ReadGraph(char *fname){
    /*Function to read graph*/
    FILE *fin = fopen(fname,"r");
    GRAPH *graph = (GRAPH *)malloc(sizeof(GRAPH));
    fscanf(fin, "%d %d", &(graph->V), &(graph->E));
    graph->H = (VERTEX *)malloc(sizeof(VERTEX)*graph->V);

    for(int i=0; i<graph->V; i++){
        graph->H[i].x = i+1;
        fscanf(fin, "%d", &(graph->H[i].n));
        graph->H[i].p = NULL;
    }

    for(int i=0; i<graph->E; i++){
        EDGE *edge = (EDGE *)malloc(sizeof(EDGE));
        int x; edge->f = 0;
        fscanf(fin, "%d %d %d", &(x), &(edge->y), &(edge->c));
        edge->c_ = edge->c;
        edge->next = graph->H[x-1].p;
        graph->H[x-1].p = edge;
    }
    fclose(fin);
    return graph;
}

int main(){
    /*Main function with required i/o and function calls*/
    printf("Input file name : ");
    char *fname = (char *)malloc(sizeof(char)*20);
    scanf("%s",fname);

    GRAPH *graph = ReadGraph(fname);
    printGraph(*graph);
    
    int s,t;
    printf("Input source and sink : ");
    scanf("%d %d", &s, &t);
    printf("\nMAX FLOW: \n");
    ComputeMaxFlow(graph, s, t);
    printGraph(*graph);

    graph = ReadGraph(fname);
    printf("\nNEED BASED FLOW: \n");
    NeedBasedFlow(graph);

    return 0;
}