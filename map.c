////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// map.c: an implementation of a Map type
// You can change this as much as you want!
//
// 2017-11-30	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "map.h"
#include "places.h"
#include "game.h"

//helpers
size_t connections (Map, int front, int *arr, int round, bool road, bool rail, bool sea, int player);
size_t dracula_connections (Map, int front, int *arr, bool road, bool sea, int *trail);

typedef struct map_adj map_adj;
typedef struct map {
	size_t n_vertices, n_edges;

	struct map_adj {
		location_t v;  // ALICANTE, etc
		transport_t type; // ROAD, RAIL, BOAT
		struct map_adj *next; // link to next node
	} *connections[NUM_MAP_LOCATIONS]; // array of lists
} map;


static void add_connections (map *);
static void add_connection (map *, location_t, location_t, transport_t);
static inline bool is_sentinel_edge (connection);

static map_adj *adjlist_insert (map_adj *, location_t, transport_t);
static bool adjlist_contains (map_adj *, location_t, transport_t);

// Create a new empty graph (for a map)
// #Vertices always same as NUM_PLACES
map *map_new (void)
{
	map *g = malloc (sizeof *g);
	if (g == NULL) err (EX_OSERR, "couldn't allocate Map");

	(*g) = (map) {
		.n_vertices = NUM_MAP_LOCATIONS,
		.n_edges = 0,
		.connections = { NULL }
	};

	add_connections (g);
	return g;
}

// Remove an existing graph
void map_drop (map *g)
{
	assert (g != NULL);

	for (size_t i = 0; i < g->n_vertices; i++) {
		map_adj *curr = g->connections[i];
		while (curr != NULL) {
			map_adj *next = curr->next;
			free (curr);
			curr = next;
		}
	}
	free (g);
}

// Display content of Map/Graph
void map_show (map *g)
{
	assert (g != NULL);

	//printf ("V=%zu, E=%zu\n", g->n_vertices, g->n_edges);
	for (size_t i = 0; i < g->n_vertices; i++) {
		for (map_adj *curr = g->connections[i]; curr != NULL; curr = curr->next) {
			//printf ("%s connects to %s by %s\n", location_get_name ((location_t) i), location_get_name (curr->v),transport_to_s (curr->type));
		}
	}
}

//helper
//is variable part of array?
static int is_part_of_array(int variable, int *arr, size_t up_to) {

	for(size_t i = 0;i<up_to;i++) {
		if(variable==arr[i]) return 1;
	}

	return 0;
}

//helper
//return n_connections from dracula's perspective
size_t dracula_connections (map *g, int front, int *arr, bool road, bool sea, int *trail)
{
	assert (g != NULL);

	size_t n_connections=0;

	//if unknown location return nothing
	if(valid_location_p(front)==0) return n_connections;

	//printf("front = %s\n", location_get_abbrev(front));

	//if on land and HIDE is not part of trail
	//HIDE is an option
	if(land_p(front) && is_part_of_array(HIDE,trail,TRAIL_SIZE-1)!=1) {

		//add current city to array
		arr[n_connections] = front;
		n_connections++;
	}

	//find all other connections and store in arr
	for (map_adj *curr = g->connections[front]; curr != NULL; curr = curr->next) {

		//printf ("%s connects to %s by %s\n", location_get_name ((location_t) front), location_get_name (curr->v), transport_to_s (curr->type));

		//skip st joseph and marys
		if(curr->v==ST_JOSEPH_AND_ST_MARYS) continue;

		//if road is allowed
		if(curr->type==ROAD && road) {
	
			//add connections that are: 
			//(1) not already part of array 
			//(2) not in trail
			if(is_part_of_array(curr->v,arr,n_connections)!=1 && 
				is_part_of_array(curr->v,trail,TRAIL_SIZE-1)!=1) {
				//printf("adding %s\n", location_get_abbrev(curr->v));
				arr[n_connections] = curr->v;
				n_connections++;
			}
		}

		//if sea is allowed
		if(curr->type==BOAT && sea) {

			//add connections that are: 
			//(1) not already part of array 
			//(2) not in trail
			if(is_part_of_array(curr->v,arr,n_connections)!=1 &&
				is_part_of_array(curr->v,trail,TRAIL_SIZE-1)!=1) {
				//printf("adding %s\n", location_get_abbrev(curr->v));
				arr[n_connections] = curr->v;
				n_connections++;
			}
		}
	}
	return n_connections;
}



