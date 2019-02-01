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


void decide_dracula_move (DraculaView dv)
{
	location_t curr = dv_get_location(dv, 4);
	//printf("%d\n", curr);
/*
	if (curr != NOWHERE) {
		register_best_play (location_get_abbrev(curr), "Have we nothing Toulouse?");	
	} 	
*/		
	if (dv_get_round(dv) < 1) {
		//puts("first round");
		register_best_play ("CD", "Mwahahahaha");
	}
	
	else {
		char *next = location_get_abbrev (dv_get_next_move(dv, 4));		
	
		register_best_play (next, "Mwahahahaha");	
	}		
}



