/* 	Author: 	Andrius Kelly
	Date:		10/13/2017
	Description: Unit test for "discardCard()" function
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
int checkHandAndDiscard(struct gameState* G, int p, int handCount, int hand[], int discardCount, int discard[]);

//compare function adapted from http://www.cplusplus.com/reference/cstdlib/qsort/
int comp (const void * a, const void * b) {
  if ( *(int*)a  <  *(int*)b ) return -1;
  if ( *(int*)a  >  *(int*)b ) return 1;
  return 0;
}

int main(void) {

	//gamestate vars
	int seed = 1000;
	int numPlayers = 2;    
    int k[10] = {adventurer, gardens, embargo, village, minion, mine, cutpurse, sea_hag, tribute, smithy};
    struct gameState G;
    struct gameState pre;

    //vars for testing
    int i;
    int testCount;
    int failures;
    int trashFlag;
    int p;
	int hand[MAX_HAND];
	int handCount;
	int discard[MAX_DECK];
	int discardCount;
	int card;

	char* actionString = NULL;
	char* trashString = "trashing";
	char* discardString = "discarding";

	//initialize new game
	initializeGame(numPlayers, k, seed, &G);	

	//copy current player's hand to hand[]
	p = G.whoseTurn;
	handCount = G.handCount[p];
	memcpy(hand, G.hand[p], handCount);

	//copy current discard
	discardCount = G.discardCount[p];
	memcpy(discard, G.discard[p], discardCount);


printf ("TESTING discardCard():\n");

// -------- VALID INPUT TESTS --------

	testCount = failures = 0;

	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));

	assert(handCount); //make sure handCount > 0

	//test discarding card discard in middle of hand
		if(NOISY_TEST) {
			printf("TESTING VALID INPUT\n");
			printf(" discarding player %d hand of size %d at index %d\n", p, handCount, handCount/2 );
		}
		//get the card number
		card = G.hand[p][handCount/2];
		discardCard( handCount/2, p, &G, 0);
		//remove card from hand copy
		hand[handCount/2] = hand[handCount-1];
		handCount--;
		discard[discardCount] = card;
		discardCount++;

		//sort hand and discard for both, to ignore order
		qsort(G.hand[p], G.handCount[p], sizeof(int), comp);
		qsort(G.discard[p], G.discardCount[p], sizeof(int), comp);
		qsort(pre.hand[p], pre.handCount[p], sizeof(int), comp);
		qsort(pre.discard[p], pre.discardCount[p], sizeof(int), comp);

		failures = checkHandAndDiscard(&G, p, handCount,  hand,  discardCount,  discard);
		testCount += 4;
		
	//copy pre gamestate
	memcpy(&pre, &G, sizeof(struct gameState));

	//iterate through hand, discarding top of hand until hand is empty
		handCount = pre.handCount[p];
		for(i = handCount-1; i >= 0; i--) {
		
			//copy expected result to avoid cascading errors
			memcpy(&G, &pre, sizeof(struct gameState));

			trashFlag = i % 2; //trash odd cards			
			
			if(NOISY_TEST){
				actionString = trashFlag ? trashString : discardString;
				printf(" %s player %d hand of size %d at index %d\n", actionString, p, pre.handCount[p], i );
			}
			
			discardCard( i, p, &G, trashFlag);
			
			card = pre.hand[p][pre.handCount[p]];
			pre.handCount[p] -= 1;
			if ( trashFlag == 0 ) {
				pre.discard[p][pre.discardCount[p]] = card;
				pre.discardCount[p]++;
			}
			
			failures += checkHandAndDiscard(&G, p, pre.handCount[p],  pre.hand[p],  pre.discardCount[p],  pre.discard[p]);
			
			testCount += 4;
		}

		//check gameState
	if(NOISY_TEST) 
		printf(" checking game states\n");
	failures += assertGameState(p, numPlayers, &pre, &G);	
	testCount += 7 + 6 * numPlayers;			

// -------- INVALID INPUT TESTS --------
	if(NOISY_TEST) 
		printf("TESTING INVALID INPUT\n");

	//reset markers
	memcpy(&pre, &G, sizeof(struct gameState));
	handCount = G.handCount[p];
	memcpy(hand, G.hand[p], handCount);
	discardCount = G.discardCount[p];
	memcpy(discard, G.discard[p], discardCount);

// discardCard on empty hand should not do anything
	for ( i=0; i<2; i++){
		if (NOISY_TEST)
			printf(" discarding player %d hand of size %d at index %d\n", p, handCount, i);
		discardCard( i, p, &G, 0);
		failures += checkHandAndDiscard(&G, p, handCount,  hand,  discardCount,  discard);
		testCount += 4;	
	}

	//check gameState
	if(NOISY_TEST) 
		printf(" checking game states\n");
	failures += assertGameState(p, numPlayers, &pre, &G);	
	testCount += 7 + 6 * numPlayers;		

//discardCard outside hand array should not do anything

	//reinitialize gamestate to fill hand
	initializeGame(numPlayers, k, seed, &G);

	//reset markers
	memcpy(&pre, &G, sizeof(struct gameState));
	handCount = G.handCount[p];
	memcpy(hand, G.hand[p], handCount);
	discardCount = G.discardCount[p];
	memcpy(discard, G.discard[p], discardCount);

	if (NOISY_TEST)
		printf(" discarding player %d hand of size %d at index %d\n", p, handCount, -1);
	discardCard( -1, p, &G, 0);
	failures += checkHandAndDiscard(&G, p, handCount,  hand,  discardCount,  discard);
	testCount += 4;
	//reinitialize G to expected to prevent cascading failures
	memcpy(&G, &pre, sizeof(struct gameState));

	if (NOISY_TEST)
		printf(" discarding player %d hand of size %d at index %d\n", p, handCount, handCount);
	discardCard( handCount, p, &G, 0);
	failures += checkHandAndDiscard(&G, p, handCount,  hand,  discardCount,  discard);
	testCount += 4;	
	//reinitialize G to expected to prevent cascading failures
	memcpy(&G, &pre, sizeof(struct gameState));

	//check gameState
	if(NOISY_TEST) 
		printf(" testing game states\n");
	failures += assertGameState(p, numPlayers, &pre, &G);	
	testCount += 7 + 6 * numPlayers;	


	//print results
	printf("%d Total Tests: %d Failures, %d Passes\n\n", testCount, failures, testCount-failures);

	return 0;

}



//---------------------------     FUNCTIONS        --------------------------

//Performs 4 checks: hand, handCount, discard, discardCount
//returns number of failures
int checkHandAndDiscard(struct gameState* G, int p, int handCount, int hand[], int discardCount, int discard[]) {

	int failures = 0;
	if(G->handCount[p] != handCount) {
		failures++;
		if (NOISY_TEST) 
			printf("  FAIL: player %d, handCount = %d, expected %d\n",p, G->handCount[p], handCount);
	}
	if( memcmp(hand, G->hand[p], handCount)){
		failures++;
		if (NOISY_TEST)
			printf("  FAIL: player %d, hand array did not match expected\n", p );
	}
	if (G->discardCount[p] != discardCount) {
		failures++;
		if (NOISY_TEST) 
			printf("  FAIL: player %d, discardCount = %d, expected %d\n",p, G->discardCount[p], discardCount);
	}
	if ( memcmp(discard, G->discard[p], discardCount)){
		failures++;
		if (NOISY_TEST)
			printf("  FAIL: player %d, discard array did not match expected\n", p );
	}
	return failures;
}


//checks integer property of pre and post gameState
int testStateIntProp(int prop1, int prop2, const char* name){
	if (prop1 != prop2 ){
		if (NOISY_TEST) 
			printf("  FAIL: gameState.%s changed\n", name );
		return 1;
	}
	return 0;
}


//checks array property of pre and post gameState
int testStateArrayProp(int a1[], int a2[], int size, const char* name){
	if( memcmp(a1, a2, size) ){
		if (NOISY_TEST) 
			printf("  FAIL: gameState.%s changed\n", name);
		return 1;
	}
	return 0;
}


/*   
**	 assertGameState() checks any differences between pre and post gamstate 
**     for ALL properties EXCEPT currentPlayer discard, discardCount, hand, handCount.
**   assertGameState() will return number of differences and 
**     prints differences to console if( NOISY_TEST )
**     performs 11 + 6 * numPlayers - 4 tests = 7 + 6 * numPlayers
*/
int assertGameState(int player, int numPlayers, struct gameState *pre, struct gameState *post){
	
	int changes = 0;
	
	changes += testStateIntProp(pre->numPlayers, post->numPlayers, "numPlayers");
	changes += testStateArrayProp(pre->supplyCount, post->supplyCount, sizeof(pre->supplyCount), "supplyCount");
	changes += testStateArrayProp(pre->embargoTokens, post->embargoTokens, sizeof(pre->embargoTokens), "embargoTokens");
	changes += testStateIntProp(pre->outpostPlayed, post->outpostPlayed, "outpostPlayed");
	changes += testStateIntProp(pre->whoseTurn, post->whoseTurn, "whoseTurn");
	changes += testStateIntProp(pre->phase, post->phase, "phase");
	changes += testStateIntProp(pre->numActions, post->numActions, "numActions");
	changes += testStateIntProp(pre->coins, post->coins, "coins");
	changes += testStateIntProp(pre->numBuys, post->numBuys, "numBuys");
	changes += testStateArrayProp(pre->playedCards, post->playedCards, sizeof(pre->playedCards), "playedCards");
	changes += testStateIntProp(pre->playedCardCount, post->playedCardCount, "playedCardCount");

	for( int i=0; i < numPlayers; i++){

		changes += testStateIntProp(pre->deckCount[i], post->deckCount[i], "deckCount");
		changes += testStateArrayProp(pre->deck[i], post->deck[i], sizeof(pre->deck[i]), "player deck");

		if ( i != player ){
			changes += testStateIntProp(pre->handCount[i], post->handCount[i], "other player handCount");
			changes += testStateArrayProp(pre->hand[i], post->hand[i], sizeof(pre->hand[i]), "other player hand");

			changes += testStateIntProp(pre->discardCount[i], post->discardCount[i], "other player discardCount");
			changes += testStateArrayProp(pre->discard[i], post->discard[i], sizeof(pre->discard[i]), "other player discard");
		}
	}

	return changes;
} 