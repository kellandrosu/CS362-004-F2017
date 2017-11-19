/* 	Author: 	Andrius Kelly
	Date:		10/13/2017
	Description: Unit test for "treasure map" card
	If player has two treasures in hand, gain four gold and trash card
	If no other treasure_map, trash card anyway
*/

#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"

//set to 1 to remove prints on every test
#define NOISY_TEST 1

int assertGameState(struct gameState *pre, struct gameState *post); 
void treasuremapGameStateHelper(struct gameState *G, int handPos);

int main(void){

	//gamestate vars
	int seed = 1000;
	int numPlayers = 2;    
    int k[10] = {adventurer, gardens, embargo, village, minion, mine, cutpurse, sea_hag, tribute, smithy};
    struct gameState G;
    struct gameState pre;

   //vars for testing
    int handPos;
    int choice = 0;
    int deckCount = 10;
    int testCount = 0;
    int failures = 0;
    int p = 0;

	deckCount = 10;

	printf ("\nTesting treasure_map card effect():\n");
	
	//initialize new game
	initializeGame(numPlayers, k, seed, &G);

	//iterate through players
	for( p = 0; p < numPlayers; p++){
	
		//choice shouldn't matter, so increment to find faults
		choice++;	

		G.whoseTurn = p;

		//fill hand 
		for(int i = 0; i < 10; i++){
			G.hand[p][i] = k[i];
		}	
		G.handCount[p] = 10;

	//TEST 1  - test with two treasuremap
		if(NOISY_TEST) printf(" Testing player %d p with two treasure_map cards in hand\n",p);
		
		handPos = 3;
		G.hand[p][2] = G.hand[p][handPos] = treasure_map;
		//copy gamestate
		memcpy(&pre, &G, sizeof(struct gameState));
		//run cardeffect
		cardEffect( treasure_map,  choice,  choice, choice, &G,  handPos, &choice);
		//manually change pre gamestate to expected
		treasuremapGameStateHelper(&pre, handPos);
		//check for failures
		if(assertGameState( &pre, &G)) failures++;
		testCount++;

		//reset gamestate G to expected
		memcpy(&G, &pre, sizeof(struct gameState));
		
	//TEST 2  - test with one treasuremap
		if(NOISY_TEST) printf(" Testing player %d with one treasure_map card in hand\n",p);
		//test with one treasuremap in hand and one outside hand
		handPos = 3;
		G.hand[p][G.handCount[p]] = G.hand[p][handPos] = treasure_map;
		//copy gamestate
		memcpy(&pre, &G, sizeof(struct gameState));
		//run cardeffect
		cardEffect( treasure_map,  choice,  choice, choice, &G,  handPos, &choice);
		//manually change pre gamestate to expected
		treasuremapGameStateHelper(&pre, handPos);
		//check for failures
		if(assertGameState( &pre, &G)) failures++;
		testCount++;

		//reset gamestate G to expected
		memcpy(&G, &pre, sizeof(struct gameState));

		//fill hand 
		for(int i = 0; i < 10; i++){
			G.hand[p][i] = k[i];
		}	
		G.handCount[p] = 10;

	//TEST 3  -  gold supply = 3
		if(NOISY_TEST) printf(" Testing player %d with two treasure cards and gold supply = 3\n",p);
		//test with two treasuremap, one at last index in hand
		handPos = G.handCount[p]-1;
		G.hand[p][3] = G.hand[p][handPos] = treasure_map;
		G.supplyCount[gold] = 3;

		//copy gamestate
		memcpy(&pre, &G, sizeof(struct gameState));
		//run cardeffect
		cardEffect( treasure_map,  choice,  choice, choice, &G,  handPos, &choice);
		//manually change pre gamestate to expected
		treasuremapGameStateHelper(&pre, handPos);
		//check for failures
		if(assertGameState( &pre, &G)) failures++;
		testCount++;

		//reset gamestate G to expected
		memcpy(&G, &pre, sizeof(struct gameState));

	//TEST 4  - gold supply = 0;
		if(NOISY_TEST) printf(" Testing player %d with two treasure cards and gold supply = 0\n", p);
		//test with two treasuremap
		handPos = 3;
		G.hand[p][0] = G.hand[p][handPos] = treasure_map;
		G.supplyCount[gold] = 0;

		//copy gamestate
		memcpy(&pre, &G, sizeof(struct gameState));
		//run cardeffect
		cardEffect( treasure_map,  choice,  choice, choice, &G,  handPos, &choice);
		//manually change pre gamestate to expected
		treasuremapGameStateHelper(&pre, handPos);
		//check for failures
		if(assertGameState( &pre, &G)) failures++;
		testCount++;

	}

	printf("%d Total Tests  %d Failures for treasure_map\n\n", testCount, failures);


	return 0;
}


//--------------------  FUNCTIONS --------------------------------

void treasuremapGameStateHelper(struct gameState *G, int handPos){
	int p = G->whoseTurn;
	int i;

if(NOISY_TEST) printf("  Pre:  hand: %d, deck: %d, supplyCount: %d\n", G->handCount[p], G->deckCount[p], G->supplyCount[gold]);

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
if(NOISY_TEST) printf("  Post: hand: %d, deck: %d, supplyCount: %d\n", G->handCount[p], G->deckCount[p], G->supplyCount[gold]);

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
	for(int i=0; i< size; i++ ) {
		if ( a1[i] != a2[i] ) {
			if (NOISY_TEST) 
				printf("  FAIL: gameState.%s array did not match expected\n", name);
			return 1;
		}
	}
	return 0;
}


/*   
**	 assertGameState() checks between pre and post gamstate and returns differences
**   prints differences to console if( NOISY_TEST )
**     performs 11 + 6 * numPlayers total tests
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
} 
