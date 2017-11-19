/* 	Author: 	Andrius Kelly
	Date:		10/28/2017
	Description: Random test for "treasure_map" card
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
void printCardLocation(int size, int arr[], int card);
void treasuremapGameStateHelper(struct gameState *G, int handPos);

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
		validRandomGameState(&G);
		whoseTurn = G.whoseTurn;
		handPos = rand() % (G.handCount[whoseTurn] + 1) - 1; //prevent divide by zero
		handPos = (handPos < 0) ? 0 : handPos;
		G.hand[whoseTurn][handPos] = treasure_map;
		bonus = rand();

		memcpy(&oracle, &G, sizeof(struct gameState));
		memcpy(&pre, &G, sizeof(struct gameState));

		cardEffect( treasure_map,  rand(),  rand(), rand(), &G,  handPos, &bonus);
		treasuremapGameStateHelper(&oracle, handPos);
		
		checkFails = assertGameState( &oracle, &G);

		if (checkFails > 0){
			testFails++;
			if(NOISY_TEST) {
				printf("FAIL:\n");
			}
		}
		else if (NOISY_TEST){
				printf("SUCCESS:\n");
		}
		if (NOISY_TEST){
			printf("player %d hand: %d, deck: %d, gold supply: %d treasure_map at index:", whoseTurn, pre.handCount[whoseTurn], pre.deckCount[whoseTurn], pre.supplyCount[gold]);
				printCardLocation(pre.handCount[whoseTurn], pre.hand[whoseTurn], treasure_map);
				printf("\n");
		}

	}

	printf("%d Total Tests with %d Failures for treasure_map\n\n", numTests, testFails);

	return 0;
}




// ----------------------------FUNCTIONS-----------------------------




//prints location of given cardType in array, if any
void printCardLocation(int size, int arr[], int card){
	int foundFlag = 0;
	for(int i = size-1; i >= 0; i--){
		if( arr[i] == card){
			printf("%d ", i);
			foundFlag = 1;
		}
	}
	if (foundFlag == 0){
		printf("None");
	}
	printf("\n");
}


//sets gamestate oracle for treasuremap card 
void treasuremapGameStateHelper(struct gameState *G, int handPos){
	int p = G->whoseTurn;
	int i;

//if(NOISY_TEST) printf("  Pre:  hand: %d, deck: %d, supplyCount: %d\n", G->handCount[p], G->deckCount[p], G->supplyCount[gold]);

	//trash the treasuremap card
	G->hand[p][handPos] = G->hand[p][ G->handCount[p]-1 ];
	G->handCount[p] -= 1;
	
	//look for another treasuremap, if found, trash that card and gain 4 gold
	if(G->handCount[p] > 0){
		
		for(i=0; i < G->handCount[p]; i++){
			
			if(G->hand[p][i] == treasure_map){
				
				//trash card
				G->hand[p][i] = G->hand[p][ G->handCount[p]-1 ];
				G->handCount[p] -= 1;

				//add up to four gold to deck
				for(i = 0; G->supplyCount[gold] > 0 && i < 4; i++ ){
					G->deck[p][G->deckCount[p] + i] = gold;
					G->supplyCount[gold] -= 1;
				}
				
				G->deckCount[p] += i;
				
				break;
			}
		}
	}

//if(NOISY_TEST) printf("  Post: hand: %d, deck: %d, supplyCount: %d\n", G->handCount[p], G->deckCount[p], G->supplyCount[gold]);

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
			printf("FAIL: gameState.%s = %d expected %d\n", name, prop2, prop1 );
		return 1;
	}
	return 0;
}

//Helper Function: checks array property of pre and post gameState
int testStateArray(int a1[], int a2[], int size, const char* name){
	for(int i=0; i< size; i++ ) {
		if ( a1[i] != a2[i] ) {
			if (NOISY_TEST > 1) 
				printf("FAIL: gameState.%s array did not match expected\n", name);
			return 1;
		}
	}
	return 0;
}
/*   
**	 assertGameState() checks between pre and post gamstate and returns differences
**   prints differences to console if( NOISY_TEST )
**     performs 11 + 2 * numPlayers total tests
**
**	 NOTE: cannot compare deck and discardelements due to randomness of shuffling
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

		sprintf(buffer, "discard[] for player %d", i);
		changes += testStateArray(pre->discard[i], post->discard[i], pre->discardCount[i], buffer);

		sprintf(buffer, "deckCount for player %d", i);
		changes += testStateInt(pre->deckCount[i], post->deckCount[i], buffer);

		sprintf(buffer, "deck[] for player %d", i);
		changes += testStateArray( pre->deck[i], post->deck[i], pre->deckCount[i], buffer);
	}

	return changes;
	return changes;
} 
