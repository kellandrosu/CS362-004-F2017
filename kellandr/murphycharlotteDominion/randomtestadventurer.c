/* -----------------------------------------------------------------------
* Author: Charlotte Murphy
* Description: CS 362-400, Assignment 4, random testing - Adventurer
* Tests expected execution of Dominion Adventurer card given random inputs
* -----------------------------------------------------------------------
*/

#include "dominion.h"
#include "dominion_helpers.h"
#include "rngs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void initializePlayerCards(struct gameState *g, int p, int h);
void testAdventurer(struct gameState *post, int player, int h, long int s);

int main()
{
	int count, numPlayers, i, p;
	//int handPos;
	long int seed;
	int k[10] = { adventurer, council_room, feast, gardens, mine,
		remodel, smithy, village, baron, great_hall };	
	struct gameState G;

	srand(time(NULL));
	
	for (i = 0; i < 5000; i++)
	{
		SelectStream(rand() % 999999999);
		PutSeed(rand() % 999999999);
		GetSeed(&seed);
		//printf("Seed = %ld\n", seed);

		// random number of players
		numPlayers = floor(Random() * MAX_PLAYERS);

		// initialize game state
		initializeGame(numPlayers, k, seed, &G);

		// random whose turn
		p = floor(Random() * (numPlayers - 1));
		G.whoseTurn = p;

		// random deck count, discard count, hand count
		G.deckCount[p] = floor(Random() * MAX_DECK);
		G.discardCount[p] = floor(Random() * MAX_DECK);
		count = 0;
		while (count < 1) {
			count = floor(Random() * MAX_HAND);
		}
		G.handCount[p] = count;

		//handPos = floor(Random() * (G.handCount[p] - 1));
		// initialize random player cards with random numbers of treasure cards and smithy
		// add adventurer to random hand position
		//initializePlayerCards(&G, p, 0);

		//testAdventurer(&G, p, handPos, seed);
	}


	printf("\n\n");
	return 0;
}

void initializePlayerCards(struct gameState *g, int p, int h)
{
	//int i, j, k;
	int i, j, k, m, n;
	int numCopper = MAX_DECK;
	int numSilver = MAX_DECK;
	int numGold = MAX_DECK;
	int temp, randIdx;
	// deck pile
	while (numCopper + numSilver + numGold >= g->deckCount[p] - 1)
	{
		numCopper = floor(Random() * 60);	// 60 = total copper cards in game
		numSilver = floor(Random() * 40);	// 40 = total silver cards in game
		numGold = floor(Random() * 30);		// 30 = total gold cards in game
	}
	for (i = 0; i < numCopper; i++)
		g->deck[p][i] = copper;
	for (j = numCopper; j < numCopper + numSilver; j++)
		g->deck[p][j] = silver;
	for (k = numCopper + numSilver; k < numCopper + numSilver + numGold; k++)
		g->deck[p][k] = gold;
	for (m = numCopper + numSilver + numGold; m < g->deckCount[p]; m++)
		g->deck[p][m] = smithy;

	for (n = 0; n < g->deckCount[p]; n++)
	{
		temp = g->deck[p][n];
		randIdx = floor(Random() * (g->deckCount[p] - 1));
		g->deck[p][n] = g->deck[p][randIdx];
		g->deck[p][randIdx] = temp;
	}

	// discard pile
	numCopper = MAX_DECK;
	numSilver = MAX_DECK;
	numGold = MAX_DECK;
	while (numCopper + numSilver + numGold >= g->discardCount[p] - 1)
	{
		numCopper = floor(Random() * 60);
		numSilver = floor(Random() * 40);
		numGold = floor(Random() * 30);
	}
	for (i = 0; i < numCopper; i++)
		g->discard[p][i] = copper;
	for (j = numCopper; j < numCopper + numSilver; j++)
		g->discard[p][j] = silver;
	for (k = numCopper + numSilver; k < numCopper + numSilver + numGold; k++)
		g->discard[p][k] = gold;
	for (m = numCopper + numSilver + numGold; m < g->discardCount[p]; m++)
		g->discard[p][m] = smithy;

	for (n = 0; n < g->discardCount[p]; n++)
	{
		temp = g->discard[p][n];
		randIdx = floor(Random() * (g->discardCount[p] - 1));
		g->discard[p][n] = g->discard[p][randIdx];
		g->discard[p][randIdx] = temp;
	}

	for (j = 0; j < g->discardCount[p]; j++)
	{
		g->discard[p][j] = copper;
	}

	// hand pile
	numCopper = MAX_DECK;
	numSilver = MAX_DECK;
	numGold = MAX_DECK;
	while (numCopper + numSilver + numGold >= g->handCount[p] - 1)
	{
		numCopper = floor(Random() * 60);
		numSilver = floor(Random() * 40);
		numGold = floor(Random() * 30);
	}

	for (i = 0; i < numCopper; i++)
		g->hand[p][i] = copper;
	for (j = numCopper; j < numCopper + numSilver; j++)
		g->hand[p][j] = silver;
	for (k = numCopper + numSilver; k < numCopper + numSilver + numGold; k++)
		g->hand[p][k] = gold;
	for (m = numCopper + numSilver + numGold; m < g->handCount[p]; m++)
		g->hand[p][m] = smithy;

	for (n = 0; n < g->handCount[p]; n++)
	{
		temp = g->hand[p][n];
		randIdx = floor(Random() * (g->handCount[p] - 1));
		g->hand[p][n] = g->hand[p][randIdx];
		g->hand[p][randIdx] = temp;
	}

	
	g->hand[p][h] = adventurer;
	

	return;
}


