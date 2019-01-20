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
#include "map.h" 

//Assumed max characters in round (5*8 = 40)
#define MAX_CHARACTERS 40

#define __unused __attribute__((unused))

//helpers
int trap_count(GameView gv, int id);
int vamp_count(GameView gv, int id);
int previous_move(GameView gv, enum player player, int back);
int previous_move(GameView gv, enum player player, int back);
void dracula_trail_fixer (GameView gv, int *trail, enum player player);
location_t hunter_dracula_location (GameView gv, enum player player);
size_t connections (Map, int front, int *arr, int round, bool road, bool rail, bool sea, int player);
size_t dracula_connections (Map, int front, int *arr, bool road, bool sea, int *trail);


typedef struct moves *Move;
struct moves {

	char location[3];
	Move next;
	Move prev;

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

//helper1
//return number of traps at location = id
int trap_count(game_view *gv, int id) {
	if(valid_location_p(id)) return gv->traps[id];
	else return 0;
}

//helper2
//return number of immature/mature vamps at location = id
int vamp_count(game_view *gv, int id) {
	if(valid_location_p(id)) return gv->vamps[id];
	else return 0;
}

//helper3
//find previous move and return its location id
int previous_move(game_view *gv, enum player player, int back) {


	Move tail = NULL; 
	if(player==PLAYER_LORD_GODALMING) tail = gv->G_tail;
	if(player==PLAYER_DR_SEWARD) tail = gv->S_tail;
	if(player==PLAYER_VAN_HELSING) tail = gv->H_tail;
	if(player==PLAYER_MINA_HARKER) tail = gv->M_tail;
	if(player==PLAYER_DRACULA) tail = gv->D_tail;

	for(int i = 0;i < back;i++) {
		//printf("i = %d\n", i);
		if(tail==NULL) return UNKNOWN_LOCATION;
		//printf("previous tail->location = %s\n", tail->location);
		tail = tail->prev;
	}

	//printf("final previous tail->location = %s\n", tail->location);

	//if invalid location
	if(tail==NULL) return UNKNOWN_LOCATION;
	else if(tail->location[0]=='C' && tail->location[1]=='?') return CITY_UNKNOWN;
	else if(tail->location[0]=='S' && tail->location[1]=='?') return SEA_UNKNOWN;
	else if(tail->location[0]=='H' && tail->location[1]=='I') return HIDE;
	else if(tail->location[0]=='D' && tail->location[1]=='1') return DOUBLE_BACK_1;
	else if(tail->location[0]=='D' && tail->location[1]=='2') return DOUBLE_BACK_2;
	else if(tail->location[0]=='D' && tail->location[1]=='3') return DOUBLE_BACK_3;
	else if(tail->location[0]=='D' && tail->location[1]=='4') return DOUBLE_BACK_4;
	else if(tail->location[0]=='D' && tail->location[1]=='5') return DOUBLE_BACK_5;
	else if(tail->location[0]=='T' && tail->location[1]=='P') return TELEPORT;
		
	//if valid location
	else return location_find_by_abbrev(tail->location);

}

//helper4
//Handles traps/vamps maturing
static void trap_vamp_matures(game_view *gv, int trap, int vamp) {
	
	//if trap
	if(trap) {

		//find the trap that has matured
		Move tail = gv->D_tail;
		
		if(gv->round>=(TRAIL_SIZE+1)) {

			//go to node just before trail begins
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
					//printf("removed all traps at = %s\n", location_get_abbrev(id));
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

			//go to node just before trail begins
			for (int i = 0;i<TRAIL_SIZE;i++) {
				tail = tail->prev;
			}

			while(tail!=NULL) {

				//get id
				int id = location_find_by_abbrev(tail->location);
	
				//mature vamp at
				if(valid_location_p(id) && gv->vamps[id]==1) {
					gv->vamps[id] = 2;
					//printf("Vampire: %s is now mature\n", location_get_abbrev(id));
				}
	
				//previous node
				tail = tail->prev;
			}
		}
	}

}

//helper5
//create new node
static Move new_node_move() {

	Move new = malloc (sizeof(struct moves));
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");
	new->next = NULL;
	new->prev = NULL;
	return new;

}

//helper6
//create new node
static game_view *new_node_game_view() {

	game_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");
	return new;

}


//helper 7
//setup hunter
static Move hunter_setup(char *past_plays, int i, game_view *new, enum player player)
{

	int health = 0,turns = 0;
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

	//if hunter died previously reset health
	if(health == 0) health = GAME_START_HUNTER_LIFE_POINTS;

			
	//new node location is past_plays[i+1] and past_plays[i+2]
	node_next->location[0] = past_plays[i+1];
	node_next->location[1] = past_plays[i+2];
	node_next->location[2] = '\0';

	//checking 
	//printf("location is %s\n", node_next->location);
	//printf("location ID is %d\n",location_find_by_abbrev(node_next->location));

	int id = location_find_by_abbrev(node_next->location);

	//hunter_trap
	for(int j = i+3;j<(i+7);j++) {
		if(past_plays[j]=='T') {
			//printf("%u Hunter: Trap\n", player);
			health = health - LIFE_LOSS_TRAP_ENCOUNTER;

			//detract from trap count
			if(valid_location_p(id)) {
				new->traps[id]--;
				//printf("new->traps[id] = %d\nid = %d\n", new->traps[id], id);
			}
		}
	}

	//immature vampire
	if(past_plays[i+3]=='V' || past_plays[i+4]=='V' || past_plays[i+5]=='V' || past_plays[i+6]=='V') {
		//printf("%u Hunter: Immature Vampire\n", player);

		//detract from immature vamp count
		if(valid_location_p(id)) {
			new->vamps[id]=0;
			//printf("new->vamps[id] = %d\nid = %d\n", new->vamps[id], id);
		}
	}

	//dracula encounter
	if(past_plays[i+3]=='D' || past_plays[i+4]=='D' || past_plays[i+5]=='D' || past_plays[i+6]=='D') {
		
		//printf("%u Hunter: Dracula Encounter\n", player);
		health = health - LIFE_LOSS_DRACULA_ENCOUNTER;
		new->D_health = new->D_health - LIFE_LOSS_HUNTER_ENCOUNTER;
	}

	//rest (only if hunter does not have full health)
	if(tail!=NULL && strcmp(tail->location,node_next->location)==0 && health!=GAME_START_HUNTER_LIFE_POINTS) {

		
		//printf("Hunter: Rest\n");

		if(health==(GAME_START_HUNTER_LIFE_POINTS-1)) health = health + 1;
		else if(health==(GAME_START_HUNTER_LIFE_POINTS-2)) health = health + 2;
		else if(health==(GAME_START_HUNTER_LIFE_POINTS-3)) health = health + 3;
		else health = health + LIFE_GAIN_REST;

	}

	//if health is less or equal to 0 
	if(health<=0) {
		//printf("hunter died\n");

		//reset health
		health = 0;

		//reduce score
		new->score = new->score - SCORE_LOSS_HUNTER_HOSPITAL;

		//send to hospital (TODO: changed this and get_location health bit)
		//node_next->location[0] = 'J';
		//node_next->location[1] = 'M';
		//node_next->location[2] = '\0';
		
	}

	//increment turn count
	turns++;

	//find player. input health and turns.
	if(player==PLAYER_LORD_GODALMING) {
		new->G_health = health; 
		new->G_turns = turns;
		//printf("new->G_health = %d\n",new->G_health);
		//printf("new->G_turns = %d\n",new->G_turns);
	}
	if(player==PLAYER_DR_SEWARD) {
		new->S_health = health;
		new->S_turns = turns;
		//printf("new->S_health = %d\n",new->S_health);
		//printf("new->S_turns = %d\n",new->S_turns);
	}
	if(player==PLAYER_VAN_HELSING) {
		new->H_health = health;
		new->H_turns = turns;
		//printf("new->H_health = %d\n",new->H_health);
		//printf("new->H_turns = %d\n",new->H_turns);
	}
	if(player==PLAYER_MINA_HARKER) {
		new->M_health = health;
		new->M_turns = turns;
		//printf("new->M_health = %d\n",new->M_health);
		//printf("new->M_turns = %d\n",new->M_turns);
	}
	//printf("new->score = %d\n", new->score);
	//printf("\n\n");
	
	return node_next;
}

//helper8
//Look at dracula's trail and only get real locations in trail remove HIDE, DOUBLE_BACK etc.
void dracula_trail_fixer (game_view *gv, int *trail, enum player player) {

	for (int i = 0;i<TRAIL_SIZE;i++) {

		//printf("trail[%d] = %s\n",i, location_get_abbrev(trail[i]));

		//if hide
		if(trail[i] == HIDE) {
			//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
			trail[i] = previous_move(gv, player, i+1);
			//printf("after: trail[%d] = %d\n\n",i, trail[i]);

			if(!valid_location_p(trail[i])) {

				//printf("HIDE IS INVALID\n");
				//printf("trail[i] = %d\n", trail[i]);

				//if teleport
				if(trail[i] == TELEPORT) {
				
					trail[i] = CASTLE_DRACULA;
					//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));
				}

				//if double back 1
				else if(trail[i] == DOUBLE_BACK_1) {
					trail[i] = previous_move(gv, player, i+2);
					//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));
				}

				//if double back 2
				else if(trail[i] == DOUBLE_BACK_2) {
					trail[i] = previous_move(gv, player, i+3);
					//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));
				}

				//if double back 3
				else if(trail[i] == DOUBLE_BACK_3) {
					trail[i] = previous_move(gv, player, i+4);
					//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));
				}

				//if double back 4
				else if(trail[i] == DOUBLE_BACK_4) {
					trail[i] = previous_move(gv, player, i+5);
					//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));
				}

				//if double back 5
				else if(trail[i] == DOUBLE_BACK_5) {
					trail[i] = previous_move(gv, player, i+6);
					//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));
				}				

			}
		}

		//if teleport
		else if(trail[i] == TELEPORT) {
			//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
			trail[i] = CASTLE_DRACULA;
			//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));
		}

		//if double back 1
		else if(trail[i] == DOUBLE_BACK_1) {
			//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
			trail[i] = previous_move(gv, player, i+1);
			//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));

			//invalid location is HIDE
			if(!valid_location_p(trail[i]) && trail[i] == HIDE) {
				//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
				trail[i] = previous_move(gv, player, i+2);
			}

		}

		//if double back 2
		else if(trail[i] == DOUBLE_BACK_2) {
			//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
			trail[i] = previous_move(gv, player, i+2);
			//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));
	
			//invalid location is HIDE
			if(!valid_location_p(trail[i]) && trail[i] == HIDE) {
				//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
				trail[i] = previous_move(gv, player, i+3);
			}
		}

		//if double back 3
		else if(trail[i] == DOUBLE_BACK_3) {
			//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
			trail[i] = previous_move(gv, player, i+3);
			//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));

			//invalid location is HIDE
			if(!valid_location_p(trail[i]) && trail[i] == HIDE) {
				//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
				trail[i] = previous_move(gv, player, i+4);
			}
		}

		//if double back 4
		else if(trail[i] == DOUBLE_BACK_4) {
			//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
			trail[i] = previous_move(gv, player, i+4);
			//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));

			//invalid location is HIDE
			if(!valid_location_p(trail[i]) && trail[i] == HIDE) {
				//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
				trail[i] = previous_move(gv, player, i+5);
			}
		}

		//if double back 5
		else if(trail[i] == DOUBLE_BACK_5) {
			//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
			trail[i] = previous_move(gv, player, i+5);
			//printf("after: trail[%d] = %s\n\n",i, location_get_abbrev(trail[i]));

			//invalid location is HIDE
			if(!valid_location_p(trail[i]) && trail[i] == HIDE) {
				//printf("\nbefore: trail[%d] = %s\n",i, location_get_abbrev(trail[i]));
				trail[i] = previous_move(gv, player, i+6);
			}
		}
	}
}

//helper 8
//change dracula's health based on where he is
static void dracula_location_blood(const char *location, int id, game_view *new) {

	//if location is SEA_UNKNOWN
	if(strcmp(location,"S?")==0 || id == SEA_UNKNOWN) {
		//printf("Dracula: Life lost at sea\n");
		new->D_health = new->D_health - LIFE_LOSS_SEA;
	}

	//if at castle gain 10 pts
	if(strcmp(location,"CD")==0 || id == CASTLE_DRACULA || strcmp(location,"TP")==0 || id == CASTLE_DRACULA) {
		//printf("Dracula: Life gained at castle\n");
		new->D_health = new->D_health + LIFE_GAIN_CASTLE_DRACULA;
	}

	//if in sea
	if(valid_location_p(id) && sea_p(id)) {
		//printf("Dracula: Life lost at sea\n");
		new->D_health = new->D_health - LIFE_LOSS_SEA;
	}
}


//helper 9
//dracula setup
static Move dracula_setup(char *past_plays, int i, game_view *new) {

	//create node
	Move node_next = new_node_move();

	//new node location is past_plays[i+1] and past_plays[i+2]
	node_next->location[0] = past_plays[i+1];
	node_next->location[1] = past_plays[i+2];
	node_next->location[2] = '\0';

	//checking
	//printf("location is %s\n",node_next->location);

	//get id
	int id = location_find_by_abbrev(node_next->location);
	//printf("id = %d\nCASTLE_DRACULA = %d\n", id, CASTLE_DRACULA);

	//dracula_trap
	if(past_plays[i+3]=='T') {
		//printf("Dracula: Trap\n");
		

		//add to trap count
		if(valid_location_p(id)) {
			new->traps[id]++;
			//printf("new->traps[id] = %d\nid = %d\n", new->traps[id], id);
		}
	}
	
	//immature vampire
	if(past_plays[i+4]=='V') {
		//printf("Dracula: Immature Vampire\n");
		

		//add to immature vamp count
		if(valid_location_p(id)) {
			new->vamps[id]=1;
			//printf("new->vamps[id] = %d\nid = %d\n", new->vamps[id], id);
		}
	}

	//trap matures
	if(past_plays[i+5]=='M') {
		//printf("Dracula: Trap matured\n");
		
		trap_vamp_matures(new, 1,0);
		
	}

	//vampire matures
	if(past_plays[i+5]=='V') {
		//printf("Dracula: Vampire matures\n");
		
		new->score = new->score - SCORE_LOSS_VAMPIRE_MATURES;
		trap_vamp_matures(new, 0,1);
	}
	
	//Double back moves
	if(node_next->location[0]=='D' && node_next->location[1] != 'U') {

		//get history
		location_t history[TRAIL_SIZE];
		gv_get_history (new, PLAYER_DRACULA, history);

		//TODO added this line below (check if it works)
		dracula_trail_fixer (new, history, PLAYER_DRACULA);

		for (int j = 0;j < 5;j++) {
			
			//number dracula doubles back by (convert from char to int)
			int double_back_number = node_next->location[1]-'0'-1;
			
			//printf("double_back_number-1 = %d\n", double_back_number-1);
			//printf("j = %d\n", j);

			if(double_back_number==j)  {
				//printf("\nDracula: Double Back %d\n",j+1);

				//printf("Doubling back to %s\n", location_get_abbrev(history[j]));

				//change dracula's health based on location
				dracula_location_blood(location_get_abbrev(history[j]), history[j], new); 
			}
		}
	}

	//if HIDE
	if(node_next->location[0]=='H' && node_next->location[1] == 'I') {

		//get history
		location_t history[TRAIL_SIZE];
		gv_get_history (new, PLAYER_DRACULA, history);

		//TODO added this line below (check if it works)
		dracula_trail_fixer (new, history, PLAYER_DRACULA);
		//printf("\n\nhistory[0] = %d\nCASTLE_DRACULA = %d\n", history[0], CASTLE_DRACULA);
		//change dracula's health based on location
		dracula_location_blood(location_get_abbrev(history[0]), history[0], new); 

	}

		


	//change dracula's health based on location
	dracula_location_blood(node_next->location, id, new);

	//reduce score
	new->score = new->score - SCORE_LOSS_DRACULA_TURN;
	//printf("new->score = %d\n", new->score);
	
	//increase turns
	new->D_turns++;

	//printf("new->D_health = %d\n",new->D_health);
	//printf("new->D_turns = %d\n\n\n",new->D_turns);
	
	return node_next;

} 


/**
 * Creates a new view to summarise the current state of the game.
 *
 * @param past_plays is a string of all plays made in the game so far by
 *    all players (including Dracula) from earliest to most recent.
 *
 * @param messages is an array containing a `player_message` for each
 *    play in the game so far.  It will have exactly the same number of
 *    elements as there are plays in `past_plays`.  The message from the
 *    first play will be at index 0, and so on.  The contents of each
 *    `player_message` will be exactly as provided by the player.
 *
 * The "player_message" type is defined in game.h.
 * You are free to ignore messages if you wish.
 */
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
	for(int i = 0;i<(int)(strlen(past_plays)+1);i = i+8) {

		//printf("\n--------ROUND %d--------\n", new->round);

		//If Lord Godalming
		if(past_plays[i]=='G') {

			//printf("Lord Godalming\n");

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

			//printf("Dr. Seward\n");

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

			//printf("Van Helsing\n");

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

			//printf("Mina Harker\n");

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

			//printf("Dracula\n");

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
		if((i+8)%MAX_CHARACTERS==0 && i!=0) new->round++; 

	}

	return new;

}

//helper11
//delete node
static void delete_list(Move node) {

	Move temp;
	while (node != NULL) {
		temp = node->next;
		free(node);
		node = temp;
	}

}

/**
 * Frees all resources allocated for `gv`.
 * After this has been called, `gv` should not be accessed.
 */
void gv_drop (game_view *gv)
{

	delete_list(gv->G_head);
	delete_list(gv->S_head);
	delete_list(gv->H_head);
	delete_list(gv->M_head);
	delete_list(gv->D_head);

	free(gv);

}

/**
 * Get the current round
 */
round_t gv_get_round (game_view *gv)
{
	return gv->round;
}

/**
 * Get the current player; effectively, whose turn is it?
 */
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

/**
 * Get the current score, a positive integer between 0 and 366.
 */
int gv_get_score (game_view *gv)
{
	return gv->score;
}

/**
 * Get the current health points for a given player.
 * @param player specifies which players's life/blood points to return;
 * @returns a value between 0..9 for Hunters, or >0 for Dracula
 */
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

//helper
//hunter's perception of where dracula is. 
//Makes deductions eg. if HIDE dracula is assumed to stay in the same location
location_t hunter_dracula_location (game_view *gv, enum player player) {

	//setup pointer
	Move tail = gv->D_tail;

	//get trail
	location_t trail[TRAIL_SIZE];
	gv_get_history (gv, player, trail);
	dracula_trail_fixer (gv, trail, player);

	for(int i = 0;i<TRAIL_SIZE;i++) //printf("trail[%d] = %s\n",i, location_get_abbrev(trail[i]));

	//if current location is valid
	if(valid_location_p(trail[0])) return trail[0];

	//if current location is invalid
	if(tail==NULL) return UNKNOWN_LOCATION;
	else if(tail->location[0]=='H' && tail->location[1]=='I') return HIDE;
	else if(tail->location[0]=='D' && tail->location[1]=='1') return DOUBLE_BACK_1;
	else if(tail->location[0]=='D' && tail->location[1]=='2') return DOUBLE_BACK_2;
	else if(tail->location[0]=='D' && tail->location[1]=='3') return DOUBLE_BACK_3;
	else if(tail->location[0]=='D' && tail->location[1]=='4') return DOUBLE_BACK_4;
	else if(tail->location[0]=='D' && tail->location[1]=='5') return DOUBLE_BACK_5;
	else if(tail->location[0]=='T' && tail->location[1]=='P') return CASTLE_DRACULA;
	else if(tail->location[0]=='C' && tail->location[1]=='?') return CITY_UNKNOWN;
	else if(tail->location[0]=='S' && tail->location[1]=='?') return SEA_UNKNOWN;
	else return location_find_by_abbrev(tail->location);

}

/**
 * Get the current location of a given player.
 *
 * May be `UNKNOWN_LOCATION` if the player has not had a turn yet
 * (i.e., at the beginning of the game when the round is 0)
 *
 * Possible values include:
 * - in the interval 0..70, if the player was (known to be) in a
 *   particular city or on a particular sea;
 * - `CITY_UNKNOWN`, if Dracula was known to be in a city;
 * - `SEA_UNKNOWN`, if Dracula was known to be at sea;
 * - `HIDE`, if Dracula was known to have made a hide move;
 * - `DOUBLE_BACK_n`, where n is [1...5], if Dracula was known to have
 *   made a double back move _n_ positions back in the trail; e.g.,
 *   `DOUBLE_BACK_1` is the last place place he visited; or
 * - `TELEPORT`, if Dracula apparated back to Castle Dracula.
 */
location_t gv_get_location (game_view *gv, enum player player)
{

	//if null
	if (gv==NULL) return 0;

	//if LORD_GODALMING 
	if(player==PLAYER_LORD_GODALMING) {
		//printf("gv_get_health (gv, PLAYER_LORD_GODALMING) = %d\n", gv_get_health (gv, PLAYER_LORD_GODALMING));
		if(gv->G_tail==NULL) return UNKNOWN_LOCATION;
		if(gv_get_health (gv, PLAYER_LORD_GODALMING) <= 0) return HOSPITAL_LOCATION;
		else return location_find_by_abbrev(gv->G_tail->location);
	}
	
	//if DR_SEWARD
	if(player==PLAYER_DR_SEWARD) {
		if(gv->S_tail==NULL) return UNKNOWN_LOCATION;
		if(gv_get_health (gv, PLAYER_DR_SEWARD) <= 0) return HOSPITAL_LOCATION;
		else return location_find_by_abbrev(gv->S_tail->location);
	}

	//if VAN_HELSING
	if(player==PLAYER_VAN_HELSING) {
		if(gv->H_tail==NULL) return UNKNOWN_LOCATION;
		if(gv_get_health (gv, PLAYER_VAN_HELSING) <= 0) return HOSPITAL_LOCATION;
		else return location_find_by_abbrev(gv->H_tail->location);
	}

	//if MINA_HARKER
	if(player==PLAYER_MINA_HARKER) {
		if(gv->M_tail==NULL) return UNKNOWN_LOCATION;
		if(gv_get_health (gv, PLAYER_VAN_HELSING) <= 0) return HOSPITAL_LOCATION;
		else return location_find_by_abbrev(gv->M_tail->location);
	}

	//if PLAYER_DRACULA 
	if(player==PLAYER_DRACULA) {

		//if invalid location
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
		
		//if valid location
		else return location_find_by_abbrev(gv->D_tail->location);
	}

	return 0;
}


/**
 * Fills the trail array with the locations of the last 6 turns for the
 * given player.
 *
 * If the move does not exist yet (i.e., the start of the game),
 * the value should be UNKNOWN_LOCATION (-1).
 *
 * For example after 2 turns the array may have the contents
 *
 *     {29, 12, -1, -1, -1, -1}
 *
 * This would mean in the first move the player started on location 12
 * then moved to the current location of 29.
 */
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


