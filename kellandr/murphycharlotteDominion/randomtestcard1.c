/* 	Author: 	Andrius Kelly
	Date:		10/28/2017
	Description: Random test for "sea_hag" card
*/


#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"


#ifndef NOISY_TEST
	#define NOISY_TEST 1 //set to 0 for summary, 1 to print relevent state, 2 to print full state failures
#endif
#ifndef NUM_TESTS 
	#define NUM_TESTS 1000
#endif
//definine max supply to 60, for valid test
#ifndef MAX_SUPPLY
	#define MAX_SUPPLY 60
#endif

void validRandomGameState(struct gameState* G);
int assertGameState(struct gameState *pre, struct gameState *post);
void printPlayerInfo(struct gameState *G);
void seahagHelper(struct gameState *G, int handPos);

int main(int argc, char* argv[]){

	int testSeed = 1000;
	
	if(argc == 2){
		testSeed = atoi(argv[1]);
	}
	srand(testSeed);

	int numTests;
	int numChecks;
	int checkFails;			// # of gamestate checks that fail per test
	int testFails;			// # of tests that have failures

	struct gameState G;
	struct gameState oracle;	//to test post conditions agains
    struct gameState pre;		//to keep track of preconditions
    int handPos;
    int whoseTurn;
    int bonus;

	//run series of tests with valid random gamestates
    testFails = numChecks = 0;
    numTests = NUM_TESTS;

	for(int i=0; i < numTests; i++){
		
		//create random gameState
		bonus = rand();
		validRandomGameState(&G);
		
		whoseTurn = G.whoseTurn;
		handPos = rand() % (G.handCount[whoseTurn] + 1) - 1; //prevent divide by zero
		handPos = (handPos < 0) ? 0 : handPos;
		G.hand[whoseTurn][handPos] = sea_hag;		

		memcpy(&oracle, &G, sizeof(struct gameState));
		memcpy(&pre, &G, sizeof(struct gameState));

		cardEffect( sea_hag,  rand(),  rand(), rand(), &G,  handPos, &bonus);
		seahagHelper(&oracle, handPos);
		
		checkFails = assertGameState( &oracle, &G);

		if (checkFails > 0){
			testFails++;
			if(NOISY_TEST) {
				printf("FAIL:\n");
				printPlayerInfo(&pre);
			}
		}
		else if (NOISY_TEST){ //print success
				printf("SUCCESS:\n");
				printPlayerInfo(&pre);
		}
	}

	printf("%d Total Tests with %d Failures for sea_hag\n\n", numTests, testFails);

	return 0;
}



// ----------------------------FUNCTIONS-----------------------------

//for debugging
void printPlayerInfo(struct gameState *G){
	for(int i=0; i < G->numPlayers; i++){
		printf("player %d hand: %d, deck: %d, discard: %d\n", 
			i, G->handCount[i], G->deckCount[i], G->discardCount[i]);
	}
	printf("Curses: %d\n\n", G->supplyCount[curse]);
}

//creates expected gamestate for seahag effect
void seahagHelper(struct gameState *G, int handPos) {
	int p = G->whoseTurn;
	int n = G->numPlayers;
	int deckTop;
	int discardCount;
	//cycle through other characters
	for (int i=0; i < n; i++){
		if( i != p){
			//each player discards top deck to discard
			//if deck is empty, add discard to deck
			if(G->deckCount[i] <= 0){
				for(int j = 0; j < G->discardCount[p]; j++){
					G->deck[i][j] = G->discard[i][j];
					G->deckCount[i] = G->discardCount[i];
					G->discardCount[i] = 0;
				}
			}

			deckTop = G->deckCount[i] - 1;
			discardCount = G->discardCount[i];

			G->discard[i][discardCount] = G->deck[i][deckTop];
			G->discardCount[i] += 1;
			G->deckCount[i] = deckTop;

			//gain curse to top of deck
			if(G->supplyCount[curse] > 0){
				G->deck[i][deckTop] = curse;
				G->deckCount[i] = deckTop + 1;
				G->supplyCount[curse] -= 1;
			}			
		}
	}

	//remove seahag from player's hand
	G->hand[p][handPos] = G->hand[p][ G->handCount[p] ];
	G->handCount[p] -= 1;

	//add seahag to played array 
	G->playedCards [ G->playedCardCount ] = sea_hag;
	G->playedCardCount += 1; 
}


