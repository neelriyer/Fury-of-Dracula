////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// game_view.c: GameView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>
#include <stdio.h>

#include "game.h"
#include "game_view.h"


//Assumed max round is 40
#define MAX_ROUNDS 40

// #include "map.h" ... if you decide to use the Map ADT
typedef struct moves *Move;

struct moves {

	char location[2];
	Move next;
	Move prev;
	int trap_count;
	int dracula_encounter;

};

typedef struct game_view {

	int round;

	Move G_tail;
	Move G_head;
	int G_turns;

	Move S_tail;
	Move S_head;
	int S_turns;

	Move H_tail;
	Move H_head;
    int H_turns;

	Move M_tail;
	Move M_head;
    int M_turns;

	Move D_tail;
	Move D_head;
    int D_turns;

} game_view;


//helper1
static Move new_node() {

	Move new = malloc (sizeof(struct moves));
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");
	new->next = NULL;
	new->prev = NULL;
	return new;

}

//helper2
static game_view *new_node_game_view() {

	game_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");

	return new;

}


//0        1        2          3     4                  5                 6  7       8
//[player][location][location][trap][immature vampire][dracula_encounter][.]["space"][player]etc.
game_view *gv_new (char *past_plays, player_message messages[])
{

	//create new node
	game_view *new = new_node_game_view();

	//Initialise
	Move G_prev = NULL;
    Move S_prev = NULL;
    Move H_prev = NULL;
    Move M_prev = NULL;
    Move D_prev = NULL;

	new->G_head = NULL;
	new->S_head = NULL;
    new->H_head = NULL;
    new->M_head = NULL;
    new->D_head = NULL;

    new->G_tail = NULL;
	new->S_tail = NULL;
    new->H_tail = NULL;
    new->M_tail = NULL;
    new->D_tail = NULL;

	new->G_turns = 0;
    new->S_turns = 0;
    new->H_turns = 0;
    new->M_turns = 0;
    new->D_turns = 0;

	printf("here\n");

	for(int i = 0;i<(MAX_ROUNDS-1) && past_plays[i]!='\0';i = i+8) {

		//Increment round
		if(i%MAX_ROUNDS==0) new->round++;

		//If Dr. Seward
		if(past_plays[i]=="S") {

			printf("here\n");
			
			//increment turns
			new->S_turns++;
	
			//create new node
			Move node_next = new_node();
	
			//new node location is past_plays[i+1] and past_plays[i+2]
			node_next->location[0] = past_plays[i+1];
			node_next->location[0] = past_plays[i+2];

			//score
			

			//health
			

			//checking 
			printf("location is %c%c\n",past_plays[i+1],past_plays[i+2]);
			
			//if empty
			if (new->S_tail == NULL) { 
				new->S_tail = node_next;
			} 

			//not empty
			else { 
				new->S_tail->next = node_next;
				new->S_tail = node_next;
			}

			//Fix previous
			node_next->prev = S_prev;
			S_prev = node_next;

		}
				
	}

	return new;
}

void gv_drop (game_view *gv)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free (gv);
}

round_t gv_get_round (game_view *gv)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

enum player gv_get_player (game_view *gv)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int gv_get_score (game_view *gv)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int gv_get_health (game_view *gv, enum player player)
{

	//Find player
	Move node = NULL;
	if(player==PLAYER_LORD_GODALMING) node = gv->G_head;
	if(player==PLAYER_DR_SEWARD) node = gv->S_head;
	if(player==PLAYER_VAN_HELSING) node = gv->H_head;
	if(player==PLAYER_MINA_HARKER) node = gv->M_head;
	if(player==PLAYER_DRACULA) node = gv->D_head;

	//continue this

	

	return 0;
}

location_t gv_get_location (game_view *gv, enum player player)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

void gv_get_history (
	game_view *gv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

location_t *gv_get_connections (
	game_view *gv, size_t *n_locations,
	location_t from, enum player player, round_t round,
	bool road, bool rail, bool sea)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*n_locations = 0;
	return NULL;
}
