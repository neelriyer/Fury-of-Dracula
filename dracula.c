////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include "dracula.h"
#include "dracula_view.h"
#include "game.h"

location_t get_next_move (DraculaView dv, enum player player);

void decide_dracula_move (DraculaView dv)
{
	char *next = location_get_abbrev (get_next_move(currentView, 3));		
	
	register_best_play (next, "Mwahahahaha");
}

location_t get_next_move (DraculaView currentView, enum player player) {
	size_t n_locations = 0;
	location_t *moves = dv_get_dests(currentView, n_locations, true, true);
	
	// if no legal place to go
	if (n_location = 0){
		// hide
		if (!dv_has_hide(currentView))
			return dv_get_location(currentView, 3);\
		// double back
		else if (!dv_has_double_back(currentView))
			return dv_double_back (currentView);
		
		else
			return NOWHERE;
	}
	
	// take the best move
	return dv_best_move_array (currentView, moves, n_location);
}

