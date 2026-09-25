#include "deck.h"

#include <stdlib.h>
#include <stdio.h>

#define CARD_COUNT 52

void deckCreate(Deck** deck)
{
    Deck* d = (Deck*)malloc(sizeof(Deck));
    if(d == NULL)
    {
        *deck = NULL;
        return;
    }

    d->drawn_card_count = 0;
    d->drawn_cards = (uint8_t*)malloc(sizeof(uint8_t) * CARD_COUNT);
    if(d->drawn_cards == NULL)
    {
        free(d);
        *deck = NULL;
        return;
    }

    *deck = d;
}

void deckDestroy(Deck* deck)
{
    free(deck->drawn_cards);
    free(deck);
}

// Unused  Suit     Type
//    0 0   0 0  0 0 0 0
uint8_t deckDrawNext(Deck* deck)
{
    int card_is_valid = 0;
    uint8_t drawn;
    while(!card_is_valid)
    {
        drawn = (uint8_t)rand();
        drawn = drawn & 0b00111111;

        uint8_t val = drawn & 0b00001111;
        if(val <= 14 && val > 1)
        {
            card_is_valid = 1;
        }

        for(int i = 0; i < deck->drawn_card_count; i++)
        {
            if(deck->drawn_cards[i] == drawn)
            {
                card_is_valid = 0;
                continue;
            }
        }
    }

    deck->drawn_cards[deck->drawn_card_count] = drawn;
    deck->drawn_card_count += 1;
    return drawn;
}

// Suits
// Clubs -> 00 (0)
// Hearts -> 01 (1)
// Diamonds -> 10 (2)
// Spades -> 11 (3)
char* cardGetSuitName(uint8_t card)
{
    uint8_t suit = (card >> 4) & 0b0011;
    switch(suit)
    {
        case 0:
            return "Clubs";
        case 1:
            return "Hearts";
        case 2:
            return "Diamonds";
        case 3:
            return "Spades";
        default:
            return "Unknown";
    }
}

// Values
// 1-10 -> 1-10
// Jacks -> 11
// Queens -> 12
// Kings -> 13
// Ace -> 14
char* cardGetValueName(uint8_t card)
{
    uint8_t value = card & 0b00001111;
    switch(value)
    {
        case 1:
            return "1";
        case 2:
            return "2";
        case 3:
            return "3";
        case 4:
            return "4";
        case 5:
            return "5";
        case 6:
            return "6";
        case 7:
            return "7";
        case 8:
            return "8";
        case 9:
            return "9";
        case 10:
            return "10";
        case 11:
            return "Jack";
        case 12:
            return "Queen";
        case 13:
            return "King";
        case 14:
            return "Ace";
        default:
            return "Unknown";
    }
}