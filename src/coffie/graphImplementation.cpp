/* 

  Implementation file
 
  Graph - adjacency list implementation based on code by Steven Skiena:

  Steven S. Skiena, "The Algorithm Design Manual", 2nd Edition, Springer, 2008.

  David Vernon
  19 March 2017

*/
 
#include "graph.h"

/* Breadth-First Search data structures */

bool processed[MAXV+1];   /* which vertices have been processed */
bool discovered[MAXV+1];  /* which vertices have been found */
int  parent[MAXV+1];      /* discovery relation */

bool debug = true;

/* Initialize graph  */

void initialize_graph(graph *g, bool directed){

   int i;                          /* counter */

   g -> nvertices = 0;
   g -> nedges = 0;
   g -> directed = directed;

   for (i=1; i<=MAXV; i++)
      g->degree[i] = 0;

   for (i=1; i<=MAXV; i++) 
      g->edges[i] = NULL; 
}

/* Initialize graph from data in a file                             */
/* return false when the number of vertices is zero; true otherwise */

bool read_graph(graph *g, bool directed) { 

   int i;    /* counter                */
   int m;    /* number of edges        */
   int x, y; /* vertices in edge (x,y) */
   int w;    /* weight on edge (x,y)   */

   initialize_graph(g, directed);

   printf("Enter the number of vertices >> ");
   scanf("%d",&(g->nvertices));
   printf("Enter the number of edges    >> ");
   scanf("%d",&m);

   //if (debug) printf("%d %d\n",g->nvertices,m);

   if (g->nvertices != 0) {
      for (i=1; i<=m; i++) {
         printf("Enter edge vertices, x and y, and weight, w >> ");
         scanf("%d %d %d",&x,&y,&w);
         //if (debug) printf("%d %d %d\n",x,y,w);
         insert_edge(g,x,y,directed, w);
      } 
      return(true);
   }
   else {
      return(false);
   }
}  




/* insert edge in a graphs */

void insert_edge(graph *g, int x, int y, bool directed, int w) {

   edgenode *p;                  /* temporary pointer */
      
   p = (EDGENODE_PTR) malloc(sizeof(edgenode)); /* allocate edgenode storage */
     //^^^^^^^^^^^^^^ ADDED CAST. DV 7/11/2014

   p->weight = w;
   p->y = y;
   p->next = g->edges[x];

   g->edges[x] = p;              /* insert at head of list        */

   g->degree[x] ++;

   if (directed == false)        /* NB: if undirected add         */
      insert_edge(g,y,x,true,w); /* the reverse edge recursively  */  
   else                          /* but directed TRUE so we do it */
      g->nedges ++;              /* only once                     */
}

/* Print a graph                                                    */

void print_graph(graph *g) {
        
   int i;                             /* counter           */
   edgenode *p;                       /* temporary pointer */

   printf("Graph adjacency list:\n");

   for (i=1; i<=g->nvertices; i++) {
      printf("%d: ",i);
      p = g->edges[i];
      while (p != NULL) {

         printf(" %d-%d", p->y, p->weight);

         p = p->next;
      }
      printf("\n");
   }
   printf("nvertices %d\n",g->nvertices);
   printf("nedges    %d\n",g->nedges);

}


bool read_graph_v2(FILE *fp_in, graph *g, bool directed, int num_vertices, int num_edges) {

   initialize_graph(g, directed);

   g->nvertices = num_vertices;
	//g->nedges = num_edges - 1;
   int i;
   int start_city = 0;
   int dest_city = 0;
   int weight_capacity = 0;

   for (i=0; i<num_edges; i++) {
	   fscanf(fp_in, "%d %d %d", &start_city, &dest_city, &weight_capacity);
	   insert_edge(g, start_city, dest_city, directed, weight_capacity);
   }
   return (true);
}


/*reset the start and destination corodinates to allow the graph to be built correctly*/
void reset_start_and_destination_coordinates(int *start_x, int *start_y, int *goal_x, int *goal_y){
	*start_x = 0;
	*start_y = 0;
	*goal_x = 0;
	*goal_y = 0;
}

/* Each vertex is initialized as undiscovered:                      */

void initialize_search(graph *g){
        
   int i;                          /* counter */
   
   for (i=1; i<=g->nvertices; i++) {
      processed[i] = discovered[i] = FALSE;
      parent[i] = -1;
   } 
}

