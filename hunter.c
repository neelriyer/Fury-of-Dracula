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

location_t get_next_move (HunterView hv, enum player player);

void decide_hunter_move (HunterView hv)
{
	enum player player = hv_get_player(hv);
	
	char *next = location_get_abbrev (get_next_move(hv, player));

	register_best_play (next, "Have we nothing Toulouse?");
	//register_best_play ("TO", "Have we nothing Toulouse?");
}

location_t get_next_move (HunterView hv, enum player player) 
{
	// die
	if (hv_get_health(hv, player) <= 0)
		return ST_JOSEPH_AND_ST_MARYS;
	
	// take a move
	location_t dra = hv_find_dracula (hv, player);
	if (dra == NOWHERE){
		printf("------------%s\n", location_get_name (dra));
		return hv_move_random (hv, player);
	} else { 
		return hv_move_dest (hv, dra, player, true, true, true);
	}
		
}


