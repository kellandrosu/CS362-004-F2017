/* -----------------------------------------------------------------------
* Author: Charlotte Murphy
* Description: CS 362-400, Assignment 4, random testing - Remodel
* Tests expected execution of Dominion Remodel card given random inputs
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

void initializePlayerCards(struct gameState *g, int p, int* h, int* c1, int* c2);
void testRemodel(struct gameState *post, int p, int h, int c1, int c2, long int seed);

int main() {
	int remodelIdx, handPos, choice1, choice2, i, w, p;
	long int seed;
	int numPlayers = 2;
	// all kingdom cards except remodel
	int kCards[19] = { adventurer, council_room, feast, gardens, mine, smithy, village, baron,
		great_hall, minion, steward, tribute, ambassador, cutpurse, embargo,
		outpost, salvager, sea_hag, treasure_map };
	int k[10];
	struct gameState G;

	srand(time(NULL));

	for (w = 0; w < 10000; w++)
	{
		SelectStream(rand() % 999999999);
		PutSeed(rand() % 999999999);
		GetSeed(&seed);
		//printf("Seed = %ld\n", seed);

		// shuffle supply cards array (all kingdom cards, except adventurer)
		for (i = 0; i < 19; i++)
		{
			int temp = kCards[i];
			int randIdx = floor(Random() * 18);
			kCards[i] = kCards[randIdx];
			kCards[randIdx] = temp;
		}

		// initialize kingdom supply piles with the first 10 supply cards
		for (i = 0; i < 10; i++)
		{
			k[i] = kCards[i];
		}

		// replace a random kingdom card supply pile with remodel
		remodelIdx = floor(Random() * 10);
		k[remodelIdx] = remodel;

		// initialize game state
		initializeGame(numPlayers, k, seed, &G);

		// random whose turn
		p = floor(Random() * (numPlayers - 1));
		G.whoseTurn = p;

		initializePlayerCards(&G, p, &handPos, &choice1, &choice2);

		testRemodel(&G, p, handPos, choice1, choice2, seed);
	}

	return 0;
}

void initializePlayerCards(struct gameState *g, int p, int* h, int *c1, int *c2)
{
	int k, card, count = 0;
	// random deck count, discard count, hand count
	g->deckCount[p] = floor(Random() * MAX_DECK);
	g->discardCount[p] = floor(Random() * MAX_DECK);
	while (count < 1) {
		count = floor(Random() * MAX_HAND);
	}
	g->handCount[p] = count;

	// Random cards in hand
	for (k = 0; k < g->handCount[p]; k++) {
		card = floor(Random() * 26);
		while (g->supplyCount[card] < 1) {
			card = floor(Random() * 26);
		}
		g->hand[p][k] = card;
	}

	// set remodel hand position
	*h = floor(Random() * g->handCount[p]);
	g->hand[p][*h] = remodel;

	// set hand position for choice 1
	*c1 = floor(Random() * g->handCount[p]);

	// set supply index for choice 2
	*c2 = floor(Random() * 26);
}

void testRemodel(struct gameState *post, int p, int h, int c1, int c2, long int seed)
{
	struct gameState pre;
	memcpy(&pre, post, sizeof(struct gameState));

	int bonus = 0;
	int r, s;
	int failedTests = 0;

	r = cardEffect(remodel, c1, c2, 0, post, h, &bonus);

	// deckCount does not change
	if (pre.deckCount[p] != post->deckCount[p]) {
		//printf("Remodel: FAIL Deck count unchanged.\n");
		failedTests++;
	}

	// if handcount == 1
	if (pre.handCount[p] == 1) {
		// If you do not have a card to trash, you do not gain one.
		if (pre.discardCount[p] + 2 == post->deckCount[p]) {
		//printf("Remodel: FAIL No card to trash, no card gained");
		failedTests++;
		}
		// discardCount + 1
		if (pre.discardCount[p] + 1 != post->deckCount[p]) {
			//printf("Remodel: FAIL Only once card added to discard pile when no card to trash");
			failedTests++;
		}
		//	no supply change
		for (s = 0; s <= 26; s++) {
			if (pre.supplyCount[s] != post->supplyCount[s]) {
				//printf("Remodel: FAIL No change to supply piles when no card to trash.\n");
				failedTests++;
				break;
			}
		}
		//	hand count -1 = 0
		if (post->handCount != 0) {
			//printf("Remodel: FAIL Hand count should = 0 after playing remodel when it is player's only card.\n");
			failedTests++;
		}
	}	
		
	// else if handcount > 1
	if (pre.handCount[p] > 1) {
		//	handcount -2
		if (pre.handCount[p] - 2 != post->handCount[p]) {
			//printf("Remodel: FAIL Hand count decrease by two.\n");
			failedTests++;
		}
		//	discardcount +2
		if (pre.discardCount[p] + 2 != post->deckCount[p]) {
			//printf("Remodel: FAIL Two cards added to discard pile.\n");
			failedTests++;
		}
		
		//	remodel card is last card discarded
		if (post->discard[p][post->discardCount[p] - 1] != remodel) {
			//printf("Remodel: FAIL Remodel card last card discarded.\n");
			failedTests++;
		}
		//	card effect returns -1  when card gained supply count == 0
		if (pre.supplyCount[c2] == 0 && r != -1) {
			//printf("Remodel: FAIL Return -1 when card gained supply count is 0.\n");
			failedTests++;
		}
		//  card effect returns -1 when trashed card is the same as card played
		if (h == c1 && r != -1) {
			//printf("Remodel: FAIL Return -1 when trashed card is the same as card played.\n");
			failedTests++;
		}
		//	The c1.cost + 2 <= c2.cost.
		//  card effect returns -1 when card gained costs more than 2 more coins than card trashed
		if ((getCost(pre.hand[p][c1]) + 2) < getCost(c2) && r != -1) {
			//printf("Remodel: FAIL Return -1 when trashed card is the same as card played.\n");
			failedTests++;
		}
		//	c2 is in discard
		if (post->discard[p][post->discardCount[p] - 2] != c2) {
			//printf("Remodel: FAIL Gained card added to discard.\n");
			failedTests++;
		}
		//	one supply pile -1 and all other supply piles unchanged
		int gainedCard = -1;
		int changedPiles = 0;
		for (s = 0; s <= 26; s++) {
			if (pre.supplyCount[s] - 1 == post->supplyCount[s]) {
				gainedCard = pre.supplyCount[s];
				changedPiles++;
			}
			else if (pre.supplyCount[s] - 1 > post->supplyCount[s]) {
				//printf("Remodel: FAIL Supply count only reduced by one.\n");
				failedTests++;
			}
		}
		// card gained is same as choice2
		if (gainedCard >= 0 && changedPiles == 1) {
			if (gainedCard != c2) {
				//printf("Remodel: FAIL Card gained is same as choice 2.\n");
				failedTests++;
			}
		}
		// One supply pile reduced by 1
		if (changedPiles != 1) {
			//printf("Remodel: FAIL One supply pile reduced by 1.\n");
			failedTests++;
		}
	}
	
	
	if (failedTests > 0) {
		printf("Remodel Card: seed = %ld,\t%d test(s) failed\n", seed, failedTests);
	}

	if (failedTests > 0) {
		printf("Adventurer Card: seed = %ld,\t%d test(s) failed\n", seed, failedTests);
	}
	
/*
	if (failedTests > 0 && failedTests != 5){
	printf("*******************************************************************************\n");
	printf("Adventurer Card: seed = %ld,\t%d test(s) failed\n", seed, failedTests);
	}
	else if (failedTests > 0) {
	printf("Adventurer Card: seed = %ld,\t%d test(s) failed\n", seed, failedTests);
	}

	if (failedTests == 0) {
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

	}
*/	
	return;
}