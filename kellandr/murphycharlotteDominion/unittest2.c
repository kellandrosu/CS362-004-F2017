/* 	Author: 	Andrius Kelly
	Date:		10/13/2017
	Description: Unit test for "drawCard()" function
*/

#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "rngs.h"

//set to 1 to remove prints on every test
#define NOISY_TEST 1

//checks that the gamestate is the same for everything that should be unchanged
int assertGameState(int player, int numPlayers, struct gameState *pre, struct gameState *post);
int checkHandAndDeck(struct gameState* G, int p, int handCount, int hand[], int deckCount, int deck[]);
void emptyDeckDraw(int numToDraw,  struct gameState *G, struct gameState *pre);

int main(void){

	//gamestate vars
	int seed = 1000;
	int numPlayers = 2;    
    int k[10] = {adventurer, gardens, embargo, village, minion, mine, cutpurse, sea_hag, tribute, smithy};
    struct gameState G;
    struct gameState pre;

   //vars for testing
    int testCount;
    int failures;
    int p;
    int handCount;
    int deckCount;

	testCount = failures = 0;

	//initialize new game
	initializeGame(numPlayers, k, seed, &G);	
	p = G.whoseTurn;

	printf ("Testing drawCard():\n");

//VALID INPUT
	if(NOISY_TEST) 
		printf("TESTING VALID INPUT\n");

//TEST 1: drawcard on full deck

	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));

	handCount = pre.handCount[p];
	deckCount = pre.deckCount[p];
	
	if(NOISY_TEST) 
		printf(" Testing drawCard() on player %d, deck of size %d,\n", p, deckCount);
	
	//pop card off top of deck and into hand
	pre.hand[p][handCount] = pre.deck[p][deckCount-1];
	pre.handCount[p] += 1;
	pre.deckCount[p] -= 1;

	drawCard(p, &G);

	//check gamestate changes
	failures += assertGameState(p, numPlayers, &pre, &G);	
	testCount += 11 + 6 * numPlayers;	


//TEST 2:  draw if deck and discard are empty (should do nothing)
	G.deckCount[p] = G.discardCount[p] = 0; 

	if(NOISY_TEST) 
		printf(" Testing drawCard() on player %d, deck of size %d, discard of size %d\n", p, G.deckCount[p], G.discardCount[p]);

	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));

	drawCard(p, &G);

	//check gamestate changes
	failures += assertGameState(p, numPlayers, &pre, &G);	
	testCount += 11 + 6 * numPlayers;


//TEST 3: if deck is empty and discard > 0

	emptyDeckDraw( 1, &G, &pre);
	//check gamestate changes
	failures += assertGameState(p, numPlayers, &pre, &G);	
	testCount += 11 + 6 * numPlayers;

	emptyDeckDraw( 5, &G, &pre);
	//check gamestate changes
	failures += assertGameState(p, numPlayers, &pre, &G);	
	testCount += 11 + 6 * numPlayers;
	
//INVALID INPUT
	if(NOISY_TEST){ 
		printf("TESTING INVALID INPUT\n Testing drawCard() on player %d, deck of size %d, discard of size %d\n", 
			p, G.deckCount[p], G.discardCount[p]);
	}

	memcpy(&pre, &G, sizeof(struct gameState));
	drawCard(-1, &G);
	//check gamestate changes
	failures += assertGameState(p, numPlayers, &pre, &G);	
	testCount += 11 + 6 * numPlayers;


//print results
	printf("%d Total Checks: %d Failures, %d Passes\n\n", testCount, failures, testCount-failures);

	return 0;
}



//---------------------------     FUNCTIONS        --------------------------

//compare function adapted from http://www.cplusplus.com/reference/cstdlib/qsort/
int comp (const void * a, const void * b) {
  if ( *(int*)a  <  *(int*)b ) return -1;
  if ( *(int*)a  >  *(int*)b ) return 1;
  return 0;
}


