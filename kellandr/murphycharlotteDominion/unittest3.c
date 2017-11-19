/* 	Author: 	Andrius Kelly
	Date:		10/13/2017
	Description: Unit test for "buyCard()" function
*/

#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"

//set to 1 to remove prints on every test
#define NOISY_TEST 1

int assertGameState(int player, int numPlayers, struct gameState *pre, struct gameState *post);

void buyHelper(struct gameState *G, struct gameState *pre, int cardenum);


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

	printf ("Testing buyCard():\n");

//VALID INPUT
	if(NOISY_TEST) 
		printf("TESTING VALID INPUT\n");

//try to buy with zero buys left (should do nothing)
	if(NOISY_TEST) 
		printf(" Testing buyCard() with numBuys = 0\n");
	G.numBuys = 0;
	G.phase = 1; //set phase to buy	
	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));
	buyCard(adventurer, &G);
	//test gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;

//try to buy card that's too expensive (should do nothing)
	G.phase = 1; //set phase to buy	
	G.numBuys = 1;
	G.coins = 4;
	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));
	if(NOISY_TEST) 
		printf(" Testing buyCard() with numBuys = %d, coins = %d, price = %d\n", 
				G.numBuys, G.coins, getCost(adventurer));

	buyCard(adventurer, &G);
	//test gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;

//try to buy card with zero supply (should do nothing)
	G.phase = 1; //set phase to buy	
	G.numBuys = 1;
	G.coins = 6;
	G.supplyCount[adventurer] = 0;
	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));
	if(NOISY_TEST) 
		printf(" Testing buyCard() with supply = %d\n", G.supplyCount[adventurer]);
	buyCard(adventurer, &G);
	//test gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;


//try to buy exact price and buys = should be successful
	//initialize new game
	initializeGame(numPlayers, k, seed, &G);
	p = G.whoseTurn;

	G.phase = 1; //set phase to buy	
	G.numBuys = 1;
	G.coins = getCost(adventurer);
	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));

	buyHelper(&G, &pre, adventurer);
	
	//test gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;


//manually test buy with extra coins and extra buys
	//initialize new game
	initializeGame(numPlayers, k, seed, &G);	
	p = G.whoseTurn;
	G.phase = 1; //set phase to buy	
	G.numBuys = 3;
	G.coins = 10;

	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));
	
	buyHelper(&G, &pre, gardens);

	//test gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;

	buyHelper(&G, &pre, gold);

	//test gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;

//iterative test all valid card buys (set coins = buys = 100)
	//initialize new game
	initializeGame(numPlayers, k, seed, &G);	
	G.phase = 1; //set phase to buy	
	p = G.whoseTurn;
	G.numBuys = G.coins = 100;
	memcpy(&pre, &G, sizeof(struct gameState));
	
	if(NOISY_TEST) 
		printf("Testing buyCard() on all cards in k[]:\n");
	//test valid kingdom cards;
	for(int i=0; i<10; i++){
		buyHelper(&G, &pre, k[i]);
	}
	
	if(NOISY_TEST) 
		printf("Testing buyCard() on all victory and coin cards:\n");
	
	for(int i = estate; i <= gold; i++){
		buyHelper(&G, &pre, i);
	}
	//assert gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;


//INVALID INPUT
if(NOISY_TEST) 
	printf("TESTING INVALID INPUT\n");
	initializeGame(numPlayers, k, seed, &G);	
	p = G.whoseTurn;
	G.phase = 1; //set phase to buy	
	G.numBuys = 1;
	G.coins = 6;

//buy card at illegal index (should do nothing)
	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));
	if(NOISY_TEST) 
		printf(" Testing buyCard() on illegal index\n");
	buyCard(treasure_map+1, &G);
	//test gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;


//buy card not in k[] array (should do nothing)
	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));
	if(NOISY_TEST) 
		printf(" Testing buyCard() on card not in k[]\n");
	buyCard(salvager, &G);
	//test gamestate
	failures += assertGameState(p, numPlayers, &pre, &G);
	testCount += 11 + 6 * numPlayers;


	printf("%d Total Checks: %d Failures, %d Passes\n\n", testCount, failures, testCount-failures);

	return 0;
}


//---------------------------     FUNCTIONS        --------------------------

//buyHelper calls buyCard() for G and manually "buys" the card for the pre gameState
void buyHelper(struct gameState *G, struct gameState *pre, int cardenum){
	
	if(NOISY_TEST) 
		printf(" Testing buyCard() with numBuys = %d, coins = %d, price = %d\n", 
				G->numBuys, G->coins, getCost(cardenum));

	//buy card for gamestate G
	buyCard(cardenum, G);

	//manually "buy" card for pre gamestate
	pre->coins -= getCost(cardenum);
	pre->numBuys -= 1;

	int p = pre->whoseTurn;
	pre->discard[p][ pre->discardCount[p] ] = cardenum;
	pre->discardCount[p] += 1;
	pre->supplyCount[cardenum] -= 1;	
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