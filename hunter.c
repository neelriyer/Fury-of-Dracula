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
}

location_t get_next_move (HunterView hv, enum player player) 
{
	// die
	if (hv_get_health(HunterView currentView, enum player player) <= 0)
		return ST_JOSEPH_AND_ST_MARYS;

	// take a move
	location_t dra = hv_find_dracula (currentView, enum player);
	if (dra == NOWHERE)
		return hv_move_random (currentView, player);
	else 
		return hv_move_dest (currentView, dra, player, true, true, true);
		
}

