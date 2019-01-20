// customDracTest.c ... test the ADTs more extensively

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "dracula_view.h"

int main()
{
    DraculaView gv;
 
    printf("\t\tTest From PastPlays in Bugfixes\n");

    printf("\n\tGame #0 samples, End of Round 1\n");
    player_message messages2[] = {"Hello", "There", "This", "Should", "Be Good", 
                                 "Yas", "I'm Getting", "A", "Bit" };
    gv = dv_new("GMN.... SPL.... HAM.... MPA.... DZU.V.. "
                     "GLV.... SLO.... HNS.... MST....", messages2);
    printf("Round/Score Tests\n");
    assert(dv_get_round(gv) == 1);
    assert(dv_get_score(gv) == 365);
    printf("passed\n");
    printf("Location History Tests\n");
    location_t history[TRAIL_SIZE];
    dv_get_trail (gv, 0, history);
    assert(history[0] == LIVERPOOL);
    assert(history[1] == MANCHESTER); 
    assert(history[2] == UNKNOWN_LOCATION);
    dv_get_trail (gv,1,history);
    assert(history[0] == LONDON);
    assert(history[1] == PLYMOUTH);
    assert(history[2] == UNKNOWN_LOCATION);
    dv_get_trail (gv,2,history);
    assert(history[0] == NORTH_SEA);
    assert(history[1] == AMSTERDAM);
    assert(history[2] == UNKNOWN_LOCATION);
    dv_get_trail (gv,3,history);
    assert(history[0] == STRASBOURG);
    assert(history[1] == PARIS);
    assert(history[2] == UNKNOWN_LOCATION);
    dv_get_trail (gv,4,history);
    assert(history[0] == ZURICH);
    assert(history[1] == UNKNOWN_LOCATION);
    printf("Passed Location History Tests\n");
    printf("Vampire/Trap Location Tests\n");
    int nT, nV;
    dv_get_locale_info(gv, ADRIATIC_SEA, &nT, &nV);
    assert(nT == 0 && nV == 0);
    dv_get_locale_info(gv, ZURICH, &nT, &nV);
    assert(nT == 0 && nV == 1);
    dv_get_locale_info(gv, ZURICH, &nT, &nV);
    assert(nT == 0 && nV == 1);
    dv_get_locale_info(gv, MANCHESTER, &nT, &nV);
    assert(nT == 0 && nV == 0);
    printf("Passed Vampire/Trap Tests\n");
     dv_drop(gv);

    printf("\n\tGame #1, Dracula's Turn, 5 complete Rounds\n");
    player_message messages3[]={""};
    gv = dv_new("GMN.... SPL.... HAM.... MPA.... DGATV.. "
                     "GLV.... SLO.... HNS.... MST.... DHIT... "
                     "GIR.... SPL.... HAO.... MZU.... DCDT... "
                     "GSW.... SLO.... HNS.... MFR.... DKLT... "
                     "GLV.... SPL.... HAO.... MZU.... DBCT... "
                     "GSW.... SLO.... HNS.... MMR....", messages3);
    printf("Score And Round Number tests\n");
    assert(dv_get_round(gv) == 5);
    assert(dv_get_score(gv) == GAME_START_SCORE - 5);
    assert(dv_get_health(gv, 4) == GAME_START_BLOOD_POINTS + LIFE_GAIN_CASTLE_DRACULA);
    printf("Passed Score/Round tests\n");
    printf("Location History Tests\n");
    dv_get_trail (gv,0,history);
    assert(history[0] == SWANSEA);
    assert(history[1] == LIVERPOOL);
    assert(history[2] == SWANSEA);
    assert(history[3] == IRISH_SEA);
    assert(history[4] == LIVERPOOL);
    assert(history[5] == MANCHESTER);
    
    dv_get_trail (gv,1,history);
    assert(history[0] == LONDON);
    assert(history[1] == PLYMOUTH);
    assert(history[2] == LONDON);
    assert(history[3] == PLYMOUTH);
    assert(history[4] == LONDON);
    assert(history[5] == PLYMOUTH);

    dv_get_trail (gv,2,history);
    assert(history[0] == NORTH_SEA);
    assert(history[1] == ATLANTIC_OCEAN);
    assert(history[2] == NORTH_SEA);
    assert(history[3] == ATLANTIC_OCEAN);
    assert(history[4] == NORTH_SEA);
    assert(history[5] == AMSTERDAM);

    dv_get_trail (gv,3,history);
    assert(history[0] == MARSEILLES);
    assert(history[1] == ZURICH);
    assert(history[2] == FRANKFURT);
    assert(history[3] == ZURICH);
    assert(history[4] == STRASBOURG);
    assert(history[5] == PARIS);

    dv_get_trail (gv,4,history);
    assert(history[0] == BUCHAREST);
    assert(history[1] == KLAUSENBURG);
    assert(history[2] == CASTLE_DRACULA);

	//printf("history[3] = %s\n", location_get_abbrev(history[3]));
    assert(history[3] == GALATZ);
    assert(history[4] == GALATZ);
    printf("Passed Location History Tests\n");
    
    printf("Vampire/Trap Location Tests\n");
    dv_get_locale_info(gv, GALATZ, &nT, &nV);
    assert(nT == 1 && nV == 1);
    dv_get_locale_info(gv, CASTLE_DRACULA, &nT, &nV);
    assert(nT == 1 && nV == 0); 
    dv_get_locale_info(gv, KLAUSENBURG, &nT, &nV);
    assert(nT == 1 && nV == 0);
    dv_get_locale_info(gv, BUCHAREST, &nT, &nV);
    assert(nT == 1 && nV == 0);
    dv_get_locale_info(gv, HAMBURG, &nT, &nV);
    assert(nT == nV && nV == 0);
    printf("passed Vampire/Trap Location Tests\n");
     dv_drop(gv);

	gv = dv_new("GED.... SGE.... HZU.... MCA.... DCF.V.. GMN.... SCFVD.. HGE.... MLS.... DBOT... GLO.... SMR.... HCF.... MMA.... DBOT... GPL.... SMS.... HMR.... MGR.... DBAT... GLO.... SBATD.. HMS.... MMA.... DC?T... GPL.... SSJ.... HBA.... MGR.... DC?T... GPL.... SSJ.... HBA.... MGR.... DD4T...", messages3);

   	dv_get_trail (gv,4,history);
    assert(history[0] == location_find_by_abbrev("BO"));
    assert(history[1] == CITY_UNKNOWN);
    assert(history[2] == CITY_UNKNOWN);
    assert(history[3] == location_find_by_abbrev("BA"));
    assert(history[4] == location_find_by_abbrev("BO"));

	

	gv = dv_new("GED.... SGE.... HZU.... MCA.... DCF.V.. GMN.... SCFVD.. HGE.... MLS.... DBOT... GLO.... SMR.... HCF.... MMA.... DTOT... GPL.... SMS.... HMR.... MGR.... DBAT... GLO.... SBATD.. HMS.... MMA.... DSRT... GPL.... SSJ.... HBA.... MGR.... DALT... GPL.... SSJ.... HBA.... MGR.... DMAT... GLO.... SBE.... HMS.... MMATD..", messages3);

	size_t n_edges;
	
	location_t *edges = dv_get_dests (gv,&n_edges, true, true);

	bool seen[NUM_MAP_LOCATIONS] = {false};
	for (size_t i = 0; i < n_edges; i++) {
		//printf("edges[%d] = %s\n",i, location_get_abbrev(edges[i]));
		seen[edges[i]] = true;
	}
	//printf("\ncannot access\n");
	location_t trail[TRAIL_SIZE];
	dv_get_trail (gv, 4, trail);

	assert(dv_get_location(gv, PLAYER_DRACULA) == location_find_by_abbrev("MA"));


	assert (n_edges == 4);
	assert (!seen[location_find_by_abbrev("MA")]);
	assert (!seen[location_find_by_abbrev("AL")]);
	assert (!seen[location_find_by_abbrev("SR")]);
	assert (!seen[location_find_by_abbrev("BA")]);
	assert (!seen[location_find_by_abbrev("TO")]);
	assert (!seen[location_find_by_abbrev("BO")]);

	assert (seen[location_find_by_abbrev("SN")]);
	assert (seen[location_find_by_abbrev("LS")]);
	assert (seen[location_find_by_abbrev("GR")]);
	assert (seen[location_find_by_abbrev("CA")]);

	free (edges);

	

    printf(" ================= LEGENDARY ===============\n");
    return 0;
}