	for (int i = 0;i < TRAIL_SIZE;i++) {

		//if tail is null
		if(tail==NULL) {
			trail[i] = UNKNOWN_LOCATION;
			continue;
		}

		//if invalid location
		else if(tail->location[0]=='C' && tail->location[1]=='?') trail[i] = CITY_UNKNOWN;
		else if(tail->location[0]=='S' && tail->location[1]=='?') trail[i] =  SEA_UNKNOWN;
		else if(tail->location[0]=='H' && tail->location[1]=='I') trail[i] = HIDE;
		else if(tail->location[0]=='D' && tail->location[1]=='1') trail[i] =  DOUBLE_BACK_1;
		else if(tail->location[0]=='D' && tail->location[1]=='2') trail[i] =  DOUBLE_BACK_2;
		else if(tail->location[0]=='D' && tail->location[1]=='3') trail[i] =  DOUBLE_BACK_3;
		else if(tail->location[0]=='D' && tail->location[1]=='4') trail[i] =  DOUBLE_BACK_4;
		else if(tail->location[0]=='D' && tail->location[1]=='5') trail[i] =  DOUBLE_BACK_5;
		else if(tail->location[0]=='T' && tail->location[1]=='P') trail[i] =  TELEPORT;

		//Otherwise
		else {
			//printf("tail->location = %s\n", tail->location);
			trail[i] = location_find_by_abbrev(tail->location);
		}
		tail = tail->prev;
	}
		
}

/**
 * Return an array of `location_t`s giving all of the locations that the
 * given `player` could reach from their current location, assuming it's
 * currently `round`.
 *
 * The array can be in any order but must contain unique entries.
 * The array size is stored at the variable referenced by `n_locations`.
 * The player's current location should be included in the array.
 *
 * `road`, `rail`, and `sea` connections should only be considered
 * if the `road`, `rail`, `sea` parameters are true, respectively.
 *
 * The function must take into account the current round and player for
 * determining how far `player` can travel by rail.
 *
 * When `player` is `PLAYER_DRACULA`, the function must take into
 * account (many of) the rules around Dracula's movements, such as that
 * Dracula may not go to the hospital, and may not travel by rail.
 * It need not take into account the trail restriction.
 */
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

