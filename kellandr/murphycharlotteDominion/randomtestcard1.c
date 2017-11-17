/* -----------------------------------------------------------------------
* Demonstration of how to write unit tests for dominion-base
* Include the following lines in your makefile:
*
* cardtest1: cardtest1.c dominion.o rngs.o
*      gcc -o cardtest1 -g  cardtest1.c dominion.o rngs.o $(CFLAGS)
* -----------------------------------------------------------------------
*/
/*
Smithy test requirements

1. Current player should receive exact 3 cards.

2. 3 cards should come from his own pile.

3. No state change should occur for other players.

4. No state change should occur to the victory card piles and kingdom card piles.

5. Smithy card is dicarded

*/

#include "dominion.h"
#include "dominion_helpers.h"
#include "rngs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void initializePlayerCards(struct gameState *g, int p, int* h);

// Test Smithy card, based on cardtest4.c example provided
int main()
{
	int i, s, numPlayers, p, failedTests;
	int handpos = 0, choice1 = 0, choice2 = 0, choice3 = 0, bonus = 0;
	long int seed;
	struct gameState G, testG;
	int k[10] = { adventurer, embargo, village, minion, mine, cutpurse,
		sea_hag, tribute, smithy, council_room };

	srand(time(NULL));

	for (i = 0; i < 10000; i++)
	{
		SelectStream(rand() % 999999999);
		PutSeed(rand() % 999999999);
		GetSeed(&seed);

		// random number of players
		numPlayers = floor(Random() * MAX_PLAYERS);

		// initialize game state
		initializeGame(numPlayers, k, seed, &G);

		// random whose turn
		p = floor(Random() * (numPlayers - 1));
		G.whoseTurn = p;

		failedTests = 0;

		memcpy(&testG, &G, sizeof(struct gameState));
		cardEffect(smithy, choice1, choice2, choice3, &testG, handpos, &bonus);

		// ----------- TEST 1: Current player should receive exactly 3 cards. --------------
		// +3 new cards -1 discarded card
		if ((G.handCount[p] + 3 - 1) != (testG.handCount[p])) {
			//printf("Smithy: FAIL Current player receives exactly 3 cards.\n");
			failedTests++;
		}

		// ----------- TEST 2: 3 cards should come from his own pile. --------------
		// -3 for cards drawn from deck and discard(if needed) and +1 for discarded smithy
		if ((G.deckCount[p] + G.discardCount[p] - 3 + 1) != (testG.deckCount[p] + testG.discardCount[p])) {
			//printf("Smithy: FAIL Player's deck and discard pile has  - 3 + 1 less cards .\n");
			failedTests++;
		}

		// ----------- TEST 4: No state change should occur to the victory card piles or kingdom card piles. --------------
		for (s = 0; s <= 26; s++) {
			if (G.supplyCount[s] != testG.supplyCount[s]) {
				// printf("Smithy: FAIL No change to supply piles.\n");
				failedTests++;
				break;
			}
		}

		// ----------- TEST 5: Discard count increased by one. --------------
		if (G.discardCount[p] + 1 != testG.discardCount[p]) {
			//printf("Smithy: FAIL Discard count increased by one.\n");
			failedTests++;
		}

		// ----------- TEST 6: Smithy card is dicarded. --------------
		if ((testG.discard[p][testG.discardCount[p] - 1]) != smithy) {
			//printf("Smithy: FAIL Smithy card is discarded.\n");
			failedTests++;
		}

		if (failedTests > 0) {
			printf("Adventurer Card: seed = %ld,\t%d test(s) failed\n", seed, failedTests);
		}

		/*
		if (failedTests > 0 && failedTests != 3) {
			printf("Adventurer Card: seed = %ld,\t%d test(s) failed\n\n", seed, failedTests);
		}
		*/
		
	}

	return 0;
}

void initializePlayerCards(struct gameState *g, int p, int* h)
{
	int i, j, k, count = 0;
	// random deck count, discard count, hand count
	g->deckCount[p] = floor(Random() * MAX_DECK);
	g->discardCount[p] = floor(Random() * MAX_DECK);
	while (count < 1) {
		count = floor(Random() * MAX_HAND);
	}
	g->handCount[p] = count;

	for (i = 0; i < g->deckCount[p]; i++)
		g->deck[p][i] = village;
	for (j = 0; j < g->discardCount[p]; i++)
		g->discard[p][j] = village;
	for (k = 0; k < g->handCount[p]; k++)
		g->hand[p][k] = village;

	// add adventurer to random hand position
	*h = floor(Random() * g->handCount[p]);
	g->hand[p][*h] = adventurer;

	return;
}

