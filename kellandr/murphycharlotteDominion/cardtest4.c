/* 	Author: 	Andrius Kelly
	Date:		10/13/2017
	Description: Unit test for "sea hag" card
	Every other player discards their top deck card and gains a
	curse to the top of their deck
*/

#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"

//set to 0 to remove prints on every test
#define NOISY_TEST 1

int assertGameState(struct gameState *pre, struct gameState *post); 
void seahagHelper(struct gameState *G, int handPos);
void printPlayerInfo(struct gameState *G);

int main(void){

	//gamestate vars
	int seed = 1000;
	int numPlayers = 4;    
    int k[10] = {adventurer, gardens, embargo, village, minion, mine, cutpurse, sea_hag, tribute, smithy};
    struct gameState G;
    struct gameState pre;

    int handPos;
    int choice = 0;
    int testCount = 0;
    int failures = 0;
    int p = 0;

    printf ("\nTesting sea_hag card effect():\n");

    initializeGame(numPlayers, k, seed, &G);

	//test 4 player game
    int numCards = 5;

	//choice shouldn't matter, so increment to find faults
	
	if(NOISY_TEST) printf(" Testing sea_hag for player %d in %d player game.\n", p, numPlayers);

//TEST 1 - testing seahag at end of hand of hand for player 0
	
	//fill hand
	G.whoseTurn = p;
	for(int i = 0; i < numCards; i++){
		G.hand[p][ i ] = i + great_hall; 
	}
	// add sea_hag
	G.hand[p][ numCards-1 ] = sea_hag;
	G.handCount[p] = numCards;

	handPos = G.handCount[p] - 1;

	memcpy(&pre, &G, sizeof(struct gameState));
	if (NOISY_TEST){
		printf("  Before: Discardtop: %d, Decktop: %d\n", 
		G.discard[1][G.discardCount[1]-1], G.deck[1][G.deckCount[1]-1]);
	}
	cardEffect( sea_hag,  choice,  choice, choice, &G,  handPos, &choice);
	seahagHelper(&pre,  handPos);
	if (NOISY_TEST){
		printf("  After Discardtop expected: %d, actual: %d Decktop expected: %d, actual: %d\n", 
		pre.discard[1][pre.discardCount[1]-1], G.discard[1][G.discardCount[1]-1],
		pre.deck[1][pre.deckCount[1]-1], G.deck[1][G.deckCount[1]-1]);
	}
	if (assertGameState(&pre, &G)) failures++;
	testCount++;


//TEST 2 - testing seahag at front of hand for player 2
	if(NOISY_TEST) printf(" Testing sea_hag for player %d in %d player game. Curse count set to 2\n", p, numPlayers);
	memcpy(&G, &pre, sizeof(struct gameState)); //reset G to pre
	G.supplyCount[curse] = 2;

	//fill hand
	G.whoseTurn = p = 2;
	for(int i = 0; i < numCards; i++){
		G.hand[p][ i ] = i + great_hall; 
	}
	// add sea_hag
	G.hand[p][ numCards-1 ] = sea_hag;
	G.handCount[p] = numCards;

	handPos = G.handCount[p] - 1;

	for(int i = 0; i < numCards; i++){
		G.hand[p][ i ] = i + great_hall; 
	}
	handPos = 0;
	G.hand[p][ handPos ] = sea_hag;

	memcpy(&pre, &G, sizeof(struct gameState));
	if (NOISY_TEST){
		printf("  Before: Discardtop: %d, Decktop: %d\n", 
		G.discard[1][G.discardCount[1]-1], G.deck[1][G.deckCount[1]-1]);
	}
	cardEffect( sea_hag,  choice,  choice, choice, &G,  handPos, &choice);
	seahagHelper(&pre,  handPos);
	if (NOISY_TEST){
		printf("  After Discardtop expected: %d, actual: %d Decktop expected: %d, actual: %d\n", 
		pre.discard[1][pre.discardCount[1]-1], G.discard[1][G.discardCount[1]-1],
		pre.deck[1][pre.deckCount[1]-1], G.deck[1][G.deckCount[1]-1]);
	}

	if (assertGameState(&pre, &G)) failures++;;
	testCount++;



//TEST 3:  2 player game
	numPlayers = 2;
	initializeGame(numPlayers, k, seed, &G);
	
	if(NOISY_TEST) printf(" Testing sea_hag for player %d in %d player game.  player 2 has empty deck.\n", p, numPlayers);
	
	//move Second player's deck to discard
	for(int i=0; i < G.deckCount[1]; i++){
		G.discard[1][i] = G.deck[1][i];
	}
		G.discardCount[1] = G.deckCount[1];
		G.deckCount[1] = 0;

	for( p = 0; p < numPlayers; p++){

		//choice shouldn't matter, so increment to find faults
		choice++;	
		//fill hand
		G.whoseTurn = p = 2;
		for(int i = 0; i < numCards; i++){
			G.hand[p][ i ] = i + great_hall; 
		}
		// add sea_hag
		G.hand[p][ numCards-1 ] = sea_hag;
		G.handCount[p] = numCards;

		memcpy(&pre, &G, sizeof(struct gameState));
		if (NOISY_TEST){
		printf("  Before: Discardtop: %d, Decktop: %d\n", 
		G.discard[1][G.discardCount[1]-1], G.deck[1][G.deckCount[1]-1]);
		}
		cardEffect( sea_hag,  choice,  choice, choice, &G,  handPos, &choice);
		seahagHelper(&pre,  handPos);
		if (NOISY_TEST){	
			printf("  After Discardtop expected: %d, actual: %d Decktop expected: %d, actual: %d\n", 
			pre.discard[1][pre.discardCount[1]-1], G.discard[1][G.discardCount[1]-1],
			pre.deck[1][pre.deckCount[1]-1], G.deck[1][G.deckCount[1]-1]);
		}

		if (assertGameState(&pre, &G)) failures++;
		testCount++;

	}	

	printf("%d Total Tests  %d Failures for sea_hag\n\n", testCount, failures);

	return 0;
}


//-------------------------- FUNCTIONS -------------------------------

//for debugging
void printPlayerInfo(struct gameState *G){
	for(int i=0; i < G->numPlayers; i++){
		printf("player %d hand: %d, deck: %d, discard: %d\n", 
			i, G->handCount[i], G->deckCount[i], G->discardCount[i]);
	}
	printf("Curses: %d\n", G->supplyCount[curse]);
}

//creates expected gamestate for seahag effect
void seahagHelper(struct gameState *G, int handPos)
{
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
	changes += testStateInt(pre->supplyCount[curse], post->supplyCount[curse], "supplyCount[curse]");
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

//		sprintf(buffer, "discard[] for player %d", i);
//		changes += testStateArray(pre->discard[i], post->discard[i], pre->discardCount[i], buffer);

		sprintf(buffer, "deckCount for player %d", i);
		changes += testStateInt(pre->deckCount[i], post->deckCount[i], buffer);

//		sprintf(buffer, "deck[] for player %d", i);
//		changes += testStateArray( pre->deck[i], post->deck[i], pre->deckCount[i], buffer);
	}

	return changes;
} 
