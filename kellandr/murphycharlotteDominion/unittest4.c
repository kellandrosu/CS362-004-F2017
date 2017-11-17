/* 	Author: 	Andrius Kelly
	Date:		10/13/2017
	Description: Unit test for "gainCard()" function
*/

#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"

//set to 1 to remove prints on every test
#define NOISY_TEST 1

//define enum for gain card destination
enum DESTINATION { toDiscard, toDeck, toHand};

int assertGameState(int player, int numPlayers, struct gameState *pre, struct gameState *post); 
void gainHelper(struct gameState *G, struct gameState *pre, int cardEnum, int destinationFlag);


int main (void){

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

	testCount = failures = 0;

	//initialize new game
	initializeGame(numPlayers, k, seed, &G);
	p = G.whoseTurn;

	printf ("Testing gainCard():\n");

//VALID INPUT
	if(NOISY_TEST) 
		printf("TESTING VALID INPUT\n Testing gainCard() on copper to discard pile\n");

//add copper to discard
	memcpy(&pre, &G, sizeof(struct gameState));
	gainHelper(&G, &pre, copper, toDiscard);
	//assert gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;

//add copper to deck
	if(NOISY_TEST) 
		printf(" Testing gainCard() on copper to deck\n");
	memcpy(&pre, &G, sizeof(struct gameState));
	gainHelper(&G, &pre, copper, toDeck);
	//assert gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;

//add copper to hand
	if(NOISY_TEST) 
		printf(" Testing gainCard() on copper to hand\n");
	memcpy(&pre, &G, sizeof(struct gameState));
	gainHelper(&G, &pre, copper, toHand);
	//assert gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;

//gain card of supply = 0
	if(NOISY_TEST) 
		printf(" Testing gainCard() on adventurer with supply set to 0\n");
	//set adventurer supply to 0
	G.supplyCount[adventurer] = 0; 
	memcpy(&pre, &G, sizeof(struct gameState));
	gainHelper(&G, &pre, adventurer, toHand);
	//assert gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;	

//iterate through all cards (including cards not in k[])
	//reinitialize game
	if(NOISY_TEST) 
		printf(" Testing gainCard() on all enumerated cards\n");
	initializeGame(numPlayers, k, seed, &G);
	p = G.whoseTurn;
	
	memcpy(&pre, &G, sizeof(struct gameState));	
	
	for(int i = curse; i <= treasure_map; i++){	
		gainHelper(&G, &pre, i, toDiscard);	
	}
	
	//assert gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;			


//INVALID INPUT
	//gain card illegal index
	if(NOISY_TEST) 
		printf("TESTING INVALID INPUT\n");

	if(NOISY_TEST) 
			printf(" Testing gainCard() on illegal card index\n");

	//reinitialize game
	initializeGame(numPlayers, k, seed, &G);
	p = G.whoseTurn;
	memcpy(&pre, &G, sizeof(struct gameState));	

	gainCard(treasure_map + 1, &G, toHand, p);
	gainCard(treasure_map + 1, &G, toDiscard, p);
	gainCard(treasure_map + 1, &G, toDeck, p);

	//assert gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;		

	printf("%d Total Checks: %d Failures, %d Passes\n\n", testCount, failures, testCount-failures);

	return 0;
}


//---------------------------     FUNCTIONS        --------------------------

//helper function calls gainCard() on G and manually "gains card" on pre
void gainHelper(struct gameState *G, struct gameState *pre, int cardEnum, int destinationFlag){
	int p = whoseTurn(G);

	gainCard(cardEnum, G, destinationFlag, p);

	if(pre->supplyCount[cardEnum] > 0) {
		
		pre->supplyCount[cardEnum] -= 1;

		switch(destinationFlag){
			// 0 : add to discard
			case toDiscard:
				pre->discard[p][pre->discardCount[p]] = cardEnum;
				pre->discardCount[p] += 1;
				break;
			// 1 : add to deck
			case toDeck:
				pre->deck[p][pre->deckCount[p]] = cardEnum;
				pre->deckCount[p] += 1;
				break;
			// 2 : add to hand
			case toHand:
				pre->hand[p][pre->handCount[p]] = cardEnum;
				pre->handCount[p] += 1;
				break;
		}
	}
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
