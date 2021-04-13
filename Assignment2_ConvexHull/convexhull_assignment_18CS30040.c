/*
    Assignment-2:   Convex Hull
    Author      :   Sriyash Poddar, 18CS30040 

    Complexity Analysis :
    1. Convex Hull: { O(NlogN) [Sorting] + O(N) [HULL FORMATION] } = O(NlogN)
    2. Containment Zone: O(NlogN) [Convex Hull] + O(N) [SEGMENTS and ARCS] = O(NlogN)
*/

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#define PI  3.141592653589793

typedef struct point_{
    /*struct to store point*/
    double x;
    double y;
}POINT;

// typedef struct circle_{
//   /*struct to store circle*/
//     POINT center;
// }CIRCLE;

typedef struct hull_{
    /*struct to store a hull stack*/
    int size;
    int capacity;
    POINT *H;
}HULL;

typedef struct line_{
    /*struct to store line*/
    POINT p1;
    POINT p2;
}LINE;

typedef struct arc_{
    /*struct to store arc*/
    POINT center;
    double start;
    double end;
}ARC;

HULL *init_hull(int n){
    /*Function to create a hull of max capacity*/
    HULL *temp = (HULL *)malloc(sizeof(HULL));
    temp->capacity = n;
    temp->size = 0;
    temp->H = (POINT *)malloc(sizeof(POINT)*n);
    return temp;
}

void push(HULL *hull, POINT a){
    /*Function to push value on stack*/
    hull->H[hull->size] = a;
    hull->size = hull->size+1;
}

void pop(HULL *hull){
    /*Function to pop top value from stack*/
    hull->size = hull->size-1;
}

POINT top(HULL *hull){
    /*Function to return top value from stack*/
    return hull->H[hull->size-1];
}

POINT top_second(HULL *hull){
    /*Function to return just below top value from stack*/
    return hull->H[hull->size-2];
}

int check_size(HULL *hull){
    /*Function to check if stack size if permissible to make lines*/
    return (hull->size > 1);
}

int check_turn(POINT p1, POINT p2, POINT p3){
    /*Function to check if given triplet is valid during Graham Scan*/
    double temp = p2.x*p3.y+p3.x*p1.y+p1.x*p2.y-
                 p3.x*p2.y-p1.x*p3.y-p2.x*p1.y;
    return (temp<=0.);
}

void merge(POINT *A, POINT *B, POINT *C, int a, int b){
    /*Function to merge two arrays during merge sort*/
    int i=0,j=0,k=0;
    while(i<a && j<b){
        if(A[i].x < B[j].x)
            C[k++] = A[i++];
        else
            C[k++] = B[j++];
    }
    while(i<a) C[k++] = A[i++];
    while(j<b) C[k++] = B[j++];
}

void sort(POINT *a, int n){
    /*Function to sort given array*/
    if(n<2) return;
    int i = n/2;
    sort(a, i);
    sort(a+i, n-i);
    POINT *temp = (POINT *)malloc(sizeof(POINT)*n);
    merge(a,a+i,temp,i,n-i);
    for(int i=0;i<n;i++) a[i] = temp[i];
    free(temp);
}

void reverse(POINT *a, int n){
    /*Function to reverse given array*/
    for(int i=0; i<n/2; i++){
        POINT temp = a[i];
        a[i] = a[n-i-1];
        a[n-i-1] = temp;
    }
}

int CH(POINT *S, int n, char flag, POINT *H){
    /*Function to find convex hull of the given array*/
    sort(S, n);
    if(flag == 'L')
        reverse(S, n);
    HULL *hull = init_hull(n);
    for(int i=0; i<n; i++){
        while(check_size(hull) && !check_turn(top_second(hull), top(hull), S[i]))
            pop(hull);
        push(hull, S[i]);
    }
    int h = hull->size;
    H = (POINT *)realloc(H, sizeof(POINT)*h);
    for(int i=0; i<h; i++) H[i] = hull->H[i];
    return h;
}

LINE get_line(POINT p1, POINT p2){
    /*Function to create line*/
    LINE temp;
    temp.p1 = p1;
    temp.p2 = p2;
    return temp;
}

ARC get_arc(POINT p, double start, double end){
    /*Function to create arc*/
    ARC temp;
    temp.center = p;
    temp.start = start;
    temp.end = end;
    return temp;
}

double get_slope(LINE line){
    /*Function to get slope of line*/
    return (line.p1.y-line.p2.y)/(line.p1.x-line.p2.x);
}

