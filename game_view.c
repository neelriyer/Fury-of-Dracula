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
#include <string.h>

#include "game.h"
#include "game_view.h"


//Assumed max round is 40
#define MAX_ROUNDS 40

#define __unused __attribute__((unused))

// #include "map.h" ... if you decide to use the Map ADT
typedef struct moves *Move;

struct moves {

	char location[2];
	Move next;
	Move prev;
	int trap;
	int dracula_encounter;
	int immature_vampire;
	int mature_vampire;
	int rest;
	int sea;
	int castle;

};

typedef struct game_view {

	int round;
	int score;

	Move G_tail;
	Move G_head;
	int G_turns;
	int G_health;

	Move S_tail;
	Move S_head;
	int S_turns;
	int S_health;

	Move H_tail;
	Move H_head;
    int H_turns;
	int H_health;

	Move M_tail;
	Move M_head;
    int M_turns;
	int M_health;

	Move D_tail;
	Move D_head;
    int D_turns;
	int D_health;

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
game_view *gv_new (char *past_plays, player_message messages[] )
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

	new->round = 0;

	new->G_health = GAME_START_HUNTER_LIFE_POINTS;
	new->S_health = GAME_START_HUNTER_LIFE_POINTS;
	new->H_health = GAME_START_HUNTER_LIFE_POINTS;
	new->M_health = GAME_START_HUNTER_LIFE_POINTS;
	new->D_health = GAME_START_BLOOD_POINTS;

	new->score = GAME_START_SCORE;
	
	for(int i = 0;i<(MAX_ROUNDS) && past_plays[i]!='\0';i = i+8) {

		//Increment round
		if(i%MAX_ROUNDS==0) new->round++;
		//TODO: fix new->found (this^ isn't going to work)

		//If G
		printf("i = %d\n",i);
		printf("past_plays[i] = %c\n",past_plays[i]);
		printf("strcmp(&past_plays[i],G) = %d\n",strcmp(&past_plays[i],"G"));
		if(strcmp(&past_plays[i],"G")==0) {
			//create new node
			Move node_next = new_node();
			printf("here\n");
			
			//new node location is past_plays[i+1] and past_plays[i+2]
			node_next->location[0] = past_plays[i+1];
			node_next->location[1] = past_plays[i+2];

			//checking 
			printf("location is %s\n",node_next->location);

			//trap
			if(strcmp(&past_plays[i+3],"T")==0) {
				node_next->trap=1;
				new->G_health = new->G_health - 2;
			}

			//immature vampire
			if(strcmp(&past_plays[i+4],"V")==0) {
				node_next->immature_vampire=1;
			}

			//dracula encounter
			if(strcmp(&past_plays[i+5],"D")==0) {
				node_next->dracula_encounter=1;
				new->G_health = new->G_health - 4;
			}

			//rest
			if(G_prev!=NULL && strcmp(G_prev->location,node_next->location)==0) {
				node_next->rest=1;
				new->G_health = new->G_health + 3;
			}

			//increment turn count
			new->G_turns++;
			
			//Fix links
			//if empty
			if (new->G_tail == NULL) { 
				new->G_tail = node_next;
			} 

			//not empty
			else { 
				new->G_tail->next = node_next;
				new->G_tail = node_next;
			}

			//Fix previous
			node_next->prev = G_prev;
			G_prev = node_next;

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
	return gv->round;
}

enum player gv_get_player (game_view *gv __unused)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int gv_get_score (game_view *gv)
{
	return gv->score;
}

int gv_get_health (game_view *gv, enum player player)
{

	//Find player
	int health=0;

	if(player==PLAYER_LORD_GODALMING) health = gv->G_health;
	if(player==PLAYER_DR_SEWARD) health = gv->S_health;
	if(player==PLAYER_VAN_HELSING) health = gv->H_health;
	if(player==PLAYER_MINA_HARKER) health = gv->M_health;
	if(player==PLAYER_DRACULA) health = gv->D_health;

	return health;
}

location_t gv_get_location (game_view *gv, enum player player)
{

	//if null
	if (gv==NULL) return 0;

	//find player 
	if(player==PLAYER_LORD_GODALMING) {
		if(gv->G_tail==NULL) return UNKNOWN_LOCATION;
		else return location_find_by_abbrev(gv->G_tail->location);
	}

	return 0;
}

void gv_get_history (
	game_view *gv __unused, enum player player __unused,
	location_t trail[TRAIL_SIZE] __unused)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

location_t *gv_get_connections (
	game_view *gv __unused, size_t *n_locations,
	location_t from __unused, enum player player __unused, round_t round __unused,
	bool road __unused, bool rail __unused, bool sea __unused)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*n_locations = 0;
	return NULL;
}