/* populates gamestate with valid card enums and does not violate minimum or maximums
** k is filled with random nonrepeating enums from adventurer to treasure_map, inclusive
** gameSeed is set to rand()
** numPlayers is set between 1 and MAX_PLAYERS
*/
void validRandomGameState(struct gameState* G){

	int i, j;

	G->numPlayers = rand() % MAX_PLAYERS + 1;
	
	for( i = 0; i<treasure_map+1; i++){
		G->supplyCount[i] = rand() % (MAX_SUPPLY + 1);
		G->embargoTokens[i] = rand() % (MAX_SUPPLY + 1);
	}

	G->outpostPlayed = rand() % (MAX_SUPPLY + 1);
	G->outpostTurn = rand() % G->numPlayers;
	G->whoseTurn = rand() % G->numPlayers;
	G->phase = rand() % 2;
	G->numActions = rand() % (MAX_SUPPLY + 1); /* Starts at 1 each turn */
	G->coins = rand() % (MAX_SUPPLY + 1); /* Use as you see fit! */
	G->numBuys = rand() % (MAX_SUPPLY + 1); /* Starts at 1 each turn */

	for ( i = 0; i < MAX_PLAYERS; i++){
		G->handCount[i] = rand() % MAX_HAND + 1;
		G->deckCount[i] = rand() % MAX_DECK;
		G->discardCount[i] = rand() % MAX_DECK;

		for( j = 0; j < MAX_DECK; j++){
			G->hand[i][j] = rand() % (treasure_map + 1);
			G->deck[i][j] = rand() % (treasure_map + 1);
			G->discard[i][j] = rand() % (treasure_map + 1);
		}
	}
	for( j = 0; j < MAX_DECK; j++)
		G->playedCards[i] = rand() % (treasure_map + 1);
}


//Helper Function: checks integer property of pre and post gameState
int testStateInt(int prop1, int prop2, const char* name){
	if (prop1 != prop2 ){
		if (NOISY_TEST > 1) 
			printf("  FAIL: gameState.%s = %d expected %d\n", name, prop2, prop1 );
		return 1;
	}
	return 0;
}

//Helper Function: checks array property of pre and post gameState
int testStateArray(int a1[], int a2[], int size, const char* name){
	for(int i=0; i< size; i++ ) {
		if ( a1[i] != a2[i] ) {
			if (NOISY_TEST > 1) 
				printf("  FAIL: gameState.%s array did not match expected\n", name);
			return 1;
		}
	}
	return 0;
}
/*   
**	 assertGameState() checks between pre and post gamstate and returns differences
**   prints differences to console if( NOISY_TEST )
**   NOTE: cannot compare deck and discard arrays due to shuffling
*/
int assertGameState(struct gameState *pre, struct gameState *post){
	
	int numPlayers = pre->numPlayers;

	char buffer[100];

	int changes = 0;
	
	changes += testStateInt(pre->numPlayers, post->numPlayers, "numPlayers");
	changes += testStateArray(pre->supplyCount, post->supplyCount, treasure_map+1, "supplyCount");
	changes += testStateArray(pre->embargoTokens, post->embargoTokens, treasure_map+1, "embargoTokens");
	changes += testStateInt(pre->outpostPlayed, post->outpostPlayed, "outpostPlayed");
	changes += testStateInt(pre->whoseTurn, post->whoseTurn, "whoseTurn");
	changes += testStateInt(pre->phase, post->phase, "phase");
	changes += testStateInt(pre->numActions, post->numActions, "numActions");
	changes += testStateInt(pre->coins, post->coins, "coins");
	changes += testStateInt(pre->numBuys, post->numBuys, "numBuys");
	changes += testStateArray(pre->playedCards, post->playedCards, pre->playedCardCount, "playedCards");
	changes += testStateInt(pre->playedCardCount, post->playedCardCount, "playedCardCount");

	for( int i=0; i < numPlayers; i++){

		sprintf(buffer, "hand[] for player %d", i);	
		changes += testStateArray(pre->hand[i], post->hand[i], pre->handCount[i], buffer);
		
		sprintf(buffer, "handCount for player %d", i);	
		changes += testStateInt(pre->handCount[i], post->handCount[i], buffer);

		sprintf(buffer, "discardCount for player %d", i);
		changes += testStateInt(pre->discardCount[i], post->discardCount[i], buffer);

		sprintf(buffer, "deckCount for player %d", i);
		changes += testStateInt(pre->deckCount[i], post->deckCount[i], buffer);

/*  //Can't compare deck and discard arrays due to shuffling

		sprintf(buffer, "discard[] for player %d", i);
		changes += testStateArray(pre->discard[i], post->discard[i], pre->discardCount[i], buffer);

		sprintf(buffer, "deck[] for player %d", i);
		changes += testStateArray( pre->deck[i], post->deck[i], pre->deckCount[i], buffer);
*/	}

	return changes;
} 