POINT get_segment_point(LINE l, POINT p, double r){
    /*Function to get segment point for the boundary*/
    double angle = atan(-1.0/get_slope(l));
    POINT p1 = {p.x + r*cos(angle), p.y + r*sin(angle)};
    POINT p2 = {p.x - r*cos(angle), p.y - r*sin(angle)};
    return check_turn(l.p1, l.p2, p1)?p2:p1;
}

double get_arc_angle(LINE line, char flag){
    /*Function to get arc angle for the boundary*/
    double angle = atan(get_slope(line));
    if (flag == 'U')
        return PI/2 + angle;
    else
        return -(PI/2 - angle);
}

void contzone(POINT *UH, int uh, POINT *LH, int lh, double r, LINE *T, ARC *A){
    /*Function to calculate the boundary of the containment zone*/
    int t=0, a=0, i=0;
    POINT pnull = {0,0};
    LINE temp;

    for(i=0; i<uh; i++){
        temp = get_line(UH[i], UH[i+1]);
        if (i==0)
            A[a] = get_arc(UH[0], PI, get_arc_angle(temp, 'U'));
        else if(i==uh-1)
            A[a] = get_arc(UH[i], A[a-1].end, 0.0);
        else
            A[a] = get_arc(UH[i], A[a-1].end, get_arc_angle(temp, 'U'));
        if(i != uh-1)
            T[t] = get_line(get_segment_point(temp,UH[i],r),get_segment_point(temp,UH[i+1],r));
        a++; t++;
    }

    for(i=0; i<lh; i++){
        temp = get_line(LH[i], LH[i+1]);
        if (i==0)
            A[a] = get_arc(LH[0], 0.0, get_arc_angle(temp, 'L'));
        else if(i==lh-1)
            A[a] = get_arc(LH[i], A[a-1].end, -PI);
        else
            A[a] = get_arc(LH[i], A[a-1].end, get_arc_angle(temp, 'L'));
        if(i != lh-1)
            T[t] = get_line(get_segment_point(temp,LH[i],r),get_segment_point(temp,LH[i+1],r));
        a++; t++;
    }
}

void printcontzone(int uh, int lh, LINE *T, ARC *A){
    /*Function to print the boundary*/
    for(int i=0; i<uh+lh; i++){
        if(i==0)  printf("--- Upper section\n");
        if(i==uh) printf("--- Lower section\n");

        printf("    Arc     : (%0.15lf,%0.15lf) From %0.15lf to %0.15lf\n", A[i].center.x, A[i].center.y, A[i].start, A[i].end);
        if(i != uh-1 && i != uh+lh-1)
            printf("    Tangent : From (%0.15lf,%0.15lf) to (%0.15lf,%0.15lf)\n", T[i].p1.x, T[i].p1.y, T[i].p2.x, T[i].p2.y);
    }
}

void print(POINT *S, int n){
    /*Function to print list of points*/
    for(int i=0; i<n; i++) printf("    %0.15lf %0.15lf \n", S[i].x, S[i].y);
}

int main(){
    /*Main function with required i/o and function calls*/
    // printf("Input file name : ");
    // char *fname = (char *)malloc(sizeof(char)*20);
    // scanf("%s",fname);
    // FILE *fin = fopen(fname, "r");

    int n; double RADIUS;
    // fscanf(fin, "%d %lf",&n, &RADIUS);
    scanf("%d %lf",&n, &RADIUS);
    POINT S[n];
    for(int i=0; i<n; i++){
        POINT temp;
        // fscanf(fin, "%lf%lf", &temp.x, &temp.y);
        scanf("%lf%lf", &temp.x, &temp.y);
        S[i] = temp;
    }

    sort(S, n);
    printf("\n+++ Circles after sorting\n");
    print(S, n);

    POINT *UH = (POINT *)malloc(sizeof(POINT)*n);
    int uh = CH(S, n, 'U', UH);
    printf("\n+++ Upper Hull\n");
    print(UH, uh);

    POINT *LH = (POINT *)malloc(sizeof(POINT)*n);
    int lh = CH(S, n, 'L', LH);
    printf("\n+++ Lower Hull\n");
    print(LH, lh);

    LINE *T = (LINE *)malloc(sizeof(LINE)*(uh+lh));
    ARC *A = (ARC *)malloc(sizeof(ARC)*(uh+lh));
    printf("\n+++ The containment zone\n");
    contzone(UH, uh, LH, lh, RADIUS, T, A);
    printcontzone(uh,lh,T,A);
}