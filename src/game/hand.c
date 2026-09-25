#include "hand.h"

#include <stdlib.h>

#define MAX_CARDS 52

void handCreate(Hand** hand)
{
    Hand* h = (Hand*)malloc(sizeof(Hand));
    if(h == NULL)
    {
        *hand = NULL;
        return;
    }

    h->card_count = 0;
    h->cards = (uint8_t*)malloc(sizeof(uint8_t) * MAX_CARDS);
    if(h->cards == NULL)
    {
        free(h);
        *hand = NULL;
        return;
    }

    *hand = h;
}

void handDestroy(Hand* hand)
{
    free(hand->cards);
    free(hand);
}

void handAddCard(Hand* hand, uint8_t card)
{
    hand->cards[hand->card_count] = card;
    hand->card_count += 1;
}