/* Once a vertex is discovered, it is placed on a queue.           */
/* Since we process these vertices in first-in, first-out order,   */
/* the oldest vertices are expanded first, which are exactly those */
/* closest to the root                                             */

void bfs(graph *g, int start)
{
   queue q;                  /* queue of vertices to visit */
   int v;                    /* current vertex             */
   int y;                    /* successor vertex           */
   edgenode *p;              /* temporary pointer          */

   init_queue(&q);
   enqueue(&q,start);
   discovered[start] = TRUE;
   while (empty_queue(&q) == FALSE) {
      v = dequeue(&q);
      process_vertex_early(v);
      processed[v] = TRUE;
      p = g->edges[v];

      while (p != NULL) {  
         
         y = p->y;
         if ((processed[y] == FALSE) || g->directed)
            process_edge(v,y);
         if (discovered[y] == FALSE) {
            enqueue(&q,y);
            discovered[y] = TRUE;
            parent[y] = v;
         }

         p = p->next; 
      }
      process_vertex_late(v);
   }
}

/* The exact behaviour of bfs depends on the functions             */
/*    process vertex early()                                       */
/*    process vertex late()                                        */
/*    process edge()                                               */
/* These functions allow us to customize what the traversal does   */
/* as it makes its official visit to each edge and each vertex.    */
/* Here, e.g., we will do all of vertex processing on entry        */
/* (to print each vertex and edge exactly once)                    */
/* so process vertex late() returns without action                 */

void process_vertex_late(int v) {
	//printf("processed vertex %d\n",v);
}

void process_vertex_early(int v){
   //printf("processed vertex %d\n",v);
}

void process_edge(int x, int y) {
   //printf("processed edge (%d,%d)\n",x,y);
}

/* this version just counts the number of edges                     */
/*
void process_edge(int x, int y) {
   nedges = nedges + 1;
}
*/

int get_weight_between_parent_and_vertex(graph *g, int parent, int vertex){

	edgenode *p;
	int next_vertex;   /* candidate next vertex */
	int weight;
	int found_weight;
	p = g->edges[vertex];

	while (p != NULL) {

		next_vertex = p->y;
		weight = p->weight;

		//printf("found this wait for %d", weight);

		if (next_vertex == parent) { //found parent;

			//printf("Found this parent %d with weight %d ", next_vertex, weight);

			found_weight = weight;
		}
		p = p->next;
	}

	return found_weight;
}


/* adapted from original to return true if it is possible to reach the end from the start */

bool find_path(int start, int end, int parents[]) {
   
   bool is_path;

   if (end == -1) {
      is_path = false; // some vertex on the path back from the end has no parent (not counting start)
      if (debug) printf("\nis_path is false: ");
	  printf("\nis_path is false: ");
   }
   else if ((start == end)) {
       if (debug) printf("\n%d",start);
       is_path = true; // we have reached the start vertex
	   printf("* ");
   }
   else {
      is_path = find_path(start,parents[end],parents);
      if (debug) printf(" %d",end);
	  printf("# ");
   }
   return(is_path); 
}

bool find_path_modified(int start, int end, int parents[], graph *g, int *optimal_max_weight_array, int *num_elements, int *best_route_array, int *best_route_counter){

   bool is_path;

   if (end == -1) {
      is_path = false; // some vertex on the path back from the end has no parent (not counting start)
      //if (debug) printf("\nis_path is false: ");
	  printf("\nis_path is false: ");
   }
   else if ((start == end)) {
       //if (debug) printf("\n%d",start);
       is_path = true; // we have reached the start vertex
	   //printf("* ");
   }
   else {
      is_path = find_path_modified(start, parents[end], parents, g, optimal_max_weight_array, num_elements, best_route_array, best_route_counter);
      //if (debug) printf(" %d",end);
	  //printf("#");

	  // add best route in an array
	  best_route_array[*best_route_counter] = end;
	  *best_route_counter +=1;

	  //build an array of all weights between edgenode and parent
	  int curr_weight = get_weight_between_parent_and_vertex(g, parents[end], end);
	  optimal_max_weight_array[*num_elements] = curr_weight;
	  *num_elements+=1;
	  
   }
   return(is_path); 

}
 
/* DV abstract version that hides implementation by removing the parent array from the parameter list */
/* leaving only parameters that can be passed as arguments from the application code                  */

