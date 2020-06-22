/* 

    assignment6Application.cpp - Minimizing the number of scheduled buses needed to take tourists to their destination
   ==============================================================================================================

   Author
   ===========================================================================================

   Isaac Coffie, MSIT Carnegie Mellon University Africa
   April 15 2020


   The functionality of the program is defined as follows.
   ============================================================================================

   The purpose of this assignment is to design a program to allow a travel agent find the efficient route to take all tourists to the destination city in the
	minimum number of trips. 
   
   This data for the city (graph) is stored in an input file (input.txt) with a number of test cases 
   For each test case, we apply the algorithm on the graph and the resulting solution is printed to the output.txt file
   
  Format of the input and output file are defined below
  =============================================================================================

   Input file
   ----------
	7 10
	1 2 30
	1 3 15
	1 4 10
	2 4 25
	2 5 60
	3 4 40
	3 6 20
	4 7 35
	5 7 20
	6 7 30
	1 7 99
	6 6
	1 2 30
	1 3 15
	2 4 20
	3 5 11
	4 6 10
	5 6 20
	1 6 49


   Output file
   ------
   The output file is formatted in the following way:

   first line is my Andrew ID
   The remaining line containts the shortest path route from the start to goal indices of the map

    coffie
	Scenario 1
	Minimum Number of Trips = 5:   24  24  24  24  3
	Route = 1  2  4  7 

	Scenario 2
                    
	Minimum Number of Trips = 5:   24  24  24  24  3
	Route = 1  2  4  7 

	And in that order...



   Solution Strategy / Summary of the Algorithm
   =================================================================================
   I developed some helper functions to help address the problem by breaking the task into smaller pieces
   I, however, spent my time modifying the Prim's algorithm which is a good choice for this problem
   The standard Prim algorithm selects vertices that have minimum cost with any vertex in the Prim's Tree. However, because we want to minimize the maximum bus stop capacity, 
   I had to adapt the algorithm such that it selects vertices that has bigger weights on the edges with any vertex in the Prim's tree
   
   I also had to keep track of the weights of the edges or paths that were selected by the algorithm. To do so, I create a helper function
   that computes the distance between the selected vertices with its parents. I then return the minimum weight, which is the bottleneck

   Since the route is the same for a city, I equally had to compute the number of trips that are requried to take the passengers from the start to destination city


   
   Test Strategy
   =================================================================================

   This program has been tested with variety of test data sets that reflect all valid conditions, including boundary conditions:

	#TEST 1: input file with empty content
			
			INPUT:

			
			EXPECTED OUTPUT:
			no content in the output file other than my AndrewID

	#TEST 2: input file with one scenario
			
			coffie
			Scenario 1
			Minimum Number of Trips = 5:   24  24  24  24  3
			Route = 1  2  4  7

	# TEST 3: graph with no route or path between start and destination city
			RESULT: No Path Found for start vertex X and destination vertex Y

	#TEST 4: path with same start vertex and destination vertex number
			RESULT: Start Vertex 2 is the same as destination vertex 2 . Cannot be allowed

	#TEST 5: Number of vertices more than 20
			RESULT: Number of vertices 21 more than the limit of 20... Not processing route

	#TEST 6: Reverse the order of path finding.. such as from 4 to 1
			RESULT: Minimum Number of Trips = 5:   7  7  7  7  7
					Route = 4  2  1
	#TEST 7: Transporting zero (0) passengers
			RESULT: Cannot transport 0 number of passengers

*/
 
#include "graph.h"


int main() {

   bool debug = false;
   FILE *fp_in, *fp_out;
   graph g;
   bool directed = false;

   //main variables
   int scenario = 1; 
   int num_vertices = 0;
   int num_edges = 0;
   int start_city = 0;
   int destination_city = 0;
   int total_number_tourists = 0;
   int optimal_max_weight_array[MAXV];
   int num_elements = 0;
   int best_route_array[MAXV];
   int best_route_counter = 0;

   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input input.txt\n");
     getchar();
     exit(0);
   }

   if ((fp_out = fopen("../data/output.txt","w")) == 0) {
	  printf("Error can't open output output.txt\n");
     getchar();
     exit(0);
   }


   fprintf(fp_out, "coffie\n");

   //read test cases from file

   while(fscanf(fp_in, "%d %d", &num_vertices, &num_edges) != EOF){

	   //break out of the loop or end program when we encounter 0 values for both cities and road segment
		if(num_vertices == 0 && num_edges == 0) break;

		fprintf(fp_out, "Scenario %d\n", scenario);

		//read the graph
		read_graph_v2(fp_in, &g, directed, num_vertices, num_edges);

		//read the start, destination and number of passengers
		fscanf(fp_in, "%d %d %d",&start_city, &destination_city, &total_number_tourists);

		// check for same start vertex and destination vertex
		if(start_city == destination_city){
			fprintf(fp_out, "Start Vertex %d is the same as destination vertex %d . Cannot be allowed\n", start_city, destination_city);
			fprintf(fp_out, "\n");
			scenario += 1;
			continue;
		}

		//check for zero number of passengers
		if(total_number_tourists <= 0){
			fprintf(fp_out, "Cannot transport %d number of passengers\n", total_number_tourists);
			fprintf(fp_out, "\n");
			scenario += 1;
			continue;
		}

		//check for more than 20 numvertices
		if(num_vertices > 20){
			//read the graph
			read_graph_v2(fp_in, &g, directed, num_vertices, num_edges);
			//read the start, destination and number of passengers
			fscanf(fp_in, "%d %d %d",&start_city, &destination_city, &total_number_tourists);

			fprintf(fp_out, "Number of vertices %d more than the limit of 20... Not processing route\n", num_vertices);
			fprintf(fp_out, "\n");

			scenario += 1;

			continue;
		}

		// if there is path found
		if(find_path(&g, start_city, destination_city, optimal_max_weight_array, &num_elements, best_route_array, &best_route_counter)){

			//get minimum weight in graph
			int i;
			int min_max_capacity = get_minimum_element(optimal_max_weight_array, num_elements);

			//get minimum number of trips
			int min_num_trips = total_number_tourists / (min_max_capacity -1);
			int min_num_trips_remainder = total_number_tourists % (min_max_capacity -1);

			if(min_num_trips_remainder > 0) min_num_trips += 1;

			fprintf(fp_out, "Minimum Number of Trips = %d: ", min_num_trips);

			for (i=0; i < min_num_trips-1; i++){
				fprintf(fp_out, "  %d", min_max_capacity-1);
			}

			if(min_num_trips_remainder > 0) fprintf(fp_out, "  %d", min_num_trips_remainder);
			if(min_num_trips_remainder == 0) fprintf(fp_out, "  %d", min_max_capacity-1);


			//print best route
			fprintf(fp_out,"\n");
			fprintf(fp_out,"Route = %d", start_city);
			int j;
			for(j=0; j < best_route_counter; j++){
				fprintf(fp_out,"  %d", best_route_array[j]);
			}
			fprintf(fp_out,"\n");
		} else{
			fprintf(fp_out,"No Path Found for start vertex %d and destination vertex %d\n", start_city, destination_city);
		}

		//reset num_elements
		num_elements = 0;
		best_route_counter = 0;
		scenario += 1; //increment scenario number
		
		//nextline formatter
		fprintf(fp_out, "\n");
   }

   fclose(fp_in);
   fclose(fp_out);

}