//
void emptyDeckDraw(int numToDraw,  struct gameState *G, struct gameState *pre){

	int p = G->whoseTurn;

	if(NOISY_TEST) 
		printf(" Testing drawCard() on player %d, deck of size %d, discard of size %d\n", p, G->deckCount[p], numToDraw);
	
	G->deckCount[p] = 0; 
	G->discardCount[p] = numToDraw;

	//copy pre gamestate
	memcpy(pre, G, sizeof(struct gameState));

	//MUST draw full deck as random deck shuffling will create different hands
	for(int i=0; i<numToDraw; i++ )
		drawCard(p, G);

	//move discard into deck
	for(int i =0; i<pre->discardCount[p]; i++){
		pre->deck[p][i] = pre->discard[p][i];
		pre->discard[p][i] = -1;
		pre->deckCount[p] += 1;
	}
	pre->discardCount[p] = 0;

	//MUST draw full deck as random deck shuffling will create different hands
	while(pre->deckCount[p] > 0){
		pre->deckCount[p] -= 1;
		pre->hand[p][pre->handCount[p]] = pre->deck[p][pre->deckCount[p]];
		pre->handCount[p] += 1;		
	}

	//sort hands and decks to remove randomness from shuffle
	qsort(G->deck[p], numToDraw, sizeof(int), comp);
	qsort(pre->deck[p], numToDraw, sizeof(int), comp);
	qsort(G->hand[p], G->handCount[p], sizeof(int), comp);
	qsort(pre->hand[p], pre->handCount[p], sizeof(int), comp);
}

//Helper Function: checks integer property of pre and post gameState
int testStateInt(int prop1, int prop2, const char* name){
	if (prop1 != prop2 ){
		if (NOISY_TEST) 
			printf("  FAIL: gameState.%s = %d expected %d\n", name, prop2, prop1 );
		return 1;
	}
	return 0;
}


//Helper Function: checks array property of pre and post gameState
int testStateArray(int a1[], int a2[], int size, const char* name){
	if( memcmp(a1, a2, size) ){
		if (NOISY_TEST) 
			printf("  FAIL: gameState.%s array did not match expected\n", name);
		return 1;
	}
	return 0;
}


/*   
**	 assertGameState() checks any differences between pre and post gamstate 
**   assertGameState() will return number of differences and 
**     prints differences to console if( NOISY_TEST )
**     performs 11 + 6 * numPlayers total tests
*/
int assertGameState(int player, int numPlayers, struct gameState *pre, struct gameState *post){
	
	char buffer[100];

	int changes = 0;
	
	changes += testStateInt(pre->numPlayers, post->numPlayers, "numPlayers");
	changes += testStateArray(pre->supplyCount, post->supplyCount, sizeof(pre->supplyCount), "supplyCount");
	changes += testStateArray(pre->embargoTokens, post->embargoTokens, sizeof(pre->embargoTokens), "embargoTokens");
	changes += testStateInt(pre->outpostPlayed, post->outpostPlayed, "outpostPlayed");
	changes += testStateInt(pre->whoseTurn, post->whoseTurn, "whoseTurn");
	changes += testStateInt(pre->phase, post->phase, "phase");
	changes += testStateInt(pre->numActions, post->numActions, "numActions");
	changes += testStateInt(pre->coins, post->coins, "coins");
	changes += testStateInt(pre->numBuys, post->numBuys, "numBuys");
	changes += testStateArray(pre->playedCards, post->playedCards, sizeof(pre->playedCards), "playedCards");
	changes += testStateInt(pre->playedCardCount, post->playedCardCount, "playedCardCount");

	for( int i=0; i < numPlayers; i++){

		sprintf(buffer, "discardCount for player %d", i);
		changes += testStateInt(pre->discardCount[i], post->discardCount[i], buffer);

		sprintf(buffer, "discard[] for player %d", i);
		changes += testStateArray(pre->discard[i], post->discard[i], sizeof(pre->discard[i]), buffer);

		sprintf(buffer, "handCount for player %d", i);	
		changes += testStateInt(pre->handCount[i], post->handCount[i], buffer);

		sprintf(buffer, "hand[] for player %d", i);	
		changes += testStateArray(pre->hand[i], post->hand[i], sizeof(pre->hand[i]), buffer);

		sprintf(buffer, "deckCount for player %d", i);
		changes += testStateInt(pre->deckCount[i], post->deckCount[i], buffer);

		sprintf(buffer, "deck[] for player %d", i);
		changes += testStateArray( pre->deck[i], post->deck[i], sizeof(pre->deck[i]), buffer);
	}

	return changes;
} 