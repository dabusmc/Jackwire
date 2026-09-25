#ifndef HAND_H
#define HAND_H

#include <stdint.h>

struct Hand
{
    uint8_t* cards;
    uint32_t card_count;
};
typedef struct Hand Hand;

void handCreate(Hand** hand);
void handDestroy(Hand* hand);

void handAddCard(Hand* hand, uint8_t card);

#endif