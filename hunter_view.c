////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter_view.c: the HunterView ADT implementation
//
// Code by TheGroup, COMP1927 14s2.
// Modified by gac & jas, 15s2, 16s2.

#include <stdlib.h>
#include <assert.h>

#include "game.h"
#include "game_view.h"
#include "hunter_view.h"
#include "places.h"


size_t *hv_get_distance (
	HunterView currentView,location_t from, location_t to,
	enum player player, bool road, bool rail, bool sea);
location_t hv_move_dest (
	HunterView currentView, location_t dest,
	enum player player, bool road, bool rail, bool sea);
location_t hv_move_random (HunterView currentView, enum player player);
bool hv_has_other_hunters (HunterView currentView,
	enum player player, location_t loc);
location_t hv_find_dracula (HunterView currentView, enum player player);

// Representation of the Hunter's view of the game

struct hunter_view {
    GameView game;
};


// Creates a new HunterView to summarise the current state of the game
HunterView hv_new(char *pastPlays, player_message messages[])
{
    HunterView hunter_view = malloc(sizeof(struct hunter_view));
    hunter_view->game = gv_new(pastPlays, messages);
    return hunter_view;
}


// Frees all memory previously allocated for the HunterView toBeDeleted
void hv_drop(HunterView toBeDeleted)
{
    free(toBeDeleted->game);
    free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current round
round_t hv_get_round(HunterView currentView)
{
    return gv_get_round(currentView->game);
}

// Get the id of current player
enum player hv_get_player(HunterView currentView)
{
    return gv_get_player(currentView->game);
}

// Get the current score
int hv_get_score(HunterView currentView)
{
    return gv_get_score(currentView->game);
}

// Get the current health points for a given player
int hv_get_health(HunterView currentView, enum player player)
{
    return gv_get_health(currentView->game, player);
}

// Get the current location id of a given player
location_t hv_get_location(HunterView currentView, enum player player)
{
    return gv_get_location(currentView->game, player);
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void hv_get_trail(HunterView currentView, enum player player,
                    location_t trail[TRAIL_SIZE])
{
    gv_get_history(currentView->game, player, trail);
}

//// Functions that query the map to find information about connectivity

// What are my possible next moves (locations)
location_t *hv_get_dests(
	HunterView currentView, size_t *numLocations,
	bool road, bool rail, bool sea)
{
    return hv_get_dests_player(
		currentView,
		numLocations,
		gv_get_player(currentView->game),
		road, rail, sea);
}

// What are the specified player's next possible moves
location_t *hv_get_dests_player(
	HunterView currentView, size_t *numLocations,
	enum player player, bool road, bool rail, bool sea)
{
    size_t i, map_nvalidLocations, index;
    location_t forbidden = NOWHERE;
    location_t *validLocations;
	
	//printf("------------%s\n", location_get_name(gv_get_location(currentView->game, player)));
    location_t *locations =
		gv_get_connections(
			currentView->game,
			numLocations,
			gv_get_location(currentView->game, player),
			player,
			gv_get_round(currentView->game),
			road, rail, sea);

    if (player == PLAYER_DRACULA)
		forbidden = ST_JOSEPH_AND_ST_MARYS;
    map_nvalidLocations = 0;
    for(i = 0; i < (*numLocations); i++){
        if (locations[i] == forbidden) continue;
        map_nvalidLocations++;
    }

    index = 0;
    validLocations = malloc(sizeof(location_t) * map_nvalidLocations);
    for(i = 0; i < map_nvalidLocations; i++){
        if (locations[i] == forbidden) continue;
        validLocations[index] = locations[i];
        index++;
    }

    free(locations);
    *numLocations = map_nvalidLocations;
    return validLocations;
}

// find the distance from a location to another
size_t *hv_get_distance (
	HunterView currentView,location_t from, location_t to,
	enum player player, bool road, bool rail, bool sea)
{
	return gv_get_distance (currentView, from, to, player, road, rail, sea);
}

// take a move toward the destination
location_t hv_move_dest (
	HunterView currentView, location_t dest,
	enum player player, bool road, bool rail, bool sea)
{
		
	size_t n_neighbors = 0;
	size_t min_dist;
	location_t next;
	location_t curr = hv_get_location(currentView, player);
	location_t *moves = hv_get_dests_player(
		currentView, n_neighbors, player, road, rail, sea);

	min_dist = hv_get_distance (
			currentView, moves[0], dest, 
			player, road, rail, sea);
	next = moves[0];

	for (size_t i = 1; i < n_neighbors; i++) {
		size_t tmp_dist = hv_get_distance (
			currentView, moves[i], dest, 
			player, road, rail, sea);
		
		if (tmp_dist < min_dist) {
			min_dist = tmp_dist;
			next = moves[i];								
		}
	}
	
	return next;
}

// take a random move to one of the reachable locations 
location_t hv_move_random (HunterView currentView, enum player player) {

	size_t n_locations = 0;
	location_t curr = hv_get_location(currentView, player);
	// get possible moves
	// current move is intra
	location_t *moves = hv_get_dests_player(
		currentView, n_locations, player, true, true, true);

	// take a random move
	for (size_t i = 0; i < n_locations; i++) {
		if (!hv_has_other_hunters(currentView, player, moves[i]))
			return moves[i];	
	}
	
	return curr;	
}	

// check a location if it is occuppied by other hunters
// intend to spread the hunters out
bool hv_has_other_hunters (HunterView currentView,
	enum player player, location_t loc) 
{

	for (size_t i = 0; i < 3; i++) {
		if (i == player) continue;
		
		if (loc == hv_get_location(currentView, player))
			return true;
	}
	
	return false;
}

// find lastest location in dracula's trail
location_t hv_find_dracula (HunterView currentView, enum player player) {
		
	location_t trail[TRAIL_SIZE];
	hv_get_trail(currentView, player, trail);
	for (size_t i = 0; i < TRAIL_SIZE; i++) {
		if (valid_location_p(trail[i]))
			return trail[i];
	}
	
	return NOWHERE;
}




