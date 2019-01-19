////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula_view.c: the DraculaView ADT implementation
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

#include "dracula_view.h"
#include "game.h"
#include "game_view.h"

#include "map.h" //... if you decide to use the Map ADT

#define __unused __attribute__((unused))


typedef struct dracula_view {

	GameView game_view;
	Move moves;
	
} dracula_view;

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
 *
 * For Dracula:
 * - the `past_plays` string should contain full details of all moves
 * - the last move should always be by Mina Harker
 */
dracula_view *dv_new (char *past_plays, player_message messages[])
{
	//create node
	dracula_view *new = malloc(sizeof(struct dracula_view));
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");

	//send to game_view
	new->game_view = gv_new(past_plays, messages);

	return new;
}

/**
 * Frees all resources allocated for `dv`.
 * After this has been called, `dv` should not be accessed.
 */
void dv_drop (dracula_view *dv)
{
	gv_drop(dv->game_view);
}

/**
 * Get the current round.
 */
round_t dv_get_round (dracula_view *dv)
{
	return gv_get_round(dv->game_view);
}

/**
 * Get the current score, a positive integer between 0 and 366.
 */
int dv_get_score (dracula_view *dv)
{
	return gv_get_score(dv->game_view);
}

/**
 * Get the current health points for a given player.
 * @param player specifies which players's life/blood points to return
 * @returns a value between 0..9 for Hunters, or >0 for Dracula
 */
int dv_get_health (dracula_view *dv, enum player player)
{
	return gv_get_health(dv->game_view, player);
}

/**
 * Get the current location of a given player.
 *
 * May be `UNKNOWN_LOCATION` if the player has not had a turn yet
 * (i.e., at the beginning of the game when the round is 0)
 *
 * Always returns an exact location, as the `past_plays` string contains
 * full Dracula locations since Dracula always knows where he's been.
 */
location_t dv_get_location (dracula_view *dv, enum player player)
{
	return gv_get_location(dv->game_view, player);
}

/**
 * Get the most recent move of a given player, returning the start and
 * end location of that move.
 *
 * Since Dracula moves last, the end location for other players is
 * always known, but the start location may be `UNKNOWN_LOCATION`
 * (for a hunter's first move).
 */
void dv_get_player_move (
	dracula_view *dv, enum player player,
	location_t *start, location_t *end)
{
	location_t trail[6] ={};
	gv_get_history(dv->game_view, player, trail);
	*start = trail[1];
	*end = trail[0];
	return;
}

/**
 * Find out what minions I (Dracula) have placed at the specified
 * location -- minions are traps and immature vampires -- and returns
 * the counts in the variables referenced by `n_traps` and `n_vamps`.
 *
 * If `where` is not a place where minions can be left
 * (e.g. at sea, or NOWHERE), then set both counts to zero.
 */
void dv_get_locale_info (
	dracula_view *dv, location_t where,
	int *n_traps, int *n_vamps)
{
	//printf("trap_count(dv->game_view) = %d\n",trap_count(dv->game_view, where));
	*n_traps = trap_count(dv->game_view, where);

	//printf("vamp_count(dv->game_view) = %d\n",vamp_count(dv->game_view, where));
	*n_vamps = vamp_count(dv->game_view, where);
	
}

/**
 * Fills the trail array with the locations of the last 6 turns for the
 * given player, including Dracula (if he asks about his own trail).
 * If the move does not exist yet (i.e., the start of the game),
 * the value should be UNKNOWN_LOCATION (-1).
 *
 * For example after 2 turns the array may have the contents
 *
 *     {29, 12, -1, -1, -1, -1}
 *
 * This would mean in the first move the player started on location 12
 * then moved to the current location of 29.
 *
 * If Dracula asks about his own trail, he should get real locations he
 * has previously been, not double-backs, etc.
 */
void dv_get_trail (
	dracula_view *dv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	return;
}

/**
 * Return an array of `location_t`s giving all locations that Dracula
 * could reach in his next move.
 *
 * The array can be in any order but must contain unique entries.
 * The array size is stored at the variable referenced by `n_locations`.
 * The current location should be included in the array.
 *
 * Road or sea connections should only be considered
 * if the `road` or `sea` parameters are true, respectively.
 *
 * The array should not include the hospital (where Dracula cannot go),
 * nor any locations only reachable by rail (which Dracula cannot use).
 * The set of possible locations must be consistent with the rules on
 * Dracula's movement (e.g. can't move to a location currently in his
 * trail).
 */
location_t *dv_get_dests (
	dracula_view *dv __unused, size_t *n_locations, bool road __unused, bool sea __unused)
{
	*n_locations = 0;
	return NULL;
}

/**
 * Return an array of `location_t`s giving all of the locations that the
 * given `player` could reach from their current location.
 *
 * The array can be in any order but must contain unique entries.
 * The array size is stored at the variable referenced by `n_locations`.
 * The player's current location should be included in the array.
 *
 * `road`, `rail`, and `sea` connections should only be considered
 * if the `road`, `rail`, `sea` parameters are true, respectively.
 *
 * The function must take into account the current round for determining
 * how far `player` can travel by rail.
 *
 * If `player` is Dracula, calls dv_get_dests().
 */
location_t *dv_get_dests_player (
	dracula_view *dv, size_t *n_locations, enum player player,
	bool road, bool rail, bool sea)
{
	return gv_get_connections(dv->game_view, n_locations, dv_get_location(dv, player), 4, dv_get_round(dv), road, rail, sea);
}