// Test Adventurer card, based on cardtest4.c example provided
void testAdventurer(struct gameState *post, int p, int h, long int seed)
{
	struct gameState pre;
	memcpy(&pre, post, sizeof(struct gameState));

	int bonus = 0;
	int newCards, s, i;
	int failedTests = 0;

	cardEffect(adventurer, 0, 0, 0, post, h, &bonus);

	// up to two cards added post - pre + 1 to offset for discarded adventurer card
	newCards = post->handCount[p] - pre.handCount[p] + 1;
	// check is no more than 2 new cards added to hand
	if (newCards > 2) {
		//printf("Adventurer: FAIL no more than two card added to player's hand.\n");
		failedTests++;
	}

	// check if cards added, new cards are treasure cars
	if (newCards > 0) {
		for (i = 1; i <= newCards; i++) {
			if (post->hand[p][post->handCount[p] - i] != copper
				|| post->hand[p][post->handCount[p] - i] != silver
				|| post->hand[p][post->handCount[p] - i] != gold) {
				//printf("Adventurer: FAIL Treasure card added to hand.\n");
				failedTests++;
				break;
			}
		}
	}

	// Check supply piles
	for (s = 0; s <= 26; s++) {
		if (pre.supplyCount[s] != post->supplyCount[s]) {
			//printf("Adventurer: FAIL No change to supply piles.\n");
			failedTests++;
			break;
		}
	}

	// check discard pile
	if (pre.discardCount[p] + 1 != post->discardCount[p]) {
		//printf("Adventurer: FAIL Discard count increased by one.\n");
		failedTests++;
	}

	if (post->discard[p][post->discardCount[p] - 1] != adventurer) {
		//printf("Adventurer: FAIL Adventurer card last card discarded.\n");
		failedTests++;
	}

	if (failedTests > 0) {
		printf("Adventurer Card: seed = %ld,\t%d test(s) failed\n", seed, failedTests);
	}

	/*
	if (failedTests > 0 && failedTests != 4){
		printf("*******************************************************************************\n");
	printf("Adventurer Card: seed = %ld,\t%d test(s) failed\n", seed, failedTests);
	}
	else if (failedTests > 0) {
		printf("Adventurer Card: seed = %ld,\t%d test(s) failed\n", seed, failedTests);
	}
	*/
	return;
}
