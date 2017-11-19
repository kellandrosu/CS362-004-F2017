/* 	Author: 	Andrius Kelly
	Date:		10/28/2017
	Description: Random test for "adventurer" card
*/


#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"

#ifndef NOISY_TEST
	#define NOISY_TEST 2 //set to 0 for summary, 1 to print relevent state, 2 to print full state failures
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
void adventurerGameStateHelper(struct gameState *G, int handPos);
void printCardLocation(int size, int arr[], int card);

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
    struct gameState pre;		//to kep track of preconditions
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
		G.hand[whoseTurn][handPos] = sea_hag;
		bonus = rand();

		memcpy(&oracle, &G, sizeof(struct gameState));
		memcpy(&pre, &G, sizeof(struct gameState));

		cardEffect( adventurer,  rand(),  rand(), rand(), &G,  handPos, &bonus);
		adventurerGameStateHelper(&oracle, handPos);
		
		checkFails = assertGameState( &oracle, &G);

		if (checkFails > 0){
			testFails++;
			if(NOISY_TEST) 
				printf("FAIL \n");
		}
		else if(NOISY_TEST) {
			printf("SUCCESS \n");
		}
		if(NOISY_TEST) {
			printf(" player: %d\n handCount: %d deckCount: %d discardCount: %d\n", whoseTurn, pre.handCount[whoseTurn], pre.deckCount[whoseTurn], pre.discardCount[whoseTurn]);
			if(pre.deckCount[whoseTurn] > 0) {
				printf("  Copper in deck index: ");
				printCardLocation(pre.deckCount[whoseTurn], pre.deck[whoseTurn], copper);
				printf("  Silver in deck index: ");
				printCardLocation(pre.deckCount[whoseTurn], pre.deck[whoseTurn], silver);
				printf("  Gold in deck index: ");
				printCardLocation(pre.deckCount[whoseTurn], pre.deck[whoseTurn], gold);
			}
			if(pre.discardCount[whoseTurn] > 0) {
				printf("  Copper in discard index: ");
				printCardLocation(pre.discardCount[whoseTurn], pre.discard[whoseTurn], copper);
				printf("  Silver in discard index: ");
				printCardLocation(pre.discardCount[whoseTurn], pre.discard[whoseTurn], silver);
				printf("  Gold in discard index: ");
				printCardLocation(pre.discardCount[whoseTurn], pre.discard[whoseTurn], gold);
			}
			printf("\n");
		}
		
	}

	printf("%d Total Tests with %d Failures for adventurer\n\n", numTests, testFails);

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
	G->numActions = rand() % (MAX_SUPPLY + 1); 
	G->coins = rand() % (MAX_SUPPLY + 1);
	G->numBuys = rand() % (MAX_SUPPLY + 1); 

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

//sets the gamestate to its expected value based on the card effect
void adventurerGameStateHelper(struct gameState *G, int handPos){
	
	int i;
	int p = G->whoseTurn;
//printf("hand: %d, deck: %d, discard: %d\n", G->handCount[p], G->deckCount[p], G->discardCount[p]);
	//treasure picked flag
	int t_flag = 0;
	
	//drawn array to store drawn cards
	int drawnCard;
	int drawn[MAX_DECK];
	int drawnCount = 0;

	while(t_flag < 2 ) {

		if ( G->deckCount[p] == 0 ){
						
			if ( G->discardCount[p] == 0 ){
				//nothing to do, so break

				break;
			} 

			else {
				//add discarded cards to deck
				for( i = 0 ; i < G->discardCount[p]; i++ ){
					G->deck[p][i] = G->discard[p][i];
				}
				G->deckCount[p] = G->discardCount[p];
				G->discardCount[p] = 0;
			}

		}

		//draw until treasure cards are picked
		drawnCard = G->deck[p][ G->deckCount[p]-1 ];
		G->deckCount[p] -= 1;

		//if treasure picked, add to hand
		if( drawnCard == copper || drawnCard == silver || drawnCard == gold){
			t_flag++;
			G->hand[p][ G->handCount[p] ] = drawnCard;
			G->handCount[p] += 1;
		}
		else{		//else add to drawn
			drawn[drawnCount] = drawnCard;
			drawnCount++;
		}
	}

	//add drawn cards to discard pile
	for( i = 0 ; i < drawnCount; i++){
		G->discard[p][ G->discardCount[p] ] = drawn[i];
		G->discardCount[p] += 1;
	}

	//remove adventurer card from hand and add to played
	G->hand[p][handPos] = G->hand[p][ G->handCount[p]-1 ];
	G->handCount[p] -= 1;

	G->playedCards[ G->playedCardCount ] = adventurer;
	G->playedCardCount += 1;

//printf("hand: %d, deck: %d, discard: %d\n", G->handCount[p], G->deckCount[p], G->discardCount[p]);

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

		//due to shuffling, can only compare deck + discard totals
		sprintf(buffer, "discard+deck for player %d", i);
		changes += testStateInt(pre->discardCount[i] + pre->deckCount[i], post->discardCount[i] + post->deckCount[i], buffer);

	}

	return changes;
} 
