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

#include "map.h" //... if you decide to use the Map ADT

struct moves {

	char location[3];
	Move next;
	Move prev;

	int hunter_trap;
	int dracula_trap;

	int hunter_rest;

	int dracula_encounter;

	int immature_vampire;
	int mature_vampire;

};

typedef struct game_view {

	round_t round;
	int score;

	int traps[NUM_MAP_LOCATIONS]; //0 if no traps, 1 if 1 trap, 2 if 2 traps, 3 if 3 traps
	int vamps[NUM_MAP_LOCATIONS]; //0 if nothing, 1 if immature vamp, 2 if mature vamp

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

static Move dracula_setup(char *past_plays, int i, game_view *new);

//helper0
int trap_count(game_view *gv, int id) {
	if(valid_location_p(id)) return gv->traps[id];
	else return 0;
}

//if rounds are greater than 6
//go back 6 dracula_moves/rounds
//delete all traps from here until start

//if rounds are greater than 13
//go back 13 dracula_moves/rounds
//delete all immature vampires and create mature vampires
static void trap_vamp_matures(game_view *gv, int trap, int vamp) {
	
	//if trap
	if(trap) {

		//find the trap that has matured
		Move tail = gv->D_tail;
		
		if(gv->round>=(TRAIL_SIZE+1)) {

			//go beyond trail
			for (int i = 0;i<TRAIL_SIZE;i++) {
				tail = tail->prev;
			}

			//remove current trap and all before it
			while(tail!=NULL) {

				//get id
				int id = location_find_by_abbrev(tail->location);
	
				//remove all traps
				if(valid_location_p(id)) {
					gv->traps[id] = 0;
					printf("removed all traps at = %s\n", location_get_abbrev(id));
				}
	
				//previous node
				tail = tail->prev;
			}
		}
	}

	//if vamp
	if(vamp) {

		//find the vamp that has matured
		Move tail = gv->D_tail;
		
		if(gv->round>=(TRAIL_SIZE+1)) {

			//go beyond trail
			for (int i = 0;i<TRAIL_SIZE;i++) {
				tail = tail->prev;
			}

			//remove current trap and all before it
			while(tail!=NULL) {

				//get id
				int id = location_find_by_abbrev(tail->location);
	
				//mature vamp at
				if(valid_location_p(id) && gv->vamps[id]==1) {
					gv->vamps[id] = 2;
					printf("Vampire: %s is now mature\n", location_get_abbrev(id));
				}
	
				//previous node
				tail = tail->prev;
			}
		}
	}

}

//helper1
static Move new_node_move() {

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


//helper 3
static Move hunter_setup(char *past_plays, int i, game_view *new, enum player player)
{

	int health=0,turns=0;
	Move tail = NULL;
	
	//find player
	if(player==PLAYER_LORD_GODALMING) {
		tail = new->G_tail;
		health = new->G_health;
		turns = new->G_turns;
	}
	if(player==PLAYER_DR_SEWARD) {
		tail = new->S_tail;
		health = new->S_health;
		turns = new->S_turns;
	}
	if(player==PLAYER_VAN_HELSING) {
		tail = new->H_tail;
		health = new->H_health;
		turns = new->H_turns;
	}
	if(player==PLAYER_MINA_HARKER) {
		tail = new->M_tail;
		health = new->M_health;
		turns = new->M_turns;
	}

	//create node
	Move node_next = new_node_move();

			
	//new node location is past_plays[i+1] and past_plays[i+2]
	node_next->location[0] = past_plays[i+1];
	node_next->location[1] = past_plays[i+2];
	node_next->location[2] = '\0';

	//checking 
	printf("location is %s\n", node_next->location);
	printf("location ID is %d\n",location_find_by_abbrev(node_next->location));

	int id = location_find_by_abbrev(node_next->location);

	//hunter_trap
	if(past_plays[i+3]=='T' || past_plays[i+4]=='T' || past_plays[i+5]=='T' || past_plays[i+6]=='T') {
		printf("%u Hunter: Trap\n", player);
		node_next->hunter_trap=1;
		health = health - LIFE_LOSS_TRAP_ENCOUNTER;

		//detract from trap count
		if(valid_location_p(id)) {
			new->traps[id]--;
			printf("new->traps[id] = %d\nid = %d\n", new->traps[id], id);
		}
	}

	//immature vampire
	if(past_plays[i+3]=='V' || past_plays[i+4]=='V' || past_plays[i+5]=='V' || past_plays[i+6]=='V') {
		printf("%u Hunter: Immature Vampire\n", player);
		node_next->immature_vampire=1;

		//detract from immature vamp count
		if(valid_location_p(id)) {
			new->vamps[id]=0;
			printf("new->vamps[id] = %d\nid = %d\n", new->vamps[id], id);
		}
	}

	//dracula encounter
	if(past_plays[i+3]=='D' || past_plays[i+4]=='D' || past_plays[i+5]=='D' || past_plays[i+6]=='D') {
		node_next->dracula_encounter=1;
		printf("%u Hunter: Dracula Encounter\n", player);
		health = health - LIFE_LOSS_DRACULA_ENCOUNTER;
		new->D_health = new->D_health - LIFE_LOSS_HUNTER_ENCOUNTER;
	}

	//rest (only if hunter does not have full health)
	if(tail!=NULL && strcmp(tail->location,node_next->location)==0 && health!=GAME_START_HUNTER_LIFE_POINTS) {

		node_next->hunter_rest=1;
		printf("Hunter: Rest\n");

		if(health==(GAME_START_HUNTER_LIFE_POINTS-1)) health = health + 1;
		else if(health==(GAME_START_HUNTER_LIFE_POINTS-2)) health = health + 2;
		else if(health==(GAME_START_HUNTER_LIFE_POINTS-3)) health = health + 3;
		else health = health + LIFE_GAIN_REST;

	}

	//if health is less or equal to 0 
	if(health<=0) {
		printf("hunter died\n");
		//rest health
		health = GAME_START_HUNTER_LIFE_POINTS;

		//reduce score
		new->score = new->score - SCORE_LOSS_HUNTER_HOSPITAL;

		//sent to hospital
		node_next->location[0] = 'J';
		node_next->location[1] = 'M';
		node_next->location[2] = '\0';
		
	}

	//increment turn count
	turns++;

	//find player. input health and turns.
	if(player==PLAYER_LORD_GODALMING) {
		new->G_health = health; 
		new->G_turns = turns;
		printf("new->G_health = %d\n",new->G_health);
		printf("new->G_turns = %d\n",new->G_turns);
	}
	if(player==PLAYER_DR_SEWARD) {
		new->S_health = health;
		new->S_turns = turns;
		printf("new->S_health = %d\n",new->S_health);
		printf("new->S_turns = %d\n",new->S_turns);
	}
	if(player==PLAYER_VAN_HELSING) {
		new->H_health = health;
		new->H_turns = turns;
		printf("new->H_health = %d\n",new->H_health);
		printf("new->H_turns = %d\n",new->H_turns);
	}
	if(player==PLAYER_MINA_HARKER) {
		new->M_health = health;
		new->M_turns = turns;
		printf("new->M_health = %d\n",new->M_health);
		printf("new->M_turns = %d\n",new->M_turns);
	}
	printf("new->score = %d\n", new->score);
	printf("\n\n");
	
	return node_next;
}

//helper 4
//change dracula's health based on where he is
static void dracula_location_blood(const char *location, int id, game_view *new) {

	//if location is SEA_UNKNOWN
	if(strcmp(location,"S?")==0 || id == SEA_UNKNOWN) {
		printf("Dracula: Life lost at sea\n");
		new->D_health = new->D_health - LIFE_LOSS_SEA;
	}

	//if at castle gain 10 pts
	if(strcmp(location,"CD")==0 || id == CASTLE_DRACULA || strcmp(location,"TP")==0 || id == CASTLE_DRACULA) {
		printf("Dracula: Life gained at castle\n");
		new->D_health = new->D_health + LIFE_GAIN_CASTLE_DRACULA;
	}

	//if in sea
	if(valid_location_p(id) && sea_p(id)) {
		printf("Dracula: Life lost at sea\n");
		new->D_health = new->D_health - LIFE_LOSS_SEA;
	}
}


//helper 5
static Move dracula_setup(char *past_plays, int i, game_view *new) {

	//create node
	Move node_next = new_node_move();

	//new node location is past_plays[i+1] and past_plays[i+2]
	node_next->location[0] = past_plays[i+1];
	node_next->location[1] = past_plays[i+2];
	node_next->location[2] = '\0';

	//checking
	printf("location is %s\n",node_next->location);

	//get id
	int id = location_find_by_abbrev(node_next->location);
	//printf("id = %d\nCASTLE_DRACULA = %d\n", id, CASTLE_DRACULA);

	//dracula_trap
	if(past_plays[i+3]=='T') {
		printf("Dracula: Trap\n");
		node_next->dracula_trap=1;

		//add to trap count
		if(valid_location_p(id)) {
			new->traps[id]++;
			printf("new->traps[id] = %d\nid = %d\n", new->traps[id], id);
		}
	}
	
	//immature vampire
	if(past_plays[i+4]=='V') {
		printf("Dracula: Immature Vampire\n");
		node_next->immature_vampire=1;

		//add to immature vamp count
		if(valid_location_p(id)) {
			new->vamps[id]=1;
			printf("new->vamps[id] = %d\nid = %d\n", new->vamps[id], id);
		}
	}

	//trap matures
	if(past_plays[i+5]=='M') {
		printf("Dracula: Trap matured\n");
		node_next->dracula_trap=1;
		trap_vamp_matures(new, 1,0);
		
	}

	//vampire matures
	if(past_plays[i+5]=='V') {
		printf("Dracula: Vampire matures\n");
		node_next->mature_vampire=1;
		new->score = new->score - SCORE_LOSS_VAMPIRE_MATURES;
		trap_vamp_matures(new, 0,1);
	}
	
	//Double back moves
	if(node_next->location[0]=='D') {

		//get history
		location_t history[TRAIL_SIZE];
		gv_get_history (new, PLAYER_DRACULA, history);

		//TODO fix this, double back doesn't work
		for (int j = 0;j < 5;j++) {
			
			//number dracula doubles back by (convert from char to int)
			int double_back_number = node_next->location[1]-'0'-1;
			
			//printf("double_back_number-1 = %d\n", double_back_number-1);
			//printf("j = %d\n", j);

			if(double_back_number==j)  {
				printf("\nDracula: Double Back %d\n",j+1);

				printf("Doubling back to %s\n", location_get_abbrev(history[j]));

				//change dracula's health based on location
				dracula_location_blood(location_get_abbrev(history[j]), history[j], new); 
			}
		}
	}

	//change dracula's health based on location
	dracula_location_blood(node_next->location, id, new);

	//reduce score
	new->score = new->score - SCORE_LOSS_DRACULA_TURN;
	printf("new->score = %d\n", new->score);
	
	//increase turns
	new->D_turns++;

	printf("new->D_health = %d\n",new->D_health);
	printf("new->D_turns = %d\n\n\n",new->D_turns);
	
	return node_next;

} 


//0        1        2          3     4                  5                 6  7       8
//[player][location][location][trap][immature vampire][dracula_encounter][.]["space"][player]etc.
game_view *gv_new (char *past_plays, player_message messages[] __unused)
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

	for(int q = 0;q<NUM_MAP_LOCATIONS;q++) new->traps[q] = 0;

	for(int q = 0;q<NUM_MAP_LOCATIONS;q++) new->vamps[q] = 0;
	
	//while less than string length of trail + 1
	for(int i = 0;i<(strlen(past_plays)+1);i = i+8) {

		printf("\n--------ROUND %d--------\n", new->round);

		//If Lord Godalming
		if(past_plays[i]=='G') {

			printf("Lord Godalming\n");

			//create new node
			Move node_next = hunter_setup(past_plays, i, new, PLAYER_LORD_GODALMING);

			//Fix links
			if (new->G_tail == NULL) {
				new->G_head = node_next; 
				new->G_tail = node_next;
			} 
			else { 
				new->G_tail->next = node_next;
				new->G_tail = node_next;
			}
			node_next->prev = G_prev;
			G_prev = node_next;

		}

		//If Dr. Seward
		if(past_plays[i]=='S') {

			printf("Dr. Seward\n");

			//create new node
			Move node_next = hunter_setup(past_plays, i, new, PLAYER_DR_SEWARD);

			//Fix links
			if (new->S_tail == NULL) { 
				new->S_head = node_next;
				new->S_tail = node_next;
			} 
			else { 
				new->S_tail->next = node_next;
				new->S_tail = node_next;
			}
			node_next->prev = S_prev;
			S_prev = node_next;
		}

		//If Van Helsing
		if(past_plays[i]=='H') {

			printf("Van Helsing\n");

			//create new node
			Move node_next = hunter_setup(past_plays, i, new, PLAYER_VAN_HELSING);

			//Fix links
			if (new->H_tail == NULL) { 
				new->H_head = node_next;
				new->H_tail = node_next;
			} 
			else { 
				new->H_tail->next = node_next;
				new->H_tail = node_next;
			}
			node_next->prev = H_prev;
			H_prev = node_next;
		}

		//If Mina Harker
		if(past_plays[i]=='M') {

			printf("Mina Harker\n");

			//create new node
			Move node_next = hunter_setup(past_plays, i, new, PLAYER_MINA_HARKER);

			//Fix links
			if (new->M_tail == NULL) { 
				new->M_head = node_next;
				new->M_tail = node_next;
			} 
			else { 
				new->M_tail->next = node_next;
				new->M_tail = node_next;
			}
			node_next->prev = M_prev;
			M_prev = node_next;
		}

		//If Dracula
		if(past_plays[i]=='D') {

			printf("Dracula\n");

			//create new node
			Move node_next = dracula_setup(past_plays, i, new);

			//Fix links
			if (new->D_tail == NULL) { 
				new->D_head = node_next;
				new->D_tail = node_next;
			} 
			else { 
				new->D_tail->next = node_next;
				new->D_tail = node_next;
			}
			node_next->prev = D_prev;
			D_prev = node_next;
		}

		//Increment round
		if((i+8)%MAX_ROUNDS==0 && i!=0) new->round++; 

	}

	return new;

}

//helper
static void delete_list(Move node) {

	Move temp;
	while (node != NULL) {
		temp = node->next;
		free(node);
		node = temp;
	}

}

//drop function
void gv_drop (game_view *gv)
{

	delete_list(gv->G_head);
	delete_list(gv->S_head);
	delete_list(gv->H_head);
	delete_list(gv->M_head);
	delete_list(gv->D_head);

}

round_t gv_get_round (game_view *gv)
{
	return gv->round;
}

//whose turn is it?
enum player gv_get_player (game_view *gv)
{
	
	//G goes first, S goes second, etc.
	int first = gv->G_turns;
	int second = gv->S_turns;
	int third = gv->H_turns;
	int fourth = gv->M_turns;
	int fifth = gv->D_turns;

	if(first>second) return PLAYER_DR_SEWARD;
	else if(second>third) return PLAYER_VAN_HELSING;
	else if(third>fourth) return PLAYER_MINA_HARKER;
	else if(fourth>fifth) return PLAYER_DRACULA;
	else return PLAYER_LORD_GODALMING;
	
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
	
	if(player==PLAYER_DR_SEWARD) {
		if(gv->S_tail==NULL) return UNKNOWN_LOCATION;
		else return location_find_by_abbrev(gv->S_tail->location);
	}

	if(player==PLAYER_VAN_HELSING) {
		if(gv->H_tail==NULL) return UNKNOWN_LOCATION;
		else return location_find_by_abbrev(gv->H_tail->location);
	}

	if(player==PLAYER_MINA_HARKER) {
		if(gv->M_tail==NULL) return UNKNOWN_LOCATION;
		else return location_find_by_abbrev(gv->M_tail->location);
	}

	if(player==PLAYER_DRACULA) {
		if(gv->D_tail==NULL) return UNKNOWN_LOCATION;
		else if(gv->D_tail->location[0]=='C' && gv->D_tail->location[1]=='?') return CITY_UNKNOWN;
		else if(gv->D_tail->location[0]=='S' && gv->D_tail->location[1]=='?') return SEA_UNKNOWN;
		else if(gv->D_tail->location[0]=='H' && gv->D_tail->location[1]=='I') return HIDE;
		else if(gv->D_tail->location[0]=='D' && gv->D_tail->location[1]=='1') return DOUBLE_BACK_1;
		else if(gv->D_tail->location[0]=='D' && gv->D_tail->location[1]=='2') return DOUBLE_BACK_2;
		else if(gv->D_tail->location[0]=='D' && gv->D_tail->location[1]=='3') return DOUBLE_BACK_3;
		else if(gv->D_tail->location[0]=='D' && gv->D_tail->location[1]=='4') return DOUBLE_BACK_4;
		else if(gv->D_tail->location[0]=='D' && gv->D_tail->location[1]=='5') return DOUBLE_BACK_5;
		else if(gv->D_tail->location[0]=='T' && gv->D_tail->location[1]=='P') return TELEPORT;
		else return location_find_by_abbrev(gv->D_tail->location);
	}

	return 0;
}

void gv_get_history (

	game_view *gv, enum player player,
	location_t trail[TRAIL_SIZE])
{

	//Find player
	Move tail = NULL;
	if(player==PLAYER_LORD_GODALMING) tail = gv->G_tail;
	if(player==PLAYER_DR_SEWARD) tail = gv->S_tail;
	if(player==PLAYER_VAN_HELSING) tail = gv->H_tail;
	if(player==PLAYER_MINA_HARKER) tail = gv->M_tail;
	if(player==PLAYER_DRACULA) tail = gv->D_tail;


	for (int i = 0;i < 6;i++) {

		//if tail is null
		if(tail==NULL) {
			trail[i] = UNKNOWN_LOCATION;
			return;
		}

		//if CITY_UNKNOWN
		else if(tail->location[0]=='C' && tail->location[1]=='?') trail[i] = CITY_UNKNOWN;

		//if SEA_UNKNOWN
		else if(tail->location[0]=='S' && tail->location[1]=='?') trail[i] =  SEA_UNKNOWN;

		//if HIDE
		else if(tail->location[0]=='H' && tail->location[1]=='I') trail[i] = HIDE;

		//if double back 
		else if(tail->location[0]=='D' && tail->location[1]=='1') trail[i] =  DOUBLE_BACK_1;
		else if(tail->location[0]=='D' && tail->location[1]=='2') trail[i] =  DOUBLE_BACK_2;
		else if(tail->location[0]=='D' && tail->location[1]=='3') trail[i] =  DOUBLE_BACK_3;
		else if(tail->location[0]=='D' && tail->location[1]=='4') trail[i] =  DOUBLE_BACK_4;
		else if(tail->location[0]=='D' && tail->location[1]=='5') trail[i] =  DOUBLE_BACK_5;

		//If Teleport
		else if(tail->location[0]=='T' && tail->location[1]=='P') trail[i] =  TELEPORT;

		//Otherwise
		else {
			printf("tail->location = %s\n", tail->location);
			trail[i] = location_find_by_abbrev(tail->location);
		}
		
		//previous node
		tail = tail->prev;
	}
		
}


location_t *gv_get_connections (
	game_view *gv, size_t *n_locations,
	location_t from, enum player player, round_t round,
	bool road, bool rail, bool sea)
{
	//NOTE: very similar to lab04 exercise

	//create map
	Map europe = map_new();

	//create array
	int *arr = malloc(NUM_MAP_LOCATIONS * sizeof *arr);

	//find number of connections
	size_t n_connections = connections(europe, from, arr, round, road, rail, sea, player); 

	//copy number of connections into n_locations
	*n_locations = n_connections;

	map_drop(europe);

	return arr;
}