bool find_path(graph *g, int start, int end, int *optimal_max_weight_array, int *num_elements, int *best_route_array, int *best_route_counter) {
   bool is_path;

   //if (debug) printf("Path from %d to %d:",start,end);

   if ((start < 1) || (start > g->nvertices)) {
      if (debug) 
         printf("Invalid start vertex\n");
      is_path = false;
   }
   else if ((end < 1) || (end > g->nvertices)) {
      if (debug) 
         printf("Invalid end vertex\n");
      is_path = false;
   }
   else {
      initialize_search(g);
      prim(g, start);
	  is_path = find_path_modified(start, end, parent, g, optimal_max_weight_array, num_elements, best_route_array, best_route_counter);
      //is_path = find_path(start, end, parent); // now call the version of find_path that has the parent array as an argument
      if (debug) printf("\n");
   }
   return(is_path);
}



/*	queue

	Implementation of a FIFO queue abstract data type.

	by: Steven Skiena
	begun: March 27, 2002
*/


/*
Copyright 2003 by Steven S. Skiena; all rights reserved. 

Permission is granted for use in non-commerical applications
provided this copyright notice remains intact and unchanged.

This program appears in my book:

"Programming Challenges: The Programming Contest Training Manual"
by Steven Skiena and Miguel Revilla, Springer-Verlag, New York 2003.

See our website www.programming-challenges.com for additional information.

This book can be ordered from Amazon.com at

http://www.amazon.com/exec/obidos/ASIN/0387001638/thealgorithmrepo/

*/
 
void init_queue(queue *q)
{
        q->first = 0;
        q->last = QUEUESIZE-1;
        q->count = 0;
}

void enqueue(queue *q, item_type x)
{
        if (q->count >= QUEUESIZE)
		printf("Warning: queue overflow enqueue x=%d\n",x);
        else {
                q->last = (q->last+1) % QUEUESIZE;
                q->q[ q->last ] = x;    
                q->count = q->count + 1;
        }
}

item_type dequeue(queue *q)
{
        item_type x;

        if (q->count <= 0) printf("Warning: empty queue dequeue.\n");
        else {
                x = q->q[ q->first ];
                q->first = (q->first+1) % QUEUESIZE;
                q->count = q->count - 1;
        }

        return(x);
}

item_type headq(queue *q)
{
	return( q->q[ q->first ] );
}

int empty_queue(queue *q)
{
        if (q->count <= 0) return (TRUE);
        else return (FALSE);
}

void print_queue(queue *q)
{
        int i;

        i=q->first; 
        
        while (i != q->last) {
                printf("%d ",q->q[i]);
                i = (i+1) % QUEUESIZE;
        }

        printf("%2d ",q->q[i]);
        printf("\n");
}


void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal\n");
   getchar();
   getchar();
   exit(status);
}

/* Code for Prim's Algorithm adapted from Stephen Skiena's textbook*/

void prim(graph *g, int start) {

	int i; /* counter */
	edgenode *p; /* temporary pointer */

	bool intree[MAXV+1]; /* is the vertex in the tree yet? */
	int distance[MAXV+1]; /* cost of adding to tree */

	int v; /* current vertex to process */
	int w; /* candidate next vertex */
	int weight; /* edge weight */
	int dist; /* best current distance from start */

	for (i=1; i<=g->nvertices; i++) {
		intree[i] = FALSE;
		distance[i] = MAXINT; //change this later
		parent[i] = -1;
	}

	distance[start] = 0;
	v = start;

	while (intree[v] == FALSE) {
		intree[v] = TRUE;
		p = g->edges[v];

		while (p != NULL) {
			w = p->y;
			weight = p->weight;

			if ((distance[w] < weight) && (intree[w] == FALSE)) {
				distance[w] = weight;
				parent[w] = v;
			}
			p = p->next;
		}

		v = 1;
		dist = MAXINT;

		for (i=1; i<=g->nvertices; i++)
			if ((intree[i] == FALSE) && (dist < distance[i])) {
			dist = distance[i];
			v = i;
		}
	}
}
int get_minimum_element(int *my_array, int num_elements){

	int i;
	int min_element = my_array[0];

	for (i=0; i < num_elements; i++){

		if (my_array[i] < min_element){
			min_element = my_array[i];
		}
	}
	return min_element;
}