//helper
//return n_connections from hunters perspective
size_t connections (map *g, int front, int *arr, int round, bool road, bool rail, bool sea, int player)
{
	assert (g != NULL);


	size_t n_connections=0;

	//if unknown location return nothing
	if(valid_location_p(front)==0) return n_connections;

	//add current city to array
	arr[n_connections] = front;
	n_connections++;

	//find all other connections and store in arr
	for (map_adj *curr = g->connections[front]; curr != NULL; curr = curr->next) {

		//printf ("%s connects to %s by %s\n", location_get_name ((location_t) front), location_get_name (curr->v), transport_to_s (curr->type));

		//If dracula skip st joseph and marys
		if(player==4 && curr->v==ST_JOSEPH_AND_ST_MARYS) continue;

		//if road is allowed
		if(curr->type==ROAD && road) {
	
			//add connections that are not already part of array
			if(is_part_of_array(curr->v,arr,n_connections)!=1) {
					arr[n_connections] = curr->v;
					n_connections++;
			}
			//else if not part of arr & dracula & not in trail
			//add to  arr
		}

		//if sea is allowed
		if(curr->type==BOAT && sea) {

			//add connections that are not already part of array
			if(is_part_of_array(curr->v,arr,n_connections)!=1) {
					arr[n_connections] = curr->v;
					n_connections++;
			}
		}

		//if rail is allowed (dracula cannot take rail)
		if(curr->type==RAIL && rail && player!=4) {

			//sum
			int sum = player + round;
			//printf("sum = %d\n", sum);

			//Move 1 rail hop
			if(sum%4==1) {

				//add connections that are not already part of array
				if(is_part_of_array(curr->v,arr,n_connections)!=1) {
					arr[n_connections] = curr->v;
					n_connections++;
				}
			}

			//Move 2 rail hops
			else if(sum%4==2) {

				//1st city (that are not already part of array)
				if(is_part_of_array(curr->v,arr,n_connections)!=1) {
					arr[n_connections] = curr->v;
					n_connections++;
				}

				//2nd city
				int second_city = curr->v;

				for (map_adj *second = g->connections[second_city]; second != NULL; second = second->next) {

					//rail connections only (that are not already part of array)
					if(second->type==RAIL && is_part_of_array(second->v,arr, n_connections)!=1) {
					//printf ("\nsecond\n%s connects to %s by %s\n", location_get_name ((location_t) second_city), location_get_name (second->v), transport_to_s (second->type));
	
					//add to array
					arr[n_connections] = second->v;
					n_connections++;
		
					}

				}

			}

			//Move 3 rail hops
			else if(sum%4==3) {

				//1st city, add connections that are not already part of array
				if(is_part_of_array(curr->v,arr,n_connections)!=1) {
					arr[n_connections] = curr->v;
					n_connections++;
				}

				//2nd city
				int second_city = curr->v;

				for (map_adj *second = g->connections[second_city]; second != NULL; second = second->next) {

					//rail connections only (that are not already part of array)
					if(second->type==RAIL && is_part_of_array(second->v,arr, n_connections)!=1) {
						//printf ("\nsecond\n%s connects to %s by %s\n", location_get_name ((location_t) second_city), location_get_name (second->v), transport_to_s (second->type));

						//add to array
						arr[n_connections] = second->v;
						n_connections++;


						//3rd city
						int third_city = second->v;

						for (map_adj *third = g->connections[third_city]; third != NULL; third = third->next) {

							//rail connections only (that are not already part of array)
							if(third->type==RAIL && is_part_of_array(third->v,arr, n_connections)!=1) {
								//printf ("\nthird\n%s connects to %s by %s\n", location_get_name ((location_t) third_city), location_get_name (third->v), transport_to_s (third->type));

								//add to array
								arr[n_connections] = third->v;
								n_connections++;

							}
						}
					}
				}
			}
		}	
	}

	//if Dracula cannot move, he teleports to castle dracula
	if(n_connections==0) {
		arr[0]=CASTLE_DRACULA;
		n_connections++;
	}

	return n_connections;
}


// Return count of nodes
size_t map_nv (map *g)
{
	assert (g != NULL);
	return g->n_vertices;
}

// Return count of edges of a particular type
size_t map_ne (map *g, transport_t type)
{
	assert (g != NULL);
	assert (valid_transport_p (type) || type == ANY);

	size_t n_edges = 0;

	for (size_t i = 0; i < g->n_vertices; i++)
		for (map_adj *curr = g->connections[i];
			 curr != NULL; curr = curr->next)
			if (curr->type == type || type == ANY)
				n_edges++;

	return n_edges;
}

/// Add edges to Graph representing map of Europe
static void add_connections (map *g)
{
	assert (g != NULL);

	for (size_t i = 0; ! is_sentinel_edge (CONNECTIONS[i]); i++)
		add_connection (
			g,
			CONNECTIONS[i].v, CONNECTIONS[i].w,
			CONNECTIONS[i].t
		);
}


/// Add a new edge to the Map/Graph
static void add_connection (
	map *g,
	location_t start,
	location_t end,
	transport_t type)
{
	assert (g != NULL);
	assert (start != end);
	assert (valid_location_p (start));
	assert (valid_location_p (end));
	assert (valid_transport_p (type));

	// don't add edges twice
	if (adjlist_contains (g->connections[start], end, type)) return;

	g->connections[start] = adjlist_insert (g->connections[start], end, type);
	g->connections[end]   = adjlist_insert (g->connections[end], start, type);
	g->n_edges++;
}

/// Is this the magic 'sentinel' edge?
static inline bool is_sentinel_edge (connection x)
{
	return x.v == -1 && x.w == -1 && x.t == ANY;
}

/// Insert a node into an adjacency list.
static map_adj *adjlist_insert (map_adj *list, location_t v, transport_t type)
{
	assert (valid_location_p (v));
	assert (valid_transport_p (type));

	map_adj *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate map_adj");
	(*new) = (map_adj){ .v = v, .type = type, .next = list };
	return new;
}

/// Does this adjacency list contain a particular value?
static bool adjlist_contains (map_adj *list, location_t v, transport_t type)
{
	assert (valid_location_p (v));
	assert (valid_transport_p (type));

	for (map_adj *curr = list; curr != NULL; curr = curr->next)
		if (curr->v == v && curr->type == type)
			return true;
	return false;
}
