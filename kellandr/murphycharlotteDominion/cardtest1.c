/* 	Author: 	Andrius Kelly
	Date:		10/13/2017
	Description: Unit test for "smithy" card
	Player should draw three cards, then remove smithy card from hand and add to played cards array
*/

#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "rngs.h"

//set to 1 to remove prints on every test
#define NOISY_TEST 1

int assertGameState(struct gameState *pre, struct gameState *post); 
void smithyGameStateHelper(struct gameState* pre, int handPos);


//compare function adapted from http://www.cplusplus.com/reference/cstdlib/qsort/
int comp (const void * a, const void * b) {
  if ( *(int*)a  <  *(int*)b ) return -1;
  if ( *(int*)a  >  *(int*)b ) return 1;
  return 0;
}


int main(void){

	//gamestate vars
	int seed = 1000;
	int numPlayers = 2;    
    int k[10] = {adventurer, gardens, embargo, village, minion, mine, cutpurse, sea_hag, tribute, smithy};
    struct gameState G;
    struct gameState pre;

   //vars for testing
    int handPos;
    int handCount;
    int deckCount;
    int p = 0;
    int bonus = 0;
    
    int testCount;
    int failures;
    testCount = failures = 0;

	//initialize new game
	initializeGame(numPlayers, k, seed, &G);

	printf ("Testing smithy card effect():\n");

	//iterate through players
	for(p=0; p < numPlayers; p++){

		G.whoseTurn = p;
		
		//choices and bonus should have no effect, so iterate various choice from -1 to 1s
		for(int choice=-1; choice < 1; choice++){

			G.numActions = 5;

			//initialize hand for only smithy card
			handCount =  1;
			handPos = 0;
			deckCount = 10;

			G.handCount[p] = handCount;
			G.hand[p][handPos] = smithy;

			//initialize deck with legal cards
			G.deckCount[p] = deckCount;
			for(int i = 0; i < G.deckCount[p]; i++){
				G.deck[p][i] = k[i];
			}

			memcpy(&pre, &G, sizeof(struct gameState));

			cardEffect( smithy,  choice, choice, choice, &G,  handPos, &bonus);

			smithyGameStateHelper(&pre, handPos);

			//NOTE: order of drawing and discarding can effect hand order
			// so need to sort hands before test
			handCount = pre.handCount[p];
			qsort(G.hand[p], handCount, sizeof(int), comp);
			qsort(pre.hand[p], handCount, sizeof(int), comp);

			if (assertGameState(&pre, &G)){
				failures++
			}
			testCount++;

			//make smithy middle card (position = 1) and play it
			handPos = 1;
			pre.hand[p][handPos] = smithy;
			//copy pregamestate to G
			memcpy(&G, &pre, sizeof(struct gameState));

			cardEffect( smithy,  choice, choice, choice, &G,  handPos, &bonus);
			smithyGameStateHelper(&pre, handPos);

			//NOTE: order of drawing and discarding can effect hand order
			// so need to sort hands before test
			handCount = pre.handCount[p];
			qsort(G.hand[p], handCount, sizeof(int), comp);
			qsort(pre.hand[p], handCount, sizeof(int), comp);

			if (assertGameState(&pre, &G)){
				failures++
			}
			testCount++;


		}

	}
	
	printf("%d Tests %d Failures for smithy\n\n", testCount, failures);

	return 0;
}

//changes the pre gamestate to what it should be after smithy is played
void smithyGameStateHelper(struct gameState* pre, int handPos){
		int player = pre->whoseTurn;
		int handCount = pre->handCount[player];
		int deckCount = pre->deckCount[player];
		int card;

		//top three cards of deck should be in hand
		for(int i=0; i < 3; i++){
			card = pre->deck[player][deckCount - 1 - i];
			pre->hand[player][handCount + i] = card;
		}

		//deck size should be 3 less
		deckCount -= 3;
		
		//handsize should be 3 more
		handCount += 3;

		//smithy card should be removed from hand at handPos
		pre->hand[player][handPos] = pre->hand[player][handCount];
		handCount -= 1;

		//smithy should be added to playedCards
		pre->playedCards[pre->playedCardCount] = smithy;
		pre->playedCardCount += 1;

		//set handcount and deckcount states
		pre->deckCount[player] = deckCount;
		pre->handCount[player] = handCount;
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
