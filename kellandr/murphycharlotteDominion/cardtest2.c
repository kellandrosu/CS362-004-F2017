/* 	Author: 	Andrius Kelly
	Date:		10/13/2017
	Description: Unit test for "adventurer" card
	Player should draw until two treasure cards are drawn, 
	then remove adventurer card from hand and add to played cards array
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
void adventurerGameStateHelper(struct gameState *G, int handPos);
int assertDeckAndDiscard(struct gameState *G, struct gameState *pre, int p);

int main(void){

	//gamestate vars
	int seed = 1000;
	int numPlayers = 2;    
    int k[10] = {adventurer, gardens, embargo, village, minion, mine, cutpurse, sea_hag, tribute, smithy};
    struct gameState G;
    struct gameState pre;

   //vars for testing
    int handPos;
    int treasureCard = copper;
    int choice = 0;
    int deckCount = 10;
    int testCount = 0;
    int failures = 0;
    int p = 0;

	deckCount = 10;

	printf ("Testing adventurer card effect():\n");

	//iterate through players
	for( p = 0; p < numPlayers; p++){
	
		//choice shouldn't matter, so increment to find faults
		choice++;	

		//test for copper silver and gold treasure cards;
		for( treasureCard = copper; treasureCard <= gold; treasureCard++){
			//initialize new game
			initializeGame(numPlayers, k, seed, &G);
			G.whoseTurn = p;

			//fill deck and discard with non treasure cards
			for(int i = 0; i < deckCount; i++){
				G.deck[p][i] = k[i];
				G.discard[p][i] = k[i];
			}			
			G.deckCount[p] = deckCount;
			G.discardCount[p] = deckCount;
		//TEST 1
			if(NOISY_TEST) printf(" Testing adventurer with two treasures on top of deck\n");		
			//add two treasures on top of deck;
			G.deck[p][deckCount - 1] = treasureCard;
			G.deck[p][deckCount - 2] = treasureCard;
			//add adventurer to hand
			handPos = 0;
			G.hand[p][handPos] = adventurer; 
			//copy gamestate
			memcpy(&pre, &G, sizeof(struct gameState));
			//run cardeffect
			cardEffect( adventurer,  choice,  choice, choice, &G,  handPos, &choice);
			//manually change pre gamestate to expected
			adventurerGameStateHelper(&pre, handPos);
			//check for failures
			if (assertGameState( &pre, &G))
				failures++;
			testCount++;

			//decksize and discardsize should be equal
			if (assertDeckAndDiscard(&G, &pre, p)) failures++;



		//TEST 2
			if(NOISY_TEST) printf(" Testing adventurer with two treasures at deck index 2 and 4\n");
			//reset gamestate G to expected
			memcpy(&G, &pre, sizeof(struct gameState));
			//put two treasure card in middle of deck deck
			G.deck[p][2] = treasureCard;
			G.deck[p][4] = treasureCard;				
			//add adventurer to hand
			handPos = 1;
			G.hand[p][handPos] = adventurer;
			//copy gamestate
			memcpy(&pre, &G, sizeof(struct gameState));
			//run cardeffect
			cardEffect( adventurer,  choice,  choice, choice, &G,  handPos, &choice);
			//manually change pre gamestate to expected
			adventurerGameStateHelper(&pre, handPos);
			//check for failures
			if (assertGameState( &pre, &G))
				failures++;
			testCount++;
			//decksize and discardsize should be equal
			if (assertDeckAndDiscard(&G, &pre, p)) failures++;


		//TEST 3
			//put one in deck one in discard
			if(NOISY_TEST) printf(" Testing adventurer with treasure at deck index 3 and discard index 5\n");
			//reset gamestate G to expected
			memcpy(&G, &pre, sizeof(struct gameState));

			//repopulate deck with cards
			for(int i = 0; i < deckCount; i++){
				G.deck[p][i] = k[i];
			}			
			G.deckCount[p] = deckCount;

			//put one in deck one in discard
			G.deck[p][3] = treasureCard;
			G.discard[p][5] = treasureCard;
			//add adventurer to hand
			handPos = 2;
			G.hand[p][handPos] = adventurer;				
			//copy gamestate
			memcpy(&pre, &G, sizeof(struct gameState));

			//run cardeffect
			cardEffect( adventurer,  choice,  choice, choice, &G,  handPos, &choice);
			//manually change pre gamestate to expected

			adventurerGameStateHelper(&pre, handPos);

			//check for failures
			if (assertGameState( &pre, &G))
				failures++;
			testCount++;

		//TEST 4
			//put both treasures in discard
			if(NOISY_TEST) printf(" Testing adventurer with treasure at discard index 3 and 5\n");
			//reset gamestate G to expected
			memcpy(&G, &pre, sizeof(struct gameState));

			//repopulate discard with cards
			for(int i = 0; i < deckCount; i++){
				G.discard[p][i] = k[i];
			}			
			G.discardCount[p] = deckCount;

			//put one in deck one in discard
			G.discard[p][3] = treasureCard;
			G.discard[p][5] = treasureCard;

			//add adventurer to hand
			handPos = 3;
			G.hand[p][handPos] = adventurer;
			
			//copy gamestate
			memcpy(&pre, &G, sizeof(struct gameState));
			//run cardeffect
			cardEffect( adventurer,  choice,  choice, choice, &G,  handPos, &choice);
			//manually change pre gamestate to expected
			adventurerGameStateHelper(&pre, handPos);
			//check for failures
			if (assertGameState( &pre, &G))
				failures++;
			testCount++;

		//TEST 4
			//no treasures
			if(NOISY_TEST) printf(" Testing adventurer no treasure cards (should do nothing)\n");
			//reset gamestate G to expected
			memcpy(&G, &pre, sizeof(struct gameState));

			//repopulate discard with cards
			for(int i = 0; i < deckCount; i++){
				G.discard[p][i] = k[i];
			}			
			G.discardCount[p] = deckCount;
			//add adventurer to hand
			handPos = 4;
			G.hand[p][handPos] = adventurer;				
			//copy gamestate
			memcpy(&pre, &G, sizeof(struct gameState));
			//run cardeffect
			cardEffect( adventurer,  choice,  choice, choice, &G,  handPos, &choice);
			//manually change pre gamestate to expected
			adventurerGameStateHelper(&pre, handPos);
			//check for failures
			if (assertGameState( &pre, &G))
				failures++;
			testCount++;
			//decksize and discardsize should be equal
			if (assertDeckAndDiscard(&G, &pre, p)) failures++;

		}

	}

	printf("%d Total Tests  %d Failures for adventurer\n\n", testCount, failures);

	return 0;
}


//----------------------  FUNCTIONS  ----------------------------

int assertDeckAndDiscard(struct gameState *G, struct gameState *pre, int p){
	int failures = 0;
	if (G->deckCount[p] != pre->deckCount[p] ){
		failures++;
		if (NOISY_TEST) printf("  FAIL: gameState.deckCount = %d expected %d\n", G->deckCount[p], pre->deckCount[p] );	
	}
	if (G->discardCount[p] != pre->discardCount[p] ){
		failures++;
		if (NOISY_TEST) printf("  FAIL: gameState.discardCount = %d expected %d\n", G->discardCount[p], pre->discardCount[p] );	
	}
	return failures;
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

	}

	return changes;
} 
