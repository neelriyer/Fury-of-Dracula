////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_game_view.c: test the GameView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_view.h"

int main (void)
{

	do {////////////////////////////////////////////////////////////////
		puts ("Test for traps/dracula confrontation");

		char *trail =
			"GED.... SGE.... HZU.... MCA.... DCF.V.. " 
			"GMN.... SGE.D.. HGE.... MLS.... DBOT... " 
			"GLO.... SMR.... HGE.... MMA.... DTOT... "
			"GPL.... SMS.... HMR.... MGR.... DBAT... "
			"GLO.... SBATD.. HMS.... MMA.... DSRT... "
			"GPL.... SSJ.... HBA.... MGR.... DALT... "
			"GPL.... SSJ.... HBA.... MGR.... DMAT... "
			"GLO.... SBE.... HMS.... MMATD.. DMAT.M. "
			"GLO.... SBE.... HMS.... MMATD.. DMAT.V. ";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		//assert (gv_get_round (gv) == 3);
		//assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		//assert (gv_get_location (gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		//assert (gv_get_location (gv, PLAYER_VAN_HELSING) == ZURICH);
		//assert (gv_get_location (gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		//assert (gv_get_location (gv, PLAYER_DRACULA) == ENGLISH_CHANNEL);
		//assert (gv_get_health (gv, PLAYER_DRACULA) == 0);

		puts ("passed");
		gv_drop (gv);
	} while (0);

	//MY TESTS TODO add more
	do {////////////////////////////////////////////////////////////////
		puts ("Test for traps/dracula confrontation");

		char *trail =
			"GAO.... SAO.... HZU.... MBB.... DSTTV.. "
			"GAO.... SAO.... HZU.... MBB.... DECTVV. "
			"GAO.... SSTTV.. HZU.... MBB.... DEC..M. ";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 3);
		//assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		//assert (gv_get_location (gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		//assert (gv_get_location (gv, PLAYER_VAN_HELSING) == ZURICH);
		//assert (gv_get_location (gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		//assert (gv_get_location (gv, PLAYER_DRACULA) == ENGLISH_CHANNEL);
		//assert (gv_get_health (gv, PLAYER_DRACULA) == 0);

		puts ("passed");
		gv_drop (gv);
	} while (0);



	//STANDARD TESTS
	do {////////////////////////////////////////////////////////////////
		puts ("Test basic empty initialisation");

		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert (gv_get_score (gv) == GAME_START_SCORE);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula trail and basic functions");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 1);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert (gv_get_location (gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert (gv_get_location (gv, PLAYER_VAN_HELSING) == ZURICH);
		assert (gv_get_location (gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert (gv_get_location (gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for encountering Dracula and hunter history");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED...";
		player_message messages[] = {
			"Hello", "Rubbish",  "Stuff", "", "Mwahahah",
			"Aha!"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_location (gv, PLAYER_DRACULA) == GENEVA);
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == 5);
		assert (gv_get_health (gv, PLAYER_DRACULA) == 30);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == GENEVA);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == GENEVA);
		assert (history[1] == UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_LORD_GODALMING, history);
		assert (history[0] == GENEVA);
		assert (history[1] == STRASBOURG);
		assert (history[2] == UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_DR_SEWARD, history);
		assert (history[0] == ATLANTIC_OCEAN);
		assert (history[1] == UNKNOWN_LOCATION);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Hunter View)");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == SEA_UNKNOWN);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Dracula View)");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DEC.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == ENGLISH_CHANNEL);

		puts ("passed");
		gv_drop (gv);
	} while (0);

	do {////////////////////////////////////////////////////////////////
		puts ("Hunter dies");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DEC.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == ENGLISH_CHANNEL);

		puts ("passed");
		gv_drop (gv);
	} while (0);



	do {////////////////////////////////////////////////////////////////
		puts ("Test for connections");
		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		do {
			puts ("Checking Galatz road connections");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				GALATZ, PLAYER_LORD_GODALMING, 0,
				true, false, false
			);
			
			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++) {
				seen[edges[i]] = true;
			}
			assert (n_edges == 5);
			assert (seen[GALATZ]);
			assert (seen[CONSTANTA]);
			assert (seen[BUCHAREST]);
			assert (seen[KLAUSENBURG]);
			assert (seen[CASTLE_DRACULA]);
			free (edges);
		} while (0);

		do {
			puts ("Checking Ionian Sea sea connections");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				IONIAN_SEA, PLAYER_LORD_GODALMING, 0,
				false, false, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++)
				seen[edges[i]] = true;


			//printf("n_edges = %d\n", n_edges);
			assert (n_edges == 7);
			assert (seen[IONIAN_SEA]);
			assert (seen[BLACK_SEA]);
			assert (seen[ADRIATIC_SEA]);
			assert (seen[TYRRHENIAN_SEA]);
			assert (seen[ATHENS]);
			assert (seen[VALONA]);
			assert (seen[SALONICA]);

			free (edges);
		} while (0);

		do {
			puts ("Checking Athens rail connections (none)");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				ATHENS, PLAYER_LORD_GODALMING, 0,
				false, true, false
			);

			assert (n_edges == 1);
			assert (edges[0] == ATHENS);

			free (edges);
		} while (0);


		//MY TESTs
		do {
			puts ("\n\nChecking Vienna connections (2 rail hops)");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				VIENNA, PLAYER_DR_SEWARD, 13,
				true, true, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++) {
				//printf("edges[%d] = %s\n",i, location_get_name(edges[i]));
				seen[edges[i]] = true;
			}

			assert (n_edges == 8);
			assert (seen[VIENNA]);
			assert (seen[VENICE]);
			assert (seen[PRAGUE]);
			assert (seen[BUDAPEST]);
			assert (seen[ZAGREB]);
			assert (seen[MUNICH]);

			free (edges);
		} while (0);

		do {
			puts ("\n\nChecking Vienna connections (3 rail hops)");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				VIENNA, PLAYER_VAN_HELSING, 13,
				true, true, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++) {
				//printf("edges[%d] = %s\n",i, location_get_name(edges[i]));
				seen[edges[i]] = true;
			}

			assert (n_edges == 12);
			assert (seen[VIENNA]);
			assert (seen[VENICE]);
			assert (seen[PRAGUE]);
			assert (seen[BERLIN]);
			assert (seen[LEIPZIG]);
			assert (seen[HAMBURG]);
			assert (seen[BUDAPEST]);
			assert (seen[SZEGED]);
			assert (seen[BUCHAREST]);
			assert (seen[BELGRADE]);
			assert (seen[ZAGREB]);
			assert (seen[MUNICH]);

			free (edges);
		} while (0);

		do {
			puts ("\n\nChecking SARAJEVO connections");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				SARAJEVO, PLAYER_DRACULA, 13,
				true, true, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++) {
				printf("edges[%d] = %s\n",i, location_get_name(edges[i]));
				seen[edges[i]] = true;
			}

			assert (n_edges == 5);
			assert (seen[SARAJEVO]);
			assert (seen[ZAGREB]);
			assert (seen[VALONA]);
			assert (seen[SOFIA]);
			assert (seen[BELGRADE]);
			

			free (edges);
		} while (0);

		do {
			puts ("\n\nChecking dracula starting in a sea");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				ADRIATIC_SEA, PLAYER_DRACULA, 13,
				true, true, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++) {
				printf("edges[%d] = %s\n",i, location_get_name(edges[i]));
				seen[edges[i]] = true;
			}

			assert (n_edges == 4);
			assert (seen[VENICE]);
			assert (seen[IONIAN_SEA]);
			assert (seen[BARI]);

			free (edges);
		} while (0);

		do {
			puts ("\n\nChecking dracula starting in an unknown sea");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				SEA_UNKNOWN, PLAYER_DRACULA, 13,
				true, true, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++) {
				//printf("edges[%d] = %s\n",i, location_get_name(edges[i]));
				seen[edges[i]] = true;
			}

			assert (n_edges == 0);

			free (edges);
		} while (0);

		do {
			puts ("\n\nChecking anyone starting in an unknown location");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				CITY_UNKNOWN, PLAYER_LORD_GODALMING, 13,
				true, true, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++) {
				//printf("edges[%d] = %s\n",i, location_get_name(edges[i]));
				seen[edges[i]] = true;
			}

			assert (n_edges == 0);

			free (edges);
		} while (0);

		puts ("passed");
		gv_drop (gv);

		do {////////////////////////////////////////////////////////////////
			puts ("Test for Goldaming death");

			char *trail =
				"GGE..D. SGE.... HGE.... MGE.... DS?.... "
				"GAO..D. SST.... HST.... MST.... DC?.... "
				"GSTT... SST.... HST.... MST.... DD2.... ";
			player_message messages[] = {
				"Hello", "Rubbish", "Stuff", "", "Mwahahah",
				"Aha!", "", "", "", "Back I go"};
			GameView gv = gv_new (trail, messages);

			assert (gv_get_player (gv) == 0);
			assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_2);
			printf("gv_get_health(gv, PLAYER_LORD_GODALMING) = %d\n",gv_get_health(gv, PLAYER_LORD_GODALMING));
			assert(gv_get_location (gv, PLAYER_LORD_GODALMING) == HOSPITAL_LOCATION);

			location_t history[TRAIL_SIZE];
			gv_get_history (gv, PLAYER_DRACULA, history);

			puts ("passed");
			gv_drop (gv);
		} while (0);

		
	} while (0);


	return EXIT_SUCCESS;
}
