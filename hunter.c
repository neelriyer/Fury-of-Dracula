////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include "game.h"
#include "hunter.h"
#include "hunter_view.h"


void decide_hunter_move (HunterView hv)	
{
	enum player player = hv_get_player(hv);
	location_t curr = hv_get_location(hv, player);
/*
	// already knew the player's location	
	if (curr != NOWHERE) {
		register_best_play (location_get_abbrev(curr), "Have we nothing Toulouse?");	
	} 
*/
	if (hv_get_round(hv) < 1) {
		puts("born in JM");
		register_best_play ("JM", "Have we nothing Toulouse?");	
	}
	
	else { 
		//puts("----------------------------else");
		char *next = location_get_abbrev (hv_get_next_move(hv, player));

		register_best_play (next, "Have we nothing Toulouse?");
	}

}




