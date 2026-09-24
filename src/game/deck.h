#ifndef DECK_H
#define DECK_H

#include <stdint.h>

struct Deck
{
    uint8_t* drawn_cards;
    uint32_t drawn_card_count;
};
typedef struct Deck Deck;

void deckCreate(Deck** deck);
void deckDestroy(Deck* deck);

uint8_t deckDrawNext(Deck* deck);

char* cardGetSuitName(uint8_t card);
char* cardGetValueName(uint8_t card);

#